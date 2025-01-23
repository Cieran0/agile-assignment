#include "atmUtil.h"

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cstring>
#include <random>

#include "net.h"

bool validatedPIN = false;
int screenWidth = GetMonitorWidth(0);  
int screenHeight = GetMonitorHeight(0);

string input = "";

enum Screen screen = WaitingForCard;

std::string cardNumber = "5030153826527268";
std::string expiryDate = "06/28";

double balance = 0;

string keyPad[4][4] = {{"1", "2", "3", "cancel"},
                    {"4", "5", "6", "clear"},
                    {"7", "8", "9", "enter"},
                    {" ", "0", " ", " "}};


void resetGlobalTextVariables() {
    input.clear();
    enteredPIN = "";
    pinDisplay = "";
    displayText = "Please enter your PIN:";
    balanceText = "Enter amount for withdrawl";
    withdrawlText = "Enter withdrawal amount";
}

void updatePinDisplay() {
    pinDisplay = string(input.size(), '*'); // Convert PIN digits to asterisks
}

void printBalanceToFile(string balance){
    ofstream balancePrint;
    balancePrint.open("printedBalance.txt");
    balancePrint << "account balance" << balance << endl;
    balancePrint.close();
}

void setScreen(Screen s) {
    screen=s;
    resetGlobalTextVariables();
}