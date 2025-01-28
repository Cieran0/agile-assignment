#ifndef ATMUTIL_H
#define ATMUTIL_H

#include <iostream>
#include <unistd.h>
#include <cstdint>
#include "raylib.h"
#include <vector>
#include <functional>
#include <ctime>
#include "Currency.hpp"

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
    WaitingForCard,
    EnterPin,
    MainMenu,
    Withdraw,
    Balance,
    BalanceAmount,
    Deposit,
    PrintBalance,
    displayOptions,
    currencySelect
};


struct Button {
    Rectangle bounds; 
    const char *text;
    Screen nextScreen;
};

struct testAccount {
std::string localPin;
double testBalance;
};

extern testAccount a1;

void handleInput(string buttonPressed);
void updatePinDisplay();
void screenManager();
void resetGlobalTextVariables();
void printBalance();
void printBalanceToFile(string balance);
void processingScreen(string messageToPrint);
void screenInit();
void drawKeypad(const std::function<void(const string&)>& handleInput);
void drawPrintedReciept();

void setCurrency(Currency currency);

void handleInput(string buttonPressed);
void handleWithdrawInput(const string& buttonPressed); 
void handleDepositInput(const string& buttonPressed);
uint64_t rand_uint64();

void drawCashSlot(const char* text);
void drawButtons(vector<Button> buttons);
void drawMoney(std::string str);
void drawCardSlot();
void drawATMScreen(const char* text);
void drawAtmCasing();
void drawSideButtons();
void drawKeypadAndCardBackground();

void drawATM(const char* text);

extern vector<string> inputs;
extern string displayText;
extern string balanceText;
extern string withdrawlText;
extern string pinDisplay;
extern string enteredPIN;
extern string input;
extern string keyPad[5][3];
extern std::string cardNumber;
extern std::string expiryDate;
extern char currencySymbol;
extern Currency currentCurrency;
extern double balance;
extern enum Screen screen;
extern enum Currency currency;
extern uint64_t atmID;

extern int atmWidth;
extern int atmHeight;
extern int atmX;
extern int atmY;

extern int background;

void setScreen(Screen s);

#endif