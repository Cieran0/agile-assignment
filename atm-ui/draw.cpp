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
char currencySymbol = ' ';
Currency currentCurrency = GBP;

#define CASING_BACK_COLOR CLITERAL(Color){(84, 103, 125, 1)}
#define CASING_FRONT_COLOR CLITERAL(Color){(59, 70, 84, 1)}

int background = 0;

int atmWidth = 1200;
int atmHeight = 900;
int atmX;
int atmY;

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include <functional>


void drawATM(const char* text){
    drawATMScreen(text);
    drawCashSlot("INSERTED CARD");
    drawSideButtons();
    drawKeypadAndCardBackground();
    drawKeypad(handleInput);
    drawCashSlot("INSERT CARD HERE");
}

void accessabilityView(){
}

void drawCashSlot(const char* text) {
    int slotWidth = 400;
    int slotHeight = 40;
    int startX = atmX + atmWidth + (atmWidth / 7);
    int slotY = atmY + 150;

    Rectangle cashSlotButton = {
        static_cast<float>(startX),
        static_cast<float>(slotY),
        static_cast<float>(slotWidth),
        static_cast<float>(slotHeight)
    };

    if (GuiButton(cashSlotButton, text)) {
        setScreen(EnterPin); 
        resetGlobalTextVariables();
        displayText = "Please enter your PIN:";
    }

    DrawRectangle(startX, atmY + 720, slotWidth, slotHeight, DARKGRAY);
    DrawText("TAKE YOUR RECEIPT", startX + 10, atmY + 730, 20, WHITE);
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
    int textWidth = MeasureText(str.c_str(), 75);
    int textX = atmX + (atmWidth - textWidth) / 2;
    int textY = atmY + (atmHeight / 3); 
    DrawText((currencySymbol + str).c_str(), textX, textY, 75, ATM_TEXT);
}

void drawCardSlot() {
    int slotWidth = 300;
    int slotHeight = 10;
    int slotX = atmX + (atmWidth - slotWidth) / 2;
    int slotY = atmY + atmHeight - 80;

    int visibilityBtnWidth = screenWidth / 5;
    int visibilityBtnHeight = 50;
    int visibilityBtnX = atmX + 20; 
    int visibilityBtnY = atmY + atmHeight - visibilityBtnHeight - 20; 
    Rectangle visibilityBtn = {
        static_cast<float>(visibilityBtnX),
        static_cast<float>(visibilityBtnY),
        static_cast<float>(visibilityBtnWidth),
        static_cast<float>(visibilityBtnHeight)
    };

    if (GuiButton(visibilityBtn, "Visibility Options")) {
        setScreen(displayOptions);
        resetGlobalTextVariables();
        displayText = "Choose from the sizing options below:";
    }

    // Uncomment if needed to draw the card slot
    // DrawRectangle(slotX, slotY, slotWidth, slotHeight, DARKGRAY);
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
    int buttonWidth = 120;
    int buttonHeight = 60;
    int spacing = 20;

    int keypadWidth = 3 * buttonWidth + 2 * spacing;
    int keypadHeight = 5 * buttonHeight + 4 * spacing;
    
    int startX = atmX + atmWidth + (atmWidth / 7);
    int startY = atmY + (keypadHeight - 100);    

    float x = startX;
    float y = startY;

    for (int row = 0; row < 5; row++) { 
        for (int col = 0; col < 3; col++) {
            string keypadButtonText = keyPad[row][col];
            if (keypadButtonText == " ") { 
                x += (buttonWidth + spacing); 
                continue; 
            }

            Rectangle btnRect = { x, y, static_cast<float>(buttonWidth), static_cast<float>(buttonHeight) };

            GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, getButtonColor(keypadButtonText));
            GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL, ColorToInt(DARKGRAY));
            GuiSetStyle(BUTTON, BORDER_COLOR_FOCUSED, ColorToInt(LIGHTGRAY));
            GuiSetStyle(BUTTON, BORDER_COLOR_PRESSED, ColorToInt(WHITE));

            if (GuiButton(btnRect, keypadButtonText.c_str())) {
                handleInput(keypadButtonText);
            }

            x += (buttonWidth + spacing);
        }
        x = startX;
        y += (buttonHeight + spacing);
    }

    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(DARKGRAY)); 
    GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL, ColorToInt(DARKGRAY));
    GuiSetStyle(BUTTON, BORDER_COLOR_FOCUSED, ColorToInt(LIGHTGRAY));
    GuiSetStyle(BUTTON, BORDER_COLOR_PRESSED, ColorToInt(WHITE));
}

void setCurrency(Currency currency) {
    switch (currency) {
        case GBP:
            currentCurrency = GBP;
            currencySymbol = '\u00A3'; // Pound symbol (£)
            break;
        case USD:
            currentCurrency = USD;
            currencySymbol = '$';
            break;
        case JPY:
            currentCurrency = JPY;
            currencySymbol = '\u00A5'; // Yen symbol (¥)
            break;
        case EUR:
            currentCurrency = EUR;
            currencySymbol = '\u20AC'; // Euro symbol (€)
            break;
        case AUD:
            currentCurrency = AUD;
            currencySymbol = '$'; // Australian Dollar shares the Dollar symbol
            break;
        case CAD:
            currentCurrency = CAD;
            currencySymbol = '$'; // Canadian Dollar shares the Dollar symbol
            break;
        case CHF:
            currentCurrency = CHF;
            currencySymbol = 'F'; // Swiss Franc (not a standard symbol, placeholder "F")
            break;
        case CNH:
            currentCurrency = CNH;
            currencySymbol = '\u5143'; 
            break;
        case HKD:
            currentCurrency = HKD;
            currencySymbol = '$'; 
            break;
        case NZD:
            currentCurrency = NZD;
            currencySymbol = '$'; 
            break;
        default:
            currentCurrency = USD;
            currencySymbol = '$';
            break;
    }
}

void screenInit() {
    InitWindow(0, 0, "raygui - NCR ATM");
    setupGuiStyle();
    ToggleFullscreen();
    SetTargetFPS(60);
    screenWidth = GetMonitorWidth(0);  
    screenHeight = GetMonitorHeight(0);

    atmX = (screenWidth - atmWidth) / 6;
    atmY = (screenHeight - atmHeight) / 2;
    GuiSetStyle(DEFAULT, TEXT_SIZE, screenHeight/40);
}

void drawSideButtons() {
    int buttonWidth = 90;
    int buttonHeight = 70;
    int buttonSpacing = 100;

    int startXLeft = atmX - buttonWidth - 15;
    int startXRight = atmX + atmWidth + 20;

    int totalButtonHeight = (4 * buttonHeight) + (3 * buttonSpacing);
    int startY = atmY + ((atmHeight - totalButtonHeight) / 2);

    for (int i = 0; i < 4; i++) {
        Rectangle btnRect = {
            static_cast<float>(startXLeft),
            static_cast<float>(startY + (i * (buttonHeight + buttonSpacing))),
            static_cast<float>(buttonWidth),
            static_cast<float>(buttonHeight)
        };

        GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(GRAY));
        GuiSetStyle(BUTTON, BASE_COLOR_FOCUSED, ColorToInt(LIGHTGRAY));
        GuiSetStyle(BUTTON, BASE_COLOR_PRESSED, ColorToInt(WHITE));
        GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, ColorToInt(WHITE));
        GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL, ColorToInt(DARKGRAY));
        GuiSetStyle(BUTTON, BORDER_WIDTH, 2);

        if (GuiButton(btnRect, ">")) {
            handleInput("");
        }
    }

    for (int i = 0; i < 4; i++) {
        Rectangle btnRect = {
            static_cast<float>(startXRight),
            static_cast<float>(startY + (i * (buttonHeight + buttonSpacing))),
            static_cast<float>(buttonWidth),
            static_cast<float>(buttonHeight)
        };

        GuiSetStyle(BUTTON, BASE_COLOR_FOCUSED, ColorToInt(LIGHTGRAY));
        GuiSetStyle(BUTTON, BASE_COLOR_PRESSED, ColorToInt(WHITE));
        GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, ColorToInt(WHITE));
        GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL, ColorToInt(DARKGRAY));
        GuiSetStyle(BUTTON, BORDER_WIDTH, 2);

        if (GuiButton(btnRect, "<")){
            handleInput(""); 
        }
    }
}

void drawATMScreen(const char* text) {
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
    int textWidth = MeasureText(text, 30);
    int textX = atmX + (atmWidth - textWidth) / 2; 
    int textY = atmY + 40; 
    DrawText(text, textX, textY, 30, ATM_TEXT);
}

void drawKeypadAndCardBackground() {
    int backgroundWidth = 435;
    int backgroundHeight = 750;
    int backgroundX = atmX + atmWidth + (atmWidth / 8);
    int backgroundY = atmY + 50;

    float rounding = 0.1f; 
    int borderThickness = 4;

    DrawRectangleRounded(
        (Rectangle){(float)backgroundX, (float)backgroundY, (float)backgroundWidth, (float)backgroundHeight},
        rounding,
        10, 
        LIGHTGRAY
    );

    DrawRectangleRoundedLines(
        (Rectangle){(float)backgroundX, (float)backgroundY, (float)backgroundWidth, (float)backgroundHeight},
        rounding,
        10, 
        DARKGRAY
    );
}


void drawAtmCasing() {
    DrawRectangle(50, 50, screenWidth, screenHeight, CASING_BACK_COLOR);
    // DrawRectangle(100, 100, screenWidth - 100, screenHeight - 200, CASING_FRONT_COLOR);
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