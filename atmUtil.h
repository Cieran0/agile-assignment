#ifndef ATMUTIL_H
#define ATMUTIL_H

#include <iostream>
#include <unistd.h>
#include <cstdint>
#include "raylib.h"

using namespace std;


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
