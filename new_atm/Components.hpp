#pragma once
#include <string>
#include <functional>
#include "raylib.h"

extern int screenWidth;
extern int screenHeight;
extern int atmX;
extern int atmY;
extern int atmWidth;
extern int atmHeight;

struct ScreenButton {
    std::string text;
    std::function<void()> func;
};

#define ATM_BACKGROUND CLITERAL(Color){ 45, 49, 66, 255 }     // Dark blue-gray
#define ATM_DISPLAY_BG CLITERAL(Color){ 200, 247, 197, 255 }  // Light green
#define ATM_BUTTON_BG CLITERAL(Color){ 77, 83, 110, 255 }     // Medium blue-gray
#define ATM_BUTTON_HOVER CLITERAL(Color){ 137, 142, 163, 255 }
#define ATM_TEXT CLITERAL(Color){ 33, 37, 41, 255 }           // Dark gray

#define CASING_BACK_COLOR CLITERAL(Color){(84, 103, 125, 1)}
#define CASING_FRONT_COLOR CLITERAL(Color){(59, 70, 84, 1)}


extern std::string keyPad[5][3];

bool inBounds(Rectangle r, int x, int y);
bool GuiButton(Rectangle r, std::string text, Color color, Color highlight);
void drawKeypad(const std::function<void(const std::string&)>& handleInput);
void drawKeypadAndCardBackground();
void drawATMScreen(std::string primaryText, std::string secondaryText);
void drawSideButtons();
void drawCardSlot(const char* text, std::function<void()> insertCardHandler);