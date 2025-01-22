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
        sqlite3_close(db);

        response.succeeded = DATABASE_ERROR;
        return response;
    }

    if (sqlite3_step(stmt) != SQLITE_ROW) {
        sqlite3_finalize(stmt);

        response.succeeded = INCORRECT_PIN;
        return response;
    }

    double current_balance = sqlite3_column_double(stmt, 0);
    
    if(transaction.withdrawalAmount == 0) {
        response.new_balance = current_balance;
        response.succeeded = true;
        return response;
    }

    if (transaction.withdrawalAmount > current_balance) {
        response.succeeded = INSUFFICIENT_FUNDS;
        return response;
    }

    double new_balance = current_balance - transaction.withdrawalAmount;
    std::string update_sql = std::format("UPDATE Customer SET Balance = {} WHERE CardNumber = '{}';", new_balance, transaction.cardNumber);

    exitCode = sqlite3_exec(db, update_sql.c_str(), nullptr, nullptr, nullptr);
    if (exitCode != SQLITE_OK) {
        response.succeeded = DATABASE_ERROR;
        return response;
    }
    
    response.new_balance = new_balance;

    sqlite3_finalize(stmt);
    return response;
}

int initDatabaseConnection(sqlite3*& db) {
    return sqlite3_open("database.db", &db);
}
