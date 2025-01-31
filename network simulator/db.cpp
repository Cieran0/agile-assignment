#include "db.hpp"
#include <iostream>
#include <sstream>
#include <format>
#include "Conversion.hpp"
#include <cmath>
#include <queue>
#include <mutex>
#include <condition_variable>
#include "sim.hpp"

typedef const unsigned char* sqlite_string; 
typedef int64_t BankCurrency;

std::queue<TransactionPromise> transactionQueue;
std::queue<std::string> transactionLogs;
std::mutex queueMutex;
std::mutex transactionLogsMx;
std::condition_variable queueCondition;

Response processTransaction(Transaction transaction, sqlite3*& db) {
    Response response = {.succeeded = NETWORK_ERROR};

    std::string sql_string = std::format("SELECT Balance, Currency, blocked FROM Customer WHERE CardNumber = '{}' AND PIN = '{}';", transaction.cardNumber, transaction.pinNo);

    sqlite3_stmt* stmt = nullptr;
    int exitCode = sqlite3_prepare_v2(db, sql_string.c_str(), -1, &stmt, nullptr);

    if (exitCode != SQLITE_OK) {
        sqlite3_close(db);

        response.succeeded = DATABASE_ERROR;
        return response;
    }

    if (sqlite3_step(stmt) != SQLITE_ROW) {
        sqlite3_finalize(stmt);

        response.succeeded = INCORRECT_PIN;
        return response;
    }

    BankCurrency currentBankBalance = sqlite3_column_int64(stmt, 0);
    sqlite_string bankCurrencyString = sqlite3_column_text(stmt, 1);
    int blocked = sqlite3_column_int(stmt, 2);

    if(blocked != 0) {
        response.succeeded = CARD_BLOCKED;
        sqlite3_finalize(stmt);
        return response;
    }

    Currency bankCurrency = currencyCodeToEnum.at((const char*)bankCurrencyString);

    
    DecimalPosition dotPosition = currencyDotPosition.at(transaction.currency);
    transaction.amount *= pow(10, dotPosition);

    BankCurrency transactionAmount = 0;
    if(ConvertCurrency(transaction.currency, bankCurrency, transaction.amount, transactionAmount, Rounding::UP)) {
        response.succeeded = DATABASE_ERROR;
        sqlite3_finalize(stmt);
        return response;
    } 


    AtmCurrency newBalance;
    if(ConvertCurrency(bankCurrency, transaction.currency, currentBankBalance, newBalance, Rounding::DOWN)) {
        response.succeeded = DATABASE_ERROR;
        sqlite3_finalize(stmt);
        return response;
    }

    if (transaction.type == PIN_CHECK || transaction.type == BALANCE_CHECK) {
        response.newBalance = newBalance;
        response.dotPosition = currencyDotPosition.at(transaction.currency);
        response.succeeded = SUCCESS;
        sqlite3_finalize(stmt);
        return response;
    }

    if (transactionAmount > currentBankBalance && transaction.type == WITHDRAWL) {
        response.succeeded = INSUFFICIENT_FUNDS;
        response.dotPosition = currencyDotPosition.at(transaction.currency);
        sqlite3_finalize(stmt);
        return response;
    }

    if(transaction.type == DEPOSIT) {
        transactionAmount *= -1;
    }

    BankCurrency newBankBalance = currentBankBalance - transactionAmount;
    std::string update_sql = std::format("UPDATE Customer SET Balance = {} WHERE CardNumber = '{}';", newBankBalance, transaction.cardNumber);

    exitCode = sqlite3_exec(db, update_sql.c_str(), nullptr, nullptr, nullptr);
    if (exitCode != SQLITE_OK) {
        response.succeeded = DATABASE_ERROR;
        sqlite3_finalize(stmt);
        return response;
    }
    
    if(ConvertCurrency(bankCurrency, transaction.currency, newBankBalance, newBalance, Rounding::DOWN)) {
        response.succeeded = DATABASE_ERROR;
        sqlite3_finalize(stmt);
        return response;
    }

    response.newBalance = newBalance;
    response.dotPosition = currencyDotPosition.at(transaction.currency);
    response.atmCurrency = transaction.currency;
    response.succeeded = ResponseType::SUCCESS;

    sqlite3_finalize(stmt);
    return response;
}

std::future<Response> enqueueTransaction(Transaction transaction) {
    std::promise<Response> promise;
    std::future<Response> future = promise.get_future();
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        transactionQueue.emplace(transaction, std::move(promise));
    }

    queueCondition.notify_one();
    return future;
}

void enqueueTransactionLog(std::string log_sql) {
    {
        std::lock_guard<std::mutex> lock(transactionLogsMx);
        transactionLogs.emplace(log_sql);
    }
}

void emptyTransactionLogs(sqlite3* db) {
    while (!transactionLogs.empty()) {
        std::string log_sql;
        {
            std::lock_guard<std::mutex> lock(transactionLogsMx);
            log_sql = std::move(transactionLogs.front());
            transactionLogs.pop();
        }

        char* errMsg = nullptr;
        int rc = sqlite3_exec(db, log_sql.c_str(), nullptr, nullptr, &errMsg);
        if (rc != SQLITE_OK) {
            sqlite3_free(errMsg);
        }
    }
}

void processTransactionQueue() {
    sqlite3* db;

    if (sqlite3_open(db_file, &db) != SQLITE_OK) {
        std::cerr << "Failed to open the database in the worker thread: " 
                  << sqlite3_errmsg(db) << std::endl;
        return;
    }

    if(initConversionRates(db) != 0 || initCurrencyPositions(db) !=0){
        std::cerr << "failed to initialise" << std::endl;
        return;
    }

    std::cout << "Database connection established in the thread." << std::endl;

    while (serverRunning) {
        TransactionPromise transactionPromisePair;
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            queueCondition.wait(lock, [] { return !transactionQueue.empty() || !serverRunning; });
            if (!serverRunning && transactionQueue.empty()) break;

            transactionPromisePair = std::move(transactionQueue.front());
            transactionQueue.pop();
        }

        Transaction transaction = transactionPromisePair.first;
        std::promise<Response> promise = std::move(transactionPromisePair.second);

        Response response = processTransaction(transaction, db);

        promise.set_value(response);

        emptyTransactionLogs(db);
    }

    sqlite3_close(db);
}
