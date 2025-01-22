#ifndef ATMUTIL_H
#define ATMUTIL_H

#include <iostream>
#include <fstream>
#include <thread>
#include <string>
#include <vector>
#include "raygui.h"

#define TRANSACTION_SUCESS 0
#define INSUFFICIENT_FUNDS 1
#define DATABASE_ERROR 2
#define INCORRECT_PIN 3

using namespace std;

struct Account {
    string PIN;
    double balance;
};

typedef struct {
    Rectangle bounds; 
    const char *text;
} Button;

struct Transaction{
    char cardNumber[20];
    char expiryDate[6];
    uint64_t atmID;
    uint64_t uniqueTransactionID;
    char pinNo[5];
    double withdrawalAmount;
};

struct Response {
    int succeeded;
    double new_balance;
};



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

void forwardToSocket(string cardNumber, string expiryDate, string transactionID, string atmID, string pin, double withdrawalAmount);
 
#endif
