// TransactionLogr.cpp
#include "TransactionLogr.h"
#include "Transaction.h"
#include <iostream>
#include <format>
#include <chrono>

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
   std::string transactionDetails = std::format("[{}]: Transaction ID {} | withdrawal of £{} with card [{}] at ATM {}\n",
        std::chrono::system_clock::now(),
        transaction.uniquetransactionID, 
        transaction.withdrawalAmount,
        transaction.cardNumber,
        transaction.atmID
    );
   
    if (logFile.is_open()) {
        logFile << transactionDetails;
        std::cout << transactionDetails;
    }
    else {
        std::cerr << "Failed to log transaction: " << transactionDetails << std::endl;
    }
}
