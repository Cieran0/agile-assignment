#pragma once
#include <unordered_map>
#include "Currency.hpp"

typedef double ConversionRate;
typedef std::unordered_map<Currency, ConversionRate> ConversionRates;

extern std::unordered_map<Currency, DecimalPosition> currencyDotPosition;
extern std::unordered_map<Currency, ConversionRates> currencyConversionRates;