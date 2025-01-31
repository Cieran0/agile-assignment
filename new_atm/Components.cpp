#include "raylib.h"
#include <vector>
#include <functional>
#include <string>
#include <iostream>
#include "Components.hpp"
#include "LocalString.hpp"
#include <chrono>
#include <iomanip>
#include <sstream>
#include "Currency.hpp"

bool inBounds(Rectangle r, int x, int y) {
    return !(x < r.x || x > r.x + r.width || y < r.y || y > r.y + r.height);
}


bool GuiButton(Rectangle r, std::string text, Color color, Color highlight) {

    if(text.empty()){
        return false;
    }

    int mouseX = GetMouseX();
    int mouseY = GetMouseY();

    bool hovering = inBounds(r, mouseX, mouseY);

    DrawRectangleRec(r, hovering? highlight : color);
    DrawRectangleLinesEx(r, 1, BLACK);
    int fontSize = r.height;
    int textSize = MeasureText(text.c_str(), fontSize);
    int yOffset = 0;

    if(text.size() != 1) {
        //int len = text.length();
        fontSize = r.height / 2;
        textSize = MeasureText(text.c_str(), fontSize);
        yOffset=r.height/4;

        if(textSize > r.width) {
            fontSize /= (((double)textSize * 1.05) / (double)r.width);
            textSize = MeasureText(text.c_str(), fontSize);
            yOffset = (r.height - fontSize) / 2;
        }

    }

    DrawText(text.c_str(), r.x + (r.width - textSize)/2, r.y + yOffset, fontSize, BLACK);

    return IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && hovering;
}

void drawKeypad(const std::function<void(const std::string&)>& handleInput) {
    int buttonWidth = 120;
    int buttonHeight = 60;
    int spacing = 20;

    //int keypadWidth = 3 * buttonWidth + 2 * spacing;
    int keypadHeight = 5 * buttonHeight + 4 * spacing;
    
    int startX = atmX + atmWidth + (atmWidth / 7);
    int startY = atmY + (keypadHeight - 100);    

    float x = startX;
    float y = startY;
    Color mainColor = LIGHTGRAY;

    for (int row = 0; row < 5; row++) { 
        for (int col = 0; col < 3; col++) {
            Color color = mainColor;
            std::string keypadButtonText = keyPad[row][col];
            if (keypadButtonText == " ") { 
                x += (buttonWidth + spacing); 
                continue; 
            }

            if(keypadButtonText == "cancel") {
                color = RED;
                keypadButtonText = inCurrentLanguage(LocalString::CANCEL);
            } else if (keypadButtonText == "enter") {
                color = GREEN;
                keypadButtonText = inCurrentLanguage(LocalString::ENTER);
            } else if (keypadButtonText == "clear") {
                color = YELLOW;
                keypadButtonText = inCurrentLanguage(LocalString::CLEAR);
            }


            Rectangle btnRect = { x, y, static_cast<float>(buttonWidth), static_cast<float>(buttonHeight) };
            if (GuiButton(btnRect, keypadButtonText, color, WHITE)) {
                handleInput(keypadButtonText);
            }

            x += (buttonWidth + spacing);
        }
        x = startX;
        y += (buttonHeight + spacing);
    }

}

void drawKeypadAndCardBackground() {
    int backgroundWidth = 435;
    int backgroundHeight = 750;
    int backgroundX = atmX + atmWidth + (atmWidth / 8);
    int backgroundY = atmY + 50;

    float rounding = 0.1f; 
    //int borderThickness = 4;

    DrawRectangleRounded(
        (Rectangle){(float)backgroundX, (float)backgroundY, (float)backgroundWidth, (float)backgroundHeight},
        rounding,
        10, 
        DARKGRAY
    );

    DrawRectangleRoundedLines(
        (Rectangle){(float)backgroundX, (float)backgroundY, (float)backgroundWidth, (float)backgroundHeight},
        rounding,
        10, 
        BLACK
    );
}

void drawATMScreen(std::string primaryText, std::string secondaryText) {
    int PRIMARY_TEXT_SIZE = 80;
    int SECONDARY_TEXT_SIZE = 150;

    DrawRectangle(atmX, atmY, atmWidth, atmHeight, ATM_BACKGROUND);
    DrawRectangleRoundedLines(
        (Rectangle){(float)atmX, (float)atmY, (float)atmWidth, (float)atmHeight},
        0.05f,
        10,   
        DARKGRAY
    );
    DrawRectangleRounded(
        (Rectangle){(float)atmX, (float)atmY, (float)atmWidth, (float)(atmHeight - 100)},
        0.05f, 
        10,   
        DARKGRAY
    ); 
    DrawRectangle(atmX + 10, atmY + 10, atmWidth - 20, atmHeight - 20, ATM_DISPLAY_BG);

    int textWidth = MeasureText(primaryText.c_str(), PRIMARY_TEXT_SIZE);

    if(textWidth > atmWidth) {
        PRIMARY_TEXT_SIZE /= (((double)textWidth * 1.05) / (double)atmWidth); 
        textWidth = MeasureText(primaryText.c_str(), PRIMARY_TEXT_SIZE);
    }

    int textX = atmX + (atmWidth - textWidth) / 2; 
    int textY = atmY + 40; 
    DrawText(primaryText.c_str(), textX, textY, PRIMARY_TEXT_SIZE, ATM_TEXT);

    if (!secondaryText.empty()) {
        int textWidth = MeasureText(secondaryText.c_str(), SECONDARY_TEXT_SIZE);
        if(textWidth > atmWidth) {
            SECONDARY_TEXT_SIZE /= (((double)textWidth * 1.05) / (double)atmWidth); 
            textWidth = MeasureText(secondaryText.c_str(), SECONDARY_TEXT_SIZE);
        }
        int textX = atmX + (atmWidth - textWidth) / 2;
        int textY = atmY + (atmHeight / 3); 
        DrawText(secondaryText.c_str(), textX, textY, SECONDARY_TEXT_SIZE, ATM_TEXT); 
    }
}

