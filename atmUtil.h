#ifndef ATMUTIL_H
#define ATMUTIL_H

#include <iostream>
#include <unistd.h>
#include <cstdint>
#include "raylib.h"

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

struct Account {
    string PIN;
    string cardNumber;
    string expiryDate;
    double balance;
};

typedef struct {
    Rectangle bounds; 
    const char *text;
} Button;


bool validatePIN(string enteredPIN);
void handleInput(string buttonPressed);
void displayTransactionChoices();
void setDisplayText(const string& text);
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
void printFunction(string balance);

void processingScreen(string messageToPrint);

void screenInit();

extern int screenWidth;
extern int screenHeight;

#endif
