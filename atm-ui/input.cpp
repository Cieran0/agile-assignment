#include "atmUtil.h"
#include "net.h"
#include <functional>


void genericInputHandler(string buttonPressed, Screen lastScreen, size_t maxInputSize, bool condition, function<void()> function) {
    if(buttonPressed == " ") return;
    if (buttonPressed == "clear") {
        input.clear();
        return;
    }
    if (buttonPressed == "cancel") {
        input.clear();
        setScreen(lastScreen);
        return;
    }
    if(buttonPressed == "enter") {
        if(condition) { 
            return function();
        }
    }
    if(input.length() < maxInputSize) {
        input.push_back(buttonPressed[0]);
    }
}

void handleInput(string buttonPressed) {
    genericInputHandler(buttonPressed, WaitingForCard, 4, (input.size() == 4), [] {
        string temp = input;
        Response r = forwardToSocket(PIN_CHECK, atmID, currentCurrency, 0, cardNumber.c_str(), expiryDate.c_str(), temp.c_str());
        if(r.succeeded == 0) {
            setScreen(MainMenu);
            balance.balance = r.newBalance;
            balance.dotPosition = r.dotPosition;
            balance.currency = r.atmCurrency;
            enteredPIN = temp;
        }
        else {
            displayText = getStringInLanguage("INCORRECT_PIN_TEXT");
            input.clear();
            updatePinDisplay();
            setScreen(EnterPin);
        }
    });
}

void doNothing(string buttonPressed) {
    return;
}

void handleWithdrawInput(const string& buttonPressed) {
    genericInputHandler(buttonPressed, MainMenu, 10, (!input.empty()), [] {
        int amount = std::stof(input);
        Response r = forwardToSocket(WITHDRAWAL, atmID, currentCurrency, amount, cardNumber.c_str(), expiryDate.c_str(), enteredPIN.c_str());
        if (r.succeeded == INSUFFICIENT_FUNDS) {
            withdrawlText = getStringInLanguage("INSUFFICIENT_FUNDS_TEXT");
            input.clear();
        } else if (r.succeeded == 0) {
                balance.balance = r.newBalance;
                balance.dotPosition = r.dotPosition;
                balance.currency = r.atmCurrency;
                input.clear();
                setScreen(MainMenu);   
        } else {
            withdrawlText = getStringInLanguage("NETWORK_ERROR");
            input.clear();
        }
    });
}

void handleDepositInput(const string& buttonPressed) {
    genericInputHandler(buttonPressed, MainMenu, 10, (!input.empty()), [] {
        int amount = std::stof(input);
        Response r = forwardToSocket(DEPOSIT, atmID, currentCurrency, amount, cardNumber.c_str(), expiryDate.c_str(), enteredPIN.c_str());
        // Response r = forwardToSocket(cardNumber, expiryDate, ATM_ID, enteredPIN, -amount);
        if(r.succeeded == 0) {
            balance.balance = r.newBalance;
            balance.dotPosition = r.dotPosition;
            balance.currency = r.atmCurrency;
            input.clear();
            setScreen(MainMenu);
        }
    });
}
