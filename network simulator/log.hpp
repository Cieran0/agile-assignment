#pragma once 
#include <functional>
#include <cstdint>
#include "db.hpp"
#include <string>

void addLogger(const std::function<void(const Transaction&)>& logger);

void log(const Transaction& transaction);

void DatabaseLogger(const Transaction& transaction);

void txtLogger(const Transaction& transaction);

void ConsoleLogger(const Transaction& transaction);

std::string getTextOut();
