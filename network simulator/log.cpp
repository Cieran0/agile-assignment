#include "log.hpp"
#include <format>
#include <vector>
#include <iostream>
#include <functional>

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
    // Not implemented
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

int main(int argc, char const *argv[]) {
    
    addLogger(ConsoleLogger);

    // Example Transaction
    Transaction exampleTransaction = {
        .cardNumber = "11",
        .atmID = 0,
        .uniqueTransactionID = 10,
        .withdrawalAmount = 20.51,
    };

    log(exampleTransaction);

    return 0;
}
