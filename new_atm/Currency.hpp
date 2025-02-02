#pragma once
#include <cstdint>
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

extern Currency currentCurrency;
extern std::string currencySymbol;

void setCurrency(Currency currency);