#include "raylib.h"
#include <vector>
#include <functional>
#include <string>
#include <iostream>
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


int main(int argc, char const *argv[])
{

    atmX = (screenWidth - atmWidth) / 6;
    atmY = (screenHeight - atmHeight) / 2;

    InitWindow(screenWidth, screenHeight, "test");
    SetTargetFPS(60);

    setCurrency(Currency::CNH);

    screen = selectLanguageScreen;

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(ATM_BACKGROUND);
        screen();
        EndDrawing();
    }
    


    return 0;
}
