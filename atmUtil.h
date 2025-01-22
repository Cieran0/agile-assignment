#ifndef ATMUTIL_H
#define ATMUTIL_H

#include <iostream>
#include <fstream>
#include <thread>
#include <string>
#include <vector>
#include "raygui.h"

using namespace std;

struct Account {
    string PIN;
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

void drawBalanceChoices();
void withdraw();
void deposit();

void viewBalance();
void printBalance();
void printFunction(string balance);

void processingScreen(string messageToPrint);

#endif
