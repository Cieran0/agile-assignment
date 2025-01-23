#include "atmUtil.h"
#include <iostream>
#include <string>

#include <sstream>

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include <vector>

using namespace std;

    string cardNumber;      
    string expiryDate;      
    string atmID;          
    string transactionID;  
    string pin; 
    double withdrawalAmount;     

int main()
{
    InitWindow(screenWidth, screenHeight, "raygui - NCR ATM");
    GuiSetStyle(DEFAULT, TEXT_SIZE, 20);
    GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, ColorToInt(BLACK));

    ToggleFullscreen();

    screenWidth  = GetScreenWidth();
    screenHeight = GetScreenHeight();

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
        screenManager();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
