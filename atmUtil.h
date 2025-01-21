#ifndef ATMUTIL_H
#define ATMUTIL_H

#include <iostream>
#include <string>
#include <vector>
#include "raygui.h"

using namespace std;


bool validatePIN(string enteredPIN);
void handleInput(string buttonPressed);
void displayTransactionChoices();
void setDisplayText(const string& text);
string getDisplayText();
void updatePinDisplay();
void displayTerminal();

void balance();
void withdraw();
void deposit();

#endif
