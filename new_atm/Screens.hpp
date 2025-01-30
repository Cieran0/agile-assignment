#pragma once
#include <functional>
#include <string>
#include "Components.hpp"

extern std::function<void()> screen;
extern std::string enteredPin;

void enterPinScreen();
void insertCardScreen();

void drawScreen(std::string primaryText, std::string secondaryText, std::vector<ScreenButton> screenButtons, std::function<void(std::string)> keypadHandler, std::function<void()> insertCardHandler);