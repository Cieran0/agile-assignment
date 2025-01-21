#pragma once
#include <iostream>
#include <string>
#include <functional>
#include <cstdint>

struct Transaction {
    char cardNumber[20];
    char expiryDate[6];
    uint64_t atmID;
    uint64_t uniqueTransactionID;
    char pinNo[5];
    double withdrawalAmount;
};

void addLogger(const std::function<void(const Transaction&)>& logger);

void log(const Transaction& transaction);

void DatabaseLogger(const Transaction& transaction);

void ConsoleLogger(const Transaction& transaction);