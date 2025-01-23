#include "atmUtil.h"
#include "net.h"
#include <functional>

void genericInputHandler(string buttonPressed, Screen lastScreen, size_t maxInputSize, bool condition, function<void()> function) {

    if(buttonPressed == " ") {
        return;
    }

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
            function();
        }
        return;
    }

    if(input.length() < maxInputSize) {
        input.push_back(buttonPressed[0]);
    }

}

void handleInput(string buttonPressed) {
    genericInputHandler(buttonPressed, WaitingForCard, 4, (input.size() == 4), [] {
            Response r = forwardToSocket(cardNumber, expiryDate,  2001, input, 0.0);

            if(r.succeeded == 0) {
                setScreen(MainMenu);
                balance = r.new_balance;
            }
            else {
                displayText = "Incorrect pin. try again";
                input.clear();
                updatePinDisplay();
                setScreen(EnterPin);
            }
    });

}

void handleWithdrawInput(const string& buttonPressed) {
    genericInputHandler(buttonPressed, MainMenu, 10, (!input.empty()), [] {
        double amount = std::stof(input);
        if (amount > balance) {
            withdrawlText = "Insufficient funds";
            input.clear();
        }
        else{
            Response r = forwardToSocket(cardNumber, expiryDate, 2001, enteredPIN, amount);
            if(r.succeeded == 0) {
                balance = r.new_balance;
                input.clear();
                setScreen(MainMenu);
            }
        }
        });
}

void handleDepositInput(const string& buttonPressed) {
    genericInputHandler(buttonPressed, MainMenu, 10, (!input.empty()), [] {
        float amount = std::stof(input);
        balance += amount;
        cout << "Deposited: £" << amount << endl;
        input.clear();
        setScreen(MainMenu);
    }
    );

}
