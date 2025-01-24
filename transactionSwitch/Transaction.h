#pragma once

#include <cstdint>

#define TRANSACTION_SUCESS 0
#define INSUFFICIENT_FUNDS 1
#define DATABASE_ERROR 2
#define INCORRECT_PIN 3
#define NETWORK_ERROR 4

struct Transaction {
    char cardNumber[20];
    char expiryDate[6];
    uint64_t atmID;
    uint64_t uniquetransactionID;
    char pinNo[5];
    double withdrawalAmount;
};

struct Response {
    int succeeded;
    double new_balance;
};