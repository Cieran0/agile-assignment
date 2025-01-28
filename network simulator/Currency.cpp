#include "Currency.hpp"
#include <sqlite3.h>
#include <iostream>

std::unordered_map<Currency, DecimalPosition> currencyDotPosition;

const std::unordered_map<std::string, Currency> currencyCodeToEnum = {
    {"GBP", GBP},
    {"USD", USD},
    {"JPY", JPY},
    {"EUR", EUR},
    {"AUD", AUD},
    {"CAD", CAD},
    {"CHF", CHF},
    {"CNH", CNH},
    {"HKD", HKD},
    {"NZD", NZD}
};

int initCurrencyPositions(sqlite3*& db) {
    const char* query = "SELECT CurrencyCode, DotPosition FROM Currencies;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return -1;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char* currencyCode = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        DecimalPosition dotPosition = static_cast<DecimalPosition>(sqlite3_column_int(stmt, 1));

        auto it = currencyCodeToEnum.find(currencyCode);
        if (it == currencyCodeToEnum.end()) {
            std::cerr << "Unknown currency code: " << currencyCode << std::endl;
            continue;
        }

        currencyDotPosition[it->second] = dotPosition;
    }

    sqlite3_finalize(stmt);

    return 0;
}
