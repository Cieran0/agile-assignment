#ifndef ATMUTIL_H
#define ATMUTIL_H

#include <iostream>
#include <string>
#include <vector>
#include "raygui.h"

using namespace std;

struct Account {
    string PIN;
    double balance;
};

bool validatePIN(string enteredPIN);
void handleInput(string buttonPressed);
void displayTransactionChoices();
void setDisplayText(const string& text);
string getDisplayText();
void updatePinDisplay();
void displayTerminal();
void drawMainMenu();
void drawWithdrawMenu();

void drawBalance();
void withdraw();
void deposit();

void presentBalance();

#endif
