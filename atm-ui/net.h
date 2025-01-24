#pragma once

#include <cstdint>
#include <string>

#define TRANSACTION_SUCESS 0
#define INSUFFICIENT_FUNDS 1
#define DATABASE_ERROR 2
#define INCORRECT_PIN 3
#define NETWORK_ERROR 4

struct Transaction {
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
    Response() {}
    Response(int code) : succeeded(code), new_balance(0) {}
};

extern const char* host;
extern int port;

Response forwardToSocket(std::string cardNumber, std::string expiryDate, uint64_t atmID, std::string pin, double withdrawalAmount);