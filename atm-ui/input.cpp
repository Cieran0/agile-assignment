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

double getFormattedBalance(Response r) {
    string rawBalance = to_string(r.newBalance);
    int dotPos = static_cast<int>(r.dotPosition);
    if (dotPos > 0 && dotPos < static_cast<int>(rawBalance.size())) {
        rawBalance.insert(rawBalance.size() - dotPos, 1, '.');
    }
    double formattedBalance = stod(rawBalance);
    formattedBalance = round(formattedBalance * 100.0) / 100.0;
    return formattedBalance;
}

void handleInput(string buttonPressed) {
    genericInputHandler(buttonPressed, WaitingForCard, 4, (input.size() == 4), [] {
        string temp = input;
        Response r = forwardToSocket(PIN_CHECK, atmID, currentCurrency, 0, cardNumber.c_str(), expiryDate.c_str(), temp.c_str());
        if(r.succeeded == 0) {
            setScreen(MainMenu);
            balance = getFormattedBalance(r);
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
        if (amount > balance) {
            withdrawlText = getStringInLanguage("INSUFFICIENT_FUNDS_TEXT");
            input.clear();
        } else {
            std::cout << "PIN: [" << enteredPIN.c_str() << "]" << std::endl;
            Response r = forwardToSocket(WITHDRAWAL, atmID, currentCurrency, amount, cardNumber.c_str(), expiryDate.c_str(), enteredPIN.c_str());
            // Response r = forwardToSocket(cardNumber, expiryDate, ATM_ID, enteredPIN, amount);
            if (r.succeeded == 0) {
                balance = getFormattedBalance(r);
                input.clear();
                setScreen(MainMenu);
            }
        }
    });
}

void handleDepositInput(const string& buttonPressed) {
    genericInputHandler(buttonPressed, MainMenu, 10, (!input.empty()), [] {
        int amount = std::stof(input);
        Response r = forwardToSocket(DEPOSIT, atmID, currentCurrency, amount, cardNumber.c_str(), expiryDate.c_str(), enteredPIN.c_str());
        // Response r = forwardToSocket(cardNumber, expiryDate, ATM_ID, enteredPIN, -amount);
        if(r.succeeded == 0) {
            balance = getFormattedBalance(r);
            input.clear();
            setScreen(MainMenu);
        }
    });
}
