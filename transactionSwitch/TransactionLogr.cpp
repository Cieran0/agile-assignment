#include "TransactionLogr.h"
#include "Transaction.h"
#include <iostream>
#include <sstream>
#include <chrono>
#include <unordered_map>
#include <mutex>
#include <thread>
#include <queue>
#include <condition_variable>

std::mutex logMutex;
std::queue<Transaction> transactionQueue;
std::condition_variable logCondition;
bool stopLogging = false;
std::thread loggingThreadInstance;


void loggingThread(TransactionLogger* logger) {
    while (true) {
        std::unique_lock<std::mutex> lock(logMutex);
        logCondition.wait(lock, [] { return !transactionQueue.empty() || stopLogging; });
        
        while (!transactionQueue.empty()) {
            Transaction transaction = transactionQueue.front();
            transactionQueue.pop();
            lock.unlock();
            logger->logTransaction(transaction);
            lock.lock();
        }
        
        if (stopLogging) break;
    }
}

TransactionLogger::TransactionLogger() {
    logFile.open("transaction_log.txt", std::ios::app);
    if (!logFile.is_open()) {
        std::cerr << "Error opening log file!" << std::endl;
    }
    loggingThreadInstance = std::thread(loggingThread, this);
}

TransactionLogger::~TransactionLogger() {
    {
        std::lock_guard<std::mutex> lock(logMutex);
        stopLogging = true;
    }
    logCondition.notify_one();
    if (loggingThreadInstance.joinable()) {
        loggingThreadInstance.join();
    }
    if (logFile.is_open()) {
        logFile.close();
    }
}

void TransactionLogger::logTransaction(const Transaction& transaction) {
    std::lock_guard<std::mutex> lock(logMutex);
    transactionQueue.push(transaction);
    logCondition.notify_one();
}