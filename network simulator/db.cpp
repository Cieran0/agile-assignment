#include "db.hpp"
#include <iostream>
#include <sstream>
#include <format>
#include "Conversion.hpp"
#include <cmath>

typedef const unsigned char* sqlite_string; 
typedef int64_t BankCurrency;

Response processTransaction(Transaction transaction, sqlite3*& db) {
    Response response = {.succeeded = NETWORK_ERROR};

    std::string sql_string = std::format("SELECT Balance, Currency FROM Customer WHERE CardNumber = '{}' AND PIN = '{}';", transaction.cardNumber, transaction.pinNo);

    sqlite3_stmt* stmt = nullptr;
    int exitCode = sqlite3_prepare_v2(db, sql_string.c_str(), -1, &stmt, nullptr);

    if (exitCode != SQLITE_OK) {
        std::cerr << "Database error (prepare): " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);

        response.succeeded = DATABASE_ERROR;
        return response;
    }

    if (sqlite3_step(stmt) != SQLITE_ROW) {
        std::cerr << "Database error (step): " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);

        response.succeeded = INCORRECT_PIN;
        return response;
    }

    BankCurrency currentBankBalance = sqlite3_column_int64(stmt, 0);
    sqlite_string bankCurrencyString = sqlite3_column_text(stmt, 1);

    Currency bankCurrency = currencyCodeToEnum.at((const char*)bankCurrencyString);

    //std::cout << "Current balance: " << currentBankBalance << std::endl;
    //std::cout << "Bank Currency: " << bankCurrencyString << " | " << bankCurrency << std::endl;
    
    DecimalPosition dotPosition = currencyDotPosition.at(transaction.currency);
    transaction.amount *= pow(10, dotPosition);

    BankCurrency transactionAmount = 0;
    if(ConvertCurrency(transaction.currency, bankCurrency, transaction.amount, transactionAmount, Rounding::UP)) {
        response.succeeded = DATABASE_ERROR;
        sqlite3_finalize(stmt);
        return response;
    } 


    AtmCurrency newBalance;
    if(ConvertCurrency(Currency::GBP, bankCurrency, transaction.amount, newBalance, Rounding::DOWN)) {
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

    if (transactionAmount > currentBankBalance) {
        response.succeeded = INSUFFICIENT_FUNDS;
        response.dotPosition = currencyDotPosition.at(transaction.currency);
        sqlite3_finalize(stmt);
        return response;
    }

    BankCurrency newBankBalance = currentBankBalance - transactionAmount;
    std::string update_sql = std::format("UPDATE Customer SET Balance = {} WHERE CardNumber = '{}';", newBankBalance, transaction.cardNumber);

    exitCode = sqlite3_exec(db, update_sql.c_str(), nullptr, nullptr, nullptr);
    if (exitCode != SQLITE_OK) {
        std::cerr << "Database error (exec): " << sqlite3_errmsg(db) << std::endl;
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

int initDatabaseConnection(sqlite3*& db) {
    return sqlite3_open("database.db", &db);
}
