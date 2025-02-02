#pragma once
#include <functional>
#include <string>
#include "Components.hpp"
#include <cstdint>

extern std::function<void()> screen;
extern std::string enteredPin;

uint64_t randomID();

void enterPinScreen();
void insertCardScreen();
void selectLanguageScreen();
void balanceInquiryScreen();
void balanceScreen();
void printBalanceScreen();
void withdrawalScreen();
void depositScreen();
void processingTransaction();

void drawScreen(std::string primaryText, std::string secondaryText, std::vector<ScreenButton> screenButtons, std::function<void(std::string)> keypadHandler, std::function<void()> insertCardHandler);