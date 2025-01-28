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
uint64_t atmID = rand_uint64();
enum Screen screen = WaitingForCard;


std::string cardNumber = "5030153826527268";
std::string expiryDate = "06/28";
double balance = 0;

//Globally declared account to allow for testing when not connected to switch and network
testAccount a1 = {localPin: "1234", testBalance : 1000};

string keyPad[5][3] = {
    {"1", "2", "3"},
    {"4", "5", "6"},
    {"7", "8", "9"},
    {" ", "0", " "},
    {"cancel", "clear", "enter"}
};


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

uint64_t rand_uint64() {
    srand((unsigned int)time(NULL));
    uint64_t r = 0;
    for (int i = 0; i < 64; i += 15) {
        r = r * ((uint64_t)RAND_MAX + 1) + rand();
    }
    return r % ((uint64_t)1 << 63);
}