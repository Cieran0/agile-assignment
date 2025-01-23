#include "db.hpp"
#include <iostream>
#include <sstream>
#include <format>

typedef const unsigned char* sqlite_string; 

Response processTransaction(Transaction transaction, sqlite3*& db) {
    Response response = {0};

    std::string sql_string = std::format("SELECT Balance FROM Customer WHERE CardNumber = '{}' AND PIN = '{}';", transaction.cardNumber, transaction.pinNo);

    sqlite3_stmt* stmt = nullptr;
    int exitCode = sqlite3_prepare_v2(db, sql_string.c_str(), -1, &stmt, nullptr);

    if (exitCode != SQLITE_OK) {
        std::cerr << "Database error (prepare): " << sqlite3_errmsg(db) << std::endl; // Print error message
        sqlite3_close(db);

        response.succeeded = DATABASE_ERROR;
        return response;
    }

    if (sqlite3_step(stmt) != SQLITE_ROW) {
        std::cerr << "Database error (step): " << sqlite3_errmsg(db) << std::endl; // Print error message
        sqlite3_finalize(stmt);

        response.succeeded = INCORRECT_PIN;
        return response;
    }

    double current_balance = sqlite3_column_double(stmt, 0);

    std::cout << "Current balance: " << current_balance << std::endl;
    
    if (transaction.withdrawalAmount == 0) {
        response.new_balance = current_balance;
        response.succeeded = TRANSACTION_SUCESS;
        sqlite3_finalize(stmt);
        return response;
    }

    if (transaction.withdrawalAmount > current_balance) {
        response.succeeded = INSUFFICIENT_FUNDS;
        sqlite3_finalize(stmt);
        return response;
    }

    double new_balance = current_balance - transaction.withdrawalAmount;
    std::string update_sql = std::format("UPDATE Customer SET Balance = {} WHERE CardNumber = '{}';", new_balance, transaction.cardNumber);

    exitCode = sqlite3_exec(db, update_sql.c_str(), nullptr, nullptr, nullptr);
    if (exitCode != SQLITE_OK) {
        std::cerr << "Database error (exec): " << sqlite3_errmsg(db) << std::endl; // Print error message
        response.succeeded = DATABASE_ERROR;
        sqlite3_finalize(stmt);
        return response;
    }
    
    response.new_balance = new_balance;

    sqlite3_finalize(stmt);
    return response;
}

int initDatabaseConnection(sqlite3*& db) {
    return sqlite3_open("database.db", &db);
}
