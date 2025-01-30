#include "raylib.h"
#include <vector>
#include <functional>
#include <string>
#include <iostream>
#include "Components.hpp"
#include "Screens.hpp"

int screenWidth = 1920;
int screenHeight = 1080;
int atmX;
int atmY;
int atmWidth = 1200;
int atmHeight = 900;

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

    screen = insertCardScreen;

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(ATM_BACKGROUND);
        screen();
        EndDrawing();
    }
    


    return 0;
}
