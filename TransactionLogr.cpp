// TransactionLogr.cpp
#include "TransactionLogr.h"
#include <iostream>

TransactionLogger::TransactionLogger() {
    // open log file in append mode
    logFile.open("transaction_log.txt", std::ios::app);
    if (!logFile.is_open()) {
        std::cerr << "Error opening log file!" << std::endl;
    }
}

TransactionLogger::~TransactionLogger() { // close file when log isnt logging ygm?
    if (logFile.is_open()) {
        logFile.close();
    }
}

void TransactionLogger::logTransaction(const std::string& transactionDetails) { // output message so it seems like something happens
    if (logFile.is_open()) {
        logFile << transactionDetails << std::endl;
        std::cout << "Logged transaction: " << transactionDetails << std::endl;
    }
    else {
        std::cerr << "Failed to log transaction: " << transactionDetails << std::endl;
    }
}