void drawSideButtons(const std::vector<ScreenButton>& screenButtons) {
    int buttonWidth = 90;
    int buttonHeight = 70;
    int buttonSpacing = 100;

    int startXLeft = atmX - buttonWidth - 15;
    int startXRight = atmX + atmWidth + 20;

    int totalButtonHeight = (4 * buttonHeight) + (3 * buttonSpacing);
    int startY = atmY + ((atmHeight - totalButtonHeight) / 2);

    for (size_t i = 0; i < 4; i++) {
        Rectangle btnRect = {
            static_cast<float>(startXLeft),
            static_cast<float>(startY + (i * (buttonHeight + buttonSpacing))),
            static_cast<float>(buttonWidth),
            static_cast<float>(buttonHeight)
        };

        if (GuiButton(btnRect, ">", LIGHTGRAY, WHITE) && screenButtons.size() > i) {
            screenButtons[i].func();
        }
    }

    for (size_t i = 0; i < 4; i++) {
        Rectangle btnRect = {
            static_cast<float>(startXRight),
            static_cast<float>(startY + (i * (buttonHeight + buttonSpacing))),
            static_cast<float>(buttonWidth),
            static_cast<float>(buttonHeight)
        };

        if (GuiButton(btnRect, "<", LIGHTGRAY, WHITE) && screenButtons.size() > i+4){
            screenButtons[i+4].func();
        }
    }
}

void drawCardSlot(const char* text, std::function<void()> insertCardHandler) {
    int slotWidth = 400;
    int slotHeight = 60;
    int startX = atmX + atmWidth + (atmWidth / 7);
    int slotY = atmY + 75;

    Rectangle cardSlotButton = {
        static_cast<float>(startX),
        static_cast<float>(slotY),
        static_cast<float>(slotWidth),
        static_cast<float>(slotHeight)
    };

    if (GuiButton(cardSlotButton, text, LIGHTGRAY, WHITE)) {
        insertCardHandler();
    }

    Rectangle cashSlotButton = {
        static_cast<float>(startX), static_cast<float>(atmY + 720), static_cast<float>(slotWidth), static_cast<float>(slotHeight)
    };

    GuiButton(cashSlotButton, inCurrentLanguage(LocalString::CASH), LIGHTGRAY, LIGHTGRAY);
}


std::string getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::tm localTime = *std::localtime(&now_c);

    std::ostringstream oss;
    oss << std::put_time(&localTime, "%H:%M:%S");
    return oss.str();
}


std::string getCurrentDate() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::tm localTime = *std::localtime(&now_c);

    std::ostringstream oss;
    oss << std::put_time(&localTime, "%Y-%m-%d");
    return oss.str();
}

bool shouldResetTime = false;

void resetReciept() {
    shouldResetTime = true;
}

void drawPrintedReciept() {

    static std::string date = getCurrentDate(); 
    static std::string time = getCurrentTime();

    if(shouldResetTime) {
        date = getCurrentDate(); 
        time = getCurrentTime();
        shouldResetTime = false;
    }

    std::string atmId = "201"; 

    int recieptWidth = screenWidth / 4;
    int recieptHeight = screenHeight / 4;
    int x = (screenWidth - recieptWidth) / 2;
    int y = screenHeight - recieptHeight - (screenHeight / 10);

    std::string balanceString = inCurrentLanguage(YOUR_BALANCE) + balanceAsString();

    Rectangle reciept = {static_cast<float>(x), static_cast<float>(y), static_cast<float>(recieptWidth), static_cast<float>(recieptHeight)};

    DrawRectangle(reciept.x, reciept.y, reciept.width, reciept.height, WHITE);
    DrawRectangleLines(reciept.x, reciept.y, reciept.width, reciept.height, BLACK);

    int padding = recieptWidth / 20;
    int textX = reciept.x + padding;
    int textY = reciept.y + padding;
    int lineHeight = recieptHeight / 10;

    DrawText(("Date: " + date).c_str(), textX, textY, lineHeight, BLACK);
    textY += lineHeight + padding;
    DrawText(("Time: " + time).c_str(), textX, textY, lineHeight, BLACK);
    textY += lineHeight + padding;
    DrawText(("ATM ID: " +  atmId).c_str(), textX, textY, lineHeight, BLACK);
    textY += lineHeight + padding;

    DrawText(balanceString.c_str(), textX, textY, lineHeight, BLACK);
}