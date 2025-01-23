#pragma once

#include <cstdint>
#include <string>

struct Transaction{
    char cardNumber[20];
    char expiryDate[6];
    uint64_t atmID;
    uint64_t uniqueTransactionID;
    char pinNo[5];
    double withdrawalAmount;
};

struct Response {
    int succeeded;
    double new_balance;
};



Response forwardToSocket(std::string cardNumber, std::string expiryDate, std::string transactionID, std::string atmID, std::string pin, double withdrawalAmount);