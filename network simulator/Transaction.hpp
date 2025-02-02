#pragma once
#include <cstdint>
#include "Currency.hpp"

enum TranscationType {
    PIN_CHECK,
    BALANCE_CHECK,
    WITHDRAWL,
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
    CARD_BLOCKED = 6,
};

typedef int64_t AtmCurrency;
typedef uint64_t AtmID;
typedef uint64_t UniqueTranscationID;

struct Transaction {
    TranscationType type;
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

