#include "Conversion.hpp"
#include <sqlite3.h>
#include <iostream>

std::unordered_map<Currency, ConversionRates> currencyConversionRates;

int initConversionRates(sqlite3* db) {
    const char* query = "SELECT CurrencyFrom, CurrencyTo, ConversionRate FROM ConversionRate;";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return -1;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char* currencyFrom = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        const char* currencyTo = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        double conversionRate = sqlite3_column_double(stmt, 2);

        auto itFrom = currencyCodeToEnum.find(currencyFrom);
        auto itTo = currencyCodeToEnum.find(currencyTo);

        if (itFrom == currencyCodeToEnum.end() || itTo == currencyCodeToEnum.end()) {
            std::cerr << "Unknown currency code in conversion rate: " 
                      << (itFrom == currencyCodeToEnum.end() ? currencyFrom : currencyTo) 
                      << std::endl;
            continue;
        }

        currencyConversionRates[itFrom->second][itTo->second] = conversionRate;
    }

    sqlite3_finalize(stmt);

    return 0;
}