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

const std::unordered_map<TranscationType, std::string> enumToTransactionTypeString = {
    {WITHDRAWL, "Withdrawal"},
    {PIN_CHECK, "PIN Check"},
    {BALANCE_CHECK, "Balance Check"},
    {DEPOSIT, "Deposit"},
    {MOBILE_APROVED_DEPOSIT, "Mobile Deposit"},
};

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
        "INSERT INTO Transactions (TransactionID, ATM_ID, CardNumber, Type, Amount, Time, Currency) "
        "VALUES ({}, {}, '{}', '{}', {}, '{}', '{}');",
        transaction.id,
        transaction.atmID,
        transaction.cardNumber,
        enumToTransactionTypeString.at(transaction.type),
        transaction.amount,
        std::chrono::system_clock::now(),
        enumToCurrencyCode.at(transaction.currency)
    );

    enqueueTransactionLog(sql);
}


std::string transactionToString(const Transaction& transaction) {
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
        message = std::format("[{}]: Transaction ID {} | Deposit of {} {} with card [{}] at ATM {}",
            std::chrono::system_clock::now(),
            transaction.id, 
            transaction.amount,
            enumToCurrencyCode.at(transaction.currency),
            transaction.cardNumber,
            transaction.atmID
        );
    } else if (transaction.type == WITHDRAWL) {
        message = std::format("[{}]: Transaction ID {} | withdrawal of {} {} with card [{}] at ATM {}",
            std::chrono::system_clock::now(),
            transaction.id, 
            transaction.amount,
            enumToCurrencyCode.at(transaction.currency),
            transaction.cardNumber,
            transaction.atmID
        );

    } else if (transaction.type == TranscationType::PIN_CHECK) {
        message = std::format("[{}]: Transaction ID {} | pin check with card [{}] at ATM {}",
            std::chrono::system_clock::now(),
            transaction.id, 
            transaction.cardNumber,
            transaction.atmID
        );
    }
    else{
        message = std::format("[{}]: Transaction ID {} | transaction of {} {} with card [{}] at ATM {}",
            std::chrono::system_clock::now(),
            transaction.id, 
            transaction.amount,
            enumToCurrencyCode.at(transaction.currency),
            transaction.cardNumber,
            transaction.atmID
        );
    }
    return message;
} 

void txtLogger(const Transaction& transaction){
    if(log_txt.is_open()) {
        log_txt << transactionToString(transaction) << std::endl;
    }
}

void ConsoleLogger(const Transaction& transaction) {
    std::cout << transactionToString(transaction) << std::endl;
}
