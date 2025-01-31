#pragma once
#include <cstdint>
#include <string>
#include "Currency.hpp"


enum TransactionType {
    PIN_CHECK,
    BALANCE_CHECK,
    WITHDRAWAL,
    DEPOSIT,
    MOBILE_APROVED_DEPOSIT
};

enum ResponseType {
    SUCCESS = 0,
    INSUFFICIENT_FUNDS = 1,
    DATABASE_ERROR = 2,
    INCORRECT_PIN = 3,
    NETWORK_ERROR = 4,
    SYSTEM_MAINTENANCE = 5,
};

typedef int64_t AtmCurrency;
typedef uint64_t AtmID;
typedef uint64_t UniqueTranscationID;

struct Transaction {
    TransactionType type;
    UniqueTranscationID id;

    AtmID atmID;
    Currency currency;
    AtmCurrency amount;

    char cardNumber[20];
    char expiryDate[6];
    char pinNo[5];
};

struct Response {
    ResponseType succeeded;
    Currency atmCurrency;
    AtmCurrency newBalance;
    DecimalPosition dotPosition;
};


#define NETWORK (Response){.succeeded = ResponseType::NETWORK_ERROR}

extern const char* host;
extern int port;

Response forwardToSocket(Transaction transaction);
uint64_t rand_uint64();

extern AtmID atmID;