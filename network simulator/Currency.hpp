#pragma once
#include <cstdint>
#include <unordered_map>
#include <sqlite3.h>
#include <string>

typedef uint64_t DecimalPosition;

enum Currency {
    GBP,
    USD,
    JPY,
    EUR,
    AUD,
    CAD,
    CHF,
    CNH,
    HKD,
    NZD
};


extern std::unordered_map<Currency, DecimalPosition> currencyDotPosition;

extern const std::unordered_map<std::string, Currency> currencyCodeToEnum;
extern const std::unordered_map<Currency, std::string> enumToCurrencyCode;

enum Rounding {
    UP,
    DOWN
};

int initCurrencyPositions(sqlite3*& db);
int ConvertCurrency(const Currency& from, const Currency& to, const int64_t& amountBefore, int64_t& amountAfter, const Rounding& rounding);