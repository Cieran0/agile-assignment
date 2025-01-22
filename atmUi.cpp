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
    int screenWidth = GetMonitorWidth(0);  
    int screenHeight = GetMonitorHeight(0); 
    InitWindow(screenWidth, screenHeight, "raygui - NCR ATM");
    ToggleFullscreen();
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

