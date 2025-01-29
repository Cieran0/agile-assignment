#include "TransactionLogr.h"
#include "Transaction.h"
#include <iostream>
#include <sstream>
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
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream transactionDetails;
    transactionDetails << "[" << std::ctime(&time) << "]: "
                       << "Transaction ID " << transaction.uniquetransactionID
                       << " | withdrawal of £" << transaction.withdrawalAmount
                       << " with card [" << transaction.cardNumber
                       << "] at ATM " << transaction.atmID << "\n";

    if (logFile.is_open()) {
        logFile << transactionDetails.str();
        std::cout << transactionDetails.str();
    } else {
        std::cerr << "Failed to log transaction: " << transactionDetails.str() << std::endl;
    }
}
