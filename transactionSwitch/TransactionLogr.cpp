#include "TransactionLogr.h"
#include "Transaction.h"
#include <iostream>
#include <sstream>
#include <chrono>
#include <unordered_map>
#include <mutex>

std::mutex logMutex;


const std::unordered_map<Currency, std::string> enumToCurrencyCode = {
    {GBP, "GBP"},
    {USD, "USD"},
    {JPY, "JPY"},
    {EUR, "EUR"},
    {AUD, "AUD"},
    {CAD, "CAD"},
    {CHF, "CHF"},
    {CNH, "CNH"},
    {HKD, "HKD"},
    {NZD, "NZD"}
};

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

TransactionLogger::TransactionLogger() {
    logFile.open("transaction_log.txt", std::ios::app);
    if (!logFile.is_open()) {
        std::cerr << "Error opening log file!" << std::endl;
    }
}

TransactionLogger::~TransactionLogger() {
    if (logFile.is_open()) {
        logFile.close();
    }
}

void TransactionLogger::logTransaction(const Transaction& transaction) {
    std::lock_guard<std::mutex> lock(logMutex);

    std::string transactionDetails = transactionToString(transaction);

    if (logFile.is_open()) {
        logFile << transactionDetails << std::endl;
        std::cout << transactionDetails << std::endl;
    } else {
        std::cerr << "Failed to log transaction: " << transactionDetails << std::endl;
    }
}
