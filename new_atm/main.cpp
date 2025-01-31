#include "raylib.h"
#include <vector>
#include <functional>
#include <string>
#include <iostream>
#include <sstream>
#include "Components.hpp"
#include "Screens.hpp"
#include "Currency.hpp"
#include "Transaction.hpp"

int screenWidth = 1920;
int screenHeight = 1200;
int atmX;
int atmY;
int atmWidth = 1200;
int atmHeight = 900;

int port = 6667;
const char* host = "127.0.0.1";

std::function<void()> screen;

std::string enteredPin;

std::string keyPad[5][3] = {
    {"1", "2", "3"},
    {"4", "5", "6"},
    {"7", "8", "9"},
    {" ", "0", " "},
    {"cancel", "clear", "enter"}
};

Currency selectedCurrency = Currency::GBP;

void parseArguments(int argc, char const *argv[]) {
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--host" && i + 1 < argc) {
            host = argv[++i];
        } else if (arg == "--port" && i + 1 < argc) {
            port = std::stoi(argv[++i]);
        } else if (arg == "--currency" && i + 1 < argc) {
            std::string currencyStr = argv[++i];
            if (currencyStr == "GBP") selectedCurrency = Currency::GBP;
            else if (currencyStr == "USD") selectedCurrency = Currency::USD;
            else if (currencyStr == "JPY") selectedCurrency = Currency::JPY;
            else if (currencyStr == "EUR") selectedCurrency = Currency::EUR;
            else if (currencyStr == "AUD") selectedCurrency = Currency::AUD;
            else if (currencyStr == "CAD") selectedCurrency = Currency::CAD;
            else if (currencyStr == "CHF") selectedCurrency = Currency::CHF;
            else if (currencyStr == "CNH") selectedCurrency = Currency::CNH;
            else if (currencyStr == "HKD") selectedCurrency = Currency::HKD;
            else if (currencyStr == "NZD") selectedCurrency = Currency::NZD;
        }
    }
}

int main(int argc, char const *argv[]) {
    parseArguments(argc, argv);

    atmX = (screenWidth - atmWidth) / 6;
    atmY = (screenHeight - atmHeight) / 2;

    InitWindow(screenWidth, screenHeight, "ATM");

    mainFont = LoadFontEx("Ubuntu-Bold.ttf", 80, 0, 250);

    ToggleFullscreen();

    atmID = randomID();

    SetTargetFPS(60);

    setCurrency(selectedCurrency);

    screen = selectLanguageScreen;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(ATM_BACKGROUND);
        screen();
        EndDrawing();
    }

    return 0;
}
