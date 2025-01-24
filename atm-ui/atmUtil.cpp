#include "atmUtil.h"
#include "net.h"
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cstring>
#include <random>

bool validatedPIN = false;

string input = "";
enum Screen screen = WaitingForCard;

std::string cardNumber = "5030153826527268";
std::string expiryDate = "06/28";
double balance = 0;

string keyPad[4][4] = {{"1", "2", "3", "cancel"},
                       {"4", "5", "6", "clear"},
                       {"7", "8", "9", "enter"},
                       {" ", "0", " ", " "}};

vector<string> inputs;
string displayText = "Please enter your PIN:";
string balanceText = "Enter amount for withdrawl";
string withdrawlText = "Enter withdrawal amount";
string pinDisplay = "";
string enteredPIN;

void resetGlobalTextVariables() {
    input.clear();
    pinDisplay = "";
    displayText = "Please enter your PIN:";
    balanceText = "Enter amount for withdrawl";
    withdrawlText = "Enter withdrawal amount";
}

void updatePinDisplay() {
    pinDisplay = string(input.size(), '*');
}

void printBalanceToFile(string balance){
    ofstream balancePrint("printedBalance.txt");
    balancePrint << "Account balance: " << balance << endl;
    balancePrint.close();
}

void setScreen(Screen s) {
    screen = s;
    resetGlobalTextVariables();
    if(s == Screen::EnterPin) {
        enteredPIN = "";
    }
}