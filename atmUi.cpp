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

int screenWidth;
int screenHeight;

int main()
{
    Response r = forwardToSocket("123", "1", "0", "1", "1234", 5);
    std::cout << r.succeeded << std::endl;


    InitWindow(0, 0, "raygui - NCR ATM");
    GuiSetStyle(DEFAULT,TEXT_SIZE ,50);
    ToggleFullscreen();

    screenWidth  = GetScreenWidth();
    screenHeight = GetScreenHeight();

    SetTargetFPS(60);
    screenHeight = GetMonitorHeight(0); 
    screenWidth = GetMonitorWidth(0);

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
