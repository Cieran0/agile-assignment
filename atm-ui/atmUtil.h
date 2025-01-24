#ifndef ATMUTIL_H
#define ATMUTIL_H

#include <iostream>
#include <unistd.h>
#include <cstdint>
#include "raylib.h"
#include <vector>
#include <functional>

extern int screenWidth;  
extern int screenHeight; 

using namespace std;

#define ATM_ID 2001

#define ATM_BACKGROUND CLITERAL(Color){ 45, 49, 66, 255 }     // Dark blue-gray
#define ATM_DISPLAY_BG CLITERAL(Color){ 200, 247, 197, 255 }  // Light green
#define ATM_BUTTON_BG CLITERAL(Color){ 77, 83, 110, 255 }     // Medium blue-gray
#define ATM_BUTTON_HOVER CLITERAL(Color){ 137, 142, 163, 255 }
#define ATM_TEXT CLITERAL(Color){ 33, 37, 41, 255 }           // Dark gray

#define RAYGUI_BUTTON_BORDER_WIDTH   2
#define RAYGUI_BUTTON_TEXT_PADDING   10

enum Screen {
    WaitingForCard = 0,
    EnterPin = 1,
    MainMenu = 2,
    Withdraw = 3,
    Balance = 4,
    BalanceAmount = 5,
    Deposit = 6,
    PrintBalance = 7
};

struct Button {
    Rectangle bounds; 
    const char *text;
    Screen nextScreen;
};

void handleInput(string buttonPressed);
void updatePinDisplay();
void screenManager();
void resetGlobalTextVariables();
void printBalance();
void printBalanceToFile(string balance);
void processingScreen(string messageToPrint);
void screenInit();
void drawKeypad(const std::function<void(const string&)>& handleInput);

void handleInput(string buttonPressed);
void handleWithdrawInput(const string& buttonPressed); 
void handleDepositInput(const string& buttonPressed);

void drawCashSlot(const char* text);
void drawButtons(vector<Button> buttons);
void drawMoney(std::string str);
void drawCardSlot();
void drawATMScreen(const char* text);

extern vector<string> inputs;
extern string displayText;
extern string balanceText;
extern string withdrawlText;
extern string pinDisplay;
extern string enteredPIN;
extern string input;
extern string keyPad[4][4];
extern std::string cardNumber;
extern std::string expiryDate;
extern double balance;
extern enum Screen screen;

extern int atmWidth;
extern int atmHeight;
extern int atmX;
extern int atmY;

extern int background;

void setScreen(Screen s);

#endif