#include "log.hpp"
#include <format>
#include <vector>
#include <iostream>
#include <functional>
#include <sqlite3.h>
#include <fstream>

std::vector<std::function<void(const Transaction&)>> loggers;

void addLogger(const std::function<void(const Transaction&)>& logger) {
    loggers.push_back(logger);
}

void log(const Transaction& transaction) {
    for (auto& log : loggers) {
        log(transaction);
    }
}

void DatabaseLogger(const Transaction& transaction) {
    sqlite3* db;
    int exitCode = sqlite3_open("database.db", &db);
    if (exitCode != SQLITE_OK) {
        std::cerr << "Failed to open database: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return;
    }

    std::string sql = std::format(
        "INSERT INTO Transactions (TransactionID, ATM_ID, WithdrawlAmount, CardNumber) "
        "VALUES ({}, {}, {}, '{}');",
        transaction.uniqueTransactionID,
        transaction.atmID,
        transaction.withdrawalAmount,
        transaction.cardNumber
    );

    char* errorMessage = nullptr;
    exitCode = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errorMessage);
    if (exitCode != SQLITE_OK) {
        std::cerr << "SQL error: " << errorMessage << std::endl;
        sqlite3_free(errorMessage);
    }

    sqlite3_close(db);
}

void txtLogger(const Transaction& transaction){
    std::ofstream log_file;

    log_file.open("sim_log.txt", std::ios_base::app);

    log_file << std::format("{}: withdrawal of £{} with card number {} at ATM {}\n",
        transaction.uniqueTransactionID,
        transaction.withdrawalAmount,
        transaction.cardNumber,
        transaction.atmID
    );
}

void ConsoleLogger(const Transaction& transaction) {
    std::string console_message = std::format("{}: withdrawal of £{} with card number {} at ATM {}",
        transaction.uniqueTransactionID, 
        transaction.withdrawalAmount,
        transaction.cardNumber,
        transaction.atmID
    );
    std::cout << console_message << std::endl;
}
