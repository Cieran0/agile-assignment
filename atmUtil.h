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

bool validatePIN(string enteredPIN);
void handleInput(string buttonPressed);
void displayTransactionChoices();
string getDisplayText();
void updatePinDisplay();
void screenManager();
void atmLayout();
void drawWithdrawMenu();

void resetGlobalTextVariables();
void drawBalanceChoices();
void withdraw();
void deposit();

void viewBalance();
void printBalance();
void printBalanceToFile(string balance);

void processingScreen(string messageToPrint);

void screenInit();

extern int screenWidth;
extern int screenHeight;

#define ROW_COUNT 4
#define COLUMN_COUNT 4

#define ATM_BACKGROUND CLITERAL(Color){ 45, 49, 66, 255 }     // Dark blue-gray
#define ATM_DISPLAY_BG CLITERAL(Color){ 200, 247, 197, 255 }  // Light green
#define ATM_BUTTON_BG CLITERAL(Color){ 77, 83, 110, 255 }     // Medium blue-gray
#define ATM_BUTTON_HOVER CLITERAL(Color){ 137, 142, 163, 255 }
#define ATM_TEXT CLITERAL(Color){ 33, 37, 41, 255 }           // Dark gray

#define RAYGUI_BUTTON_BORDER_WIDTH   2
#define RAYGUI_BUTTON_TEXT_PADDING   10


extern vector<string> inputs;
extern string displayText;
extern string balanceText;
extern string withdrawlText;

void drawWaitingForCard();

void handleDepositInput(const string& buttonPressed);
void handleWithdrawInput(const string& buttonPressed);
void drawDepositMenu();

extern enum Screen screen;

extern string pinDisplay;
extern string enteredPIN;
extern string input;

extern string keyPad[4][4];

extern std::string cardNumber;
extern std::string expiryDate;
extern double balance;

void setScreen(Screen s);

void drawKeypad(const std::function<void(const string&)>& handleInput);
#endif
