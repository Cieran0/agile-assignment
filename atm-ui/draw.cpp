#include "atmUtil.h"
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cstring>
#include <random>
#include <unordered_map>

int screenWidth = GetMonitorWidth(0);  
int screenHeight = GetMonitorHeight(0);

int background = 0;

int atmWidth = 750;
int atmHeight = 900;
int atmX;
int atmY;

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include <functional>

void accessabilityView(){

}

void drawCashSlot(const char* text) {
    DrawRectangle(atmX + 20, atmY + 250, 200, 10, DARKGRAY);
    DrawText(text, atmX + 20, atmY + 230, 15, LIGHTGRAY);
}

void drawButtons(vector<Button> buttons) {
    for(const Button& button : buttons) {
        if(GuiButton(button.bounds, button.text)) {
            setScreen(button.nextScreen);
            break;
        }
    }
}

void drawMoney(std::string str) {
    if(str.empty()) str = "0";
    DrawText(("£" + str).c_str(), atmX + 20, atmY + 40, 30, ATM_TEXT);
}

void drawCardSlot() {
    int slotWidth = 200;
    int slotHeight = 10;
    int slotX = atmX + (atmWidth - slotWidth) / 2;
    int slotY = atmY + atmHeight - 80; 
    DrawRectangle(slotX, slotY, slotWidth, slotHeight, DARKGRAY);

    Rectangle visibilityBtn = { (float)slotX - 10, (float)(slotY - 300), (float)slotWidth + 20, 50 };
    Rectangle cardBtn = { (float)slotX, (float)(slotY - 40), (float)slotWidth, 30 };

    if (GuiButton(visibilityBtn, "Visibility Options")){
        setScreen(displayOptions);
        resetGlobalTextVariables();
        displayText = "Choose from the sizing optoins below:";
    }
    
    if (GuiButton(cardBtn, "INSERT CARD")) {
        setScreen(EnterPin);
        resetGlobalTextVariables();
        displayText = "Please enter your PIN:";
    }
}
    
time_t atmTime;
bool timeInitialized = false;

void drawPrintedReciept() {
    if (!timeInitialized) {
        time(&atmTime);
        timeInitialized = true;
    }

    std::string dateTime = ctime(&atmTime);
    std::string date = dateTime.substr(0, 10) + " " + dateTime.substr(20, 4); 
    std::string time = dateTime.substr(11, 8);
    std::string atmId = to_string(ATM_ID); 

    int recieptWidth = screenWidth / 4;
    int recieptHeight = screenHeight / 4;
    int x = (screenWidth - recieptWidth) / 2;
    int y = screenHeight - recieptHeight - (screenHeight / 10);

    std::string balanceString = "Account balance: " + std::to_string(a1.testBalance);

    Rectangle reciept = {x, y, recieptWidth, recieptHeight};

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


void setupGuiStyle() {
    GuiSetStyle(DEFAULT, BORDER_COLOR_NORMAL, ColorToInt(DARKGRAY));
    GuiSetStyle(DEFAULT, BASE_COLOR_NORMAL, ColorToInt(ATM_BUTTON_BG));
    GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, ColorToInt(WHITE));
    GuiSetStyle(DEFAULT, BASE_COLOR_FOCUSED, ColorToInt(ATM_BUTTON_HOVER));
    GuiSetStyle(DEFAULT, TEXT_COLOR_FOCUSED, ColorToInt(WHITE));
    GuiSetStyle(DEFAULT, BASE_COLOR_PRESSED, ColorToInt(DARKGRAY));
    GuiSetStyle(DEFAULT, TEXT_COLOR_PRESSED, ColorToInt(WHITE));
    GuiSetStyle(DEFAULT, BORDER_WIDTH, RAYGUI_BUTTON_BORDER_WIDTH);
    GuiSetStyle(DEFAULT, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
    GuiSetStyle(DEFAULT, TEXT_PADDING, RAYGUI_BUTTON_TEXT_PADDING);
    GuiSetStyle(DEFAULT, BORDER_WIDTH, 2);
    GuiSetStyle(BUTTON, BORDER_WIDTH, 2);
    GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL, ColorToInt(DARKGRAY));
    GuiSetStyle(BUTTON, BORDER_COLOR_FOCUSED, ColorToInt(LIGHTGRAY));
    GuiSetStyle(BUTTON, BORDER_COLOR_PRESSED, ColorToInt(WHITE));

    background = GuiGetStyle(DEFAULT, BACKGROUND_COLOR);
}

int getButtonColor(const std::string& text) {
    if (text == "cancel") return ColorToInt(RED);
    if (text == "clear") return ColorToInt(YELLOW);
    if (text == "enter") return ColorToInt(GREEN);
    return ColorToInt(GRAY);
}

void drawKeypad(const std::function<void(const string&)>& handleInput) {
    int keypadWidth = 300;
    int keypadHeight = 300;
    int startX = (atmX + atmWidth/2) - (keypadWidth/2);
    int startY = (atmY + atmHeight/2) - (keypadHeight/2);
    int buttonWidth = 80;
    int buttonHeight = 60;
    int spacing = 20;
    float x = startX;
    float y = startY;

    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            string keypadButtonText = keyPad[row][col];
            if (keypadButtonText == " ") { 
                x += (buttonWidth + spacing); 
                continue; 
            }
            Rectangle btnRect = { x, y, static_cast<float>(buttonWidth), static_cast<float>(buttonHeight) };
            GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, getButtonColor(keypadButtonText));
            if (GuiButton(btnRect, keyPad[row][col].c_str())) {
                handleInput(keyPad[row][col]);
            }
            x += (buttonWidth + spacing);
        }
        x = startX;
        y += (buttonHeight + spacing);
    }
}

void screenInit() {
    InitWindow(0, 0, "raygui - NCR ATM");
    setupGuiStyle();
    ToggleFullscreen();
    SetTargetFPS(60);
    screenWidth = GetMonitorWidth(0);  
    screenHeight = GetMonitorHeight(0);
    //screenWidth = 1920;  
    //screenHeight = 1080;
    atmX = (screenWidth - atmWidth)/2;
    atmY = (screenHeight - atmHeight) / 2;
    GuiSetStyle(DEFAULT, TEXT_SIZE, screenHeight/100);
}

void drawATMScreen(const char* text) {
    DrawRectangle(atmX, atmY, atmWidth, atmHeight, ATM_BACKGROUND);
    DrawRectangleLines(atmX, atmY, atmWidth, atmHeight, DARKGRAY);
    DrawRectangle(atmX, atmY, atmWidth, 200, DARKGRAY);
    DrawRectangle(atmX + 10, atmY + 10, atmWidth - 20, 180, ATM_DISPLAY_BG);
    DrawText(text, atmX + 20, atmY + 20, 20, ATM_TEXT);
}

void processingScreen(string messageToPrint) {
    static int counter = 0;
    if (counter < 60*5) {
        counter++;
        DrawText("Processing...", 470, 170, 20, ATM_TEXT);
    } else {
        DrawText("Printing successful. ", 200, 500, 40, BLACK);
        if (GuiButton({200, 600, 200, 50}, "Back")) {
            setScreen(MainMenu); 
        }
    }
}