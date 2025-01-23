#include "atmUtil.h"

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cstring>
#include <random>
#include <unordered_map>

int atmWidth = 750;
int atmHeight = 900;
int atmX;
int atmY;

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include <functional>

vector<string> inputs;
string displayText = "Please enter your PIN:"; // Global display text
string balanceText = "Enter amount for withdrawl";
string withdrawlText = "Enter withdrawal amount";

string pinDisplay = ""; // To show asterisks for PIN
string enteredPIN;

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

    if(str.empty()) {
        str = "0";
    }

    DrawText(("£" + str).c_str(), atmX + 20, atmY + 40, 30, ATM_TEXT);
}

void drawCardSlot() {
    int slotWidth  = 200;
    int slotHeight = 10;
    int slotX = atmX + (atmWidth - slotWidth) / 2;
    int slotY = atmY + atmHeight - 80; 
    DrawRectangle(slotX, slotY, slotWidth, slotHeight, DARKGRAY);

    Rectangle cardBtn = {
        (float)slotX,
        (float)(slotY - 40),
        (float)slotWidth,
        30
    };
    if (GuiButton(cardBtn, "INSERT CARD")) {
        setScreen(EnterPin);
        resetGlobalTextVariables();
        displayText = "Please enter your PIN:";
    }
}

void drawATMScreen(const char* text) {
    DrawRectangle(atmX, atmY, atmWidth, atmHeight, ATM_BACKGROUND);
    DrawRectangleLines(atmX, atmY, atmWidth, atmHeight, DARKGRAY);

    DrawRectangle(atmX, atmY, atmWidth, 200, DARKGRAY);
    DrawRectangle(atmX + 10, atmY + 10, atmWidth - 20, 180, ATM_DISPLAY_BG);
    DrawText(text, atmX + 20, atmY + 20, 20, ATM_TEXT);
}

void drawDepositMenu() {
    drawATMScreen("Enter Deposit Amount:");
    drawMoney(input);
    drawKeypad(handleDepositInput);
    drawCashSlot("INSERT CASH HERE");
}

void drawWithdrawMenu() {
    drawATMScreen(withdrawlText.c_str());
    drawMoney(input);
    drawKeypad(handleWithdrawInput);
}

void drawWaitingForCard() {
    drawATMScreen("Please Insert Card");
    drawCardSlot();
}

void printBalance() {
   
   static int counter = 0;
   if (counter < 60 * 5) { 
       counter++;
       drawATMScreen("Processing....");
       printBalanceToFile(to_string(balance));
   } else {
       drawATMScreen("Please take your receipt");
       Rectangle backBtn = {
           static_cast<float>(atmX + 200), 
           static_cast<float>(atmY + 450), 
           350.0f, 
           60.0f
       };

       if (GuiButton(backBtn, "Back to Main Menu")) {
           setScreen(MainMenu);
           counter = 0;  
       }
   }

    drawCashSlot("INSERTED CARD");
}

void processingScreen(string messageToPrint){
    static int counter = 0;
    if (counter < 60*5)
    {
        counter++;
        DrawText("Processing...", 470, 170, 20, ATM_TEXT);
    } else {
        DrawText("Printing successful. ", 200, 500, 40, BLACK);
        if (GuiButton({200, 600, 200, 50}, "Back")) {
            setScreen(MainMenu); 
        }
    }
}

void viewBalance() {
    
    drawATMScreen("");
    DrawText("Your Current Balance:", atmX + 250, atmY + 100, 20, ATM_TEXT);
    DrawText(("£" + to_string(balance)).c_str(), atmX + 250, atmY + 120, 40, ATM_TEXT);

    Rectangle backBtn = { static_cast<float>(atmX + 200), static_cast<float>(atmY + 450), 350.0f, 60.0f };

    if (GuiButton(backBtn, "Back to Main Menu")) {
        setScreen(MainMenu);
    }
}

void drawBalanceChoices() {

    drawATMScreen("Select balance option");

    vector<Button> buttons = {
        {{static_cast<float>(atmX + 200), static_cast<float>(atmY + 300), 350.0f, 60.0f}, "View Balance", BalanceAmount},
        {{static_cast<float>(atmX + 200), static_cast<float>(atmY + 380), 350.0f, 60.0f}, "Print Balance", PrintBalance},
        {{static_cast<float>(atmX + 200), static_cast<float>(atmY + 460), 350.0f, 60.0f}, "Back to Main Menu", MainMenu}
    };    

    drawButtons(buttons);

    drawCashSlot("INSERTED CARD");
}

