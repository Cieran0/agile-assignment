#include "log.hpp"
#include <format>
#include <vector>
#include <iostream>
#include <functional>
#include <sqlite3.h>
#include <fstream>
#include <sstream>
#include <chrono>
#include "db.hpp"

std::vector<std::function<void(const Transaction&)>> loggers;
std::ofstream log_txt;

void addLogger(const std::function<void(const Transaction&)>& logger) {
    loggers.push_back(logger);
}

void log(const Transaction& transaction) {
    for (auto& log : loggers) {
        log(transaction);
    }
}

void DatabaseLogger(const Transaction& transaction) {

    std::string sql = std::format(
        "INSERT INTO Transactions (TransactionID, ATM_ID, WithdrawlAmount, CardNumber) "
        "VALUES ({}, {}, {}, '{}');",
        transaction.id,
        transaction.atmID,
        transaction.amount,
        transaction.cardNumber
    );

    enqueueTransactionLog(sql);
}

void txtLogger(const Transaction& transaction){

    std::string message;
    if(transaction.type == BALANCE_CHECK){ 
        message = std::format("[{}]: Transaction ID {} | Card number [{}] checked balance at ATM {} {}",
            std::chrono::system_clock::now(),
            transaction.id, 
            transaction.amount,
            transaction.cardNumber,
            transaction.atmID
        );
    }
    else if(transaction.type == DEPOSIT){   
        message = std::format("[{}]: Transaction ID {} | Deposit of £{} with card [{}] at ATM {}",
            std::chrono::system_clock::now(),
            transaction.id, 
            -transaction.amount,
            transaction.cardNumber,
            transaction.atmID
        );
    }
    else{
        message = std::format("[{}]: Transaction ID {} | withdrawal of £{} with card [{}] at ATM {}",
            std::chrono::system_clock::now(),
            transaction.id, 
            transaction.amount,
            transaction.cardNumber,
            transaction.atmID
        );
    }

    if(log_txt.is_open()) {
        log_txt << message << std::endl;
    }
}

void ConsoleLogger(const Transaction& transaction) {
    std::string console_message = std::format("[{}]: Transaction ID {} | withdrawal of £{} with card [{}] at ATM {}",
        std::chrono::system_clock::now(),
        transaction.id, 
        transaction.amount,
        transaction.cardNumber,
        transaction.atmID
    );
    std::cout << console_message << std::endl;
}