const std::unordered_map<Screen, std::function<void()>> screens = {
    {WaitingForCard, drawWaitingForCard},
    {EnterPin, atmLayout},
    {MainMenu, displayTransactionChoices},
    {Withdraw, drawWithdrawMenu},
    {Balance, drawBalanceChoices},
    {BalanceAmount, viewBalance},
    {Deposit, drawDepositMenu},
    {PrintBalance, printBalance}
};

void screenManager(){
    ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
    screens.at(screen)();
}

void setupGuiStyle() {
    // Set up button colors
    GuiSetStyle(DEFAULT, BORDER_COLOR_NORMAL, ColorToInt(DARKGRAY));
    GuiSetStyle(DEFAULT, BASE_COLOR_NORMAL, ColorToInt(ATM_BUTTON_BG));
    GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, ColorToInt(WHITE));
    
    // Hover colors
    GuiSetStyle(DEFAULT, BASE_COLOR_FOCUSED, ColorToInt(ATM_BUTTON_HOVER));
    GuiSetStyle(DEFAULT, TEXT_COLOR_FOCUSED, ColorToInt(WHITE));
    
    // Pressed colors
    GuiSetStyle(DEFAULT, BASE_COLOR_PRESSED, ColorToInt(DARKGRAY));
    GuiSetStyle(DEFAULT, TEXT_COLOR_PRESSED, ColorToInt(WHITE));
    
    // Border width
    GuiSetStyle(DEFAULT, BORDER_WIDTH, RAYGUI_BUTTON_BORDER_WIDTH);
    
    // Text alignment
    GuiSetStyle(DEFAULT, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
    
    // Button padding
    GuiSetStyle(DEFAULT, TEXT_PADDING, RAYGUI_BUTTON_TEXT_PADDING);
    
    // Button rounding
    GuiSetStyle(DEFAULT, BORDER_WIDTH, 2);
    GuiSetStyle(BUTTON, BORDER_WIDTH, 2);
    GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL, ColorToInt(DARKGRAY));
    GuiSetStyle(BUTTON, BORDER_COLOR_FOCUSED, ColorToInt(LIGHTGRAY));
    GuiSetStyle(BUTTON, BORDER_COLOR_PRESSED, ColorToInt(WHITE));
}

void displayTransactionChoices()
{
    drawATMScreen("");

    for (int i = 0; i < 4; i++) {
        DrawRectangle(
            atmX + 203,           
            atmY + 253 + i*80, 
            350, 
            60, 
            DARKGRAY
        );
    }

    vector<Button> buttons = {
        {{(float)(atmX + 200), (float)(atmY + 250), 350, 60}, "Balance Inquiry", Balance},
        {{(float)(atmX + 200), (float)(atmY + 330), 350, 60}, "Cash Withdrawal", Withdraw},
        {{(float)(atmX + 200), (float)(atmY + 410), 350, 60}, "Deposit", Deposit},
        {{(float)(atmX + 200), (float)(atmY + 490), 350, 60}, "Exit", WaitingForCard}
    };

    drawButtons(buttons);
}

int getButtonColor(const std::string& text) {
    if (text == "cancel") 
        return ColorToInt(RED);
    if (text == "clear") 
        return ColorToInt(YELLOW);
    if (text == "enter") 
        return ColorToInt(GREEN);
    return ColorToInt(GRAY);
}

void drawKeypad(const std::function<void(const string&)>& handleInput) {
    int keypadWidth  = 300;
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

void atmLayout() 
{
    drawATMScreen(displayText.c_str());
    updatePinDisplay();

    if (!pinDisplay.empty()) {
        DrawText(pinDisplay.c_str(), atmX + 20, atmY + 60, 30, ATM_TEXT);
    }

    drawKeypad(handleInput);
    drawCashSlot("INSERTED CARD");
}


void screenInit() {
    InitWindow(0, 0, "raygui - NCR ATM");

    setupGuiStyle();
    ToggleFullscreen();
    SetTargetFPS(60);
    screenWidth = GetMonitorWidth(0);  
    screenHeight = GetMonitorHeight(0);
    atmX = screenWidth / 2.5f;
    atmY = screenHeight / 5;
    GuiSetStyle(DEFAULT,TEXT_SIZE ,screenHeight/100);
}