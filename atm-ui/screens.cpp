#include "atmUtil.h"
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cstring>
#include <random>
#include <unordered_map>

void drawWaitingForCard() {
    drawATMScreen("Please Insert Card");
    drawCardSlot();
    drawSideButtons({});
    drawKeypadAndCardBackground();
    drawKeypad(handleInput);
    drawCashSlot("INSERT CARD HERE");
}

void atmLayout() {
    drawAtmCasing();
    drawATMScreen(displayText.c_str());
    updatePinDisplay();
    if (!pinDisplay.empty()) {
        int textWidth = MeasureText(pinDisplay.c_str(), 75);
        int textX = atmX + (atmWidth - textWidth) / 2;
        int textY = atmY + (atmHeight / 3); 
        DrawText(pinDisplay.c_str(), textX, textY, 75, ATM_TEXT); 
    }
    drawSideButtons({});
    drawKeypadAndCardBackground();
    drawKeypad(handleInput);
    drawCashSlot("INSERT CARD HERE");
}

void displayTransactionChoices() {
    drawATMScreen("");

    vector<Button> sideButtons = {
        {{}, "Balance Inquiry", Balance},
        {{}, "Cash Withdrawal", Withdraw},
        {{}, "Deposit", Deposit},
        {{}, "Exit", WaitingForCard}
    };

    drawSideButtons(sideButtons);
    drawKeypadAndCardBackground();
    drawKeypad(handleInput);
    drawCashSlot("INSERT CARD HERE");
}

void drawWithdrawMenu() {
    drawATMScreen(withdrawlText.c_str());
    drawMoney(input);
    drawSideButtons({});
    drawKeypadAndCardBackground();
    drawCashSlot("INSERT CARD HERE");
    drawKeypad(handleWithdrawInput);
}

void drawBalanceChoices() {
    drawATMScreen("Select balance option");

    vector<Button> sideButtons = {
        {{}, "View Balance", BalanceAmount},
        {{}, "Print Balance", PrintBalance},
        {{}, "Back to Main Menu", MainMenu}
    };

    drawSideButtons(sideButtons);
    drawKeypadAndCardBackground();
    drawKeypad(handleInput);
    drawCashSlot("INSERT CARD HERE");
}

void viewBalance() {
    drawATMScreen("");
    DrawText("Your Current Balance:", atmX + 250, atmY + 100, 20, ATM_TEXT);
    DrawText((currencySymbol + to_string(balance)).c_str(), atmX + 250, atmY + 120, 40, ATM_TEXT);

    drawSideButtons({});
    drawKeypadAndCardBackground();
    drawKeypad(handleInput);
    drawCashSlot("INSERT CARD HERE");
}

void drawDepositMenu() {
    drawATMScreen("Enter Deposit Amount:");
    drawMoney(input);
    drawSideButtons({});
    drawKeypad(handleDepositInput);
    drawCashSlot("INSERT CASH HERE");
    drawKeypadAndCardBackground();
    drawKeypad(handleInput);
    drawCashSlot("INSERT CARD HERE");
}

void printBalance() {
    static int counter = 0;
    if (counter < 60 * 5) { 
        counter++;
        drawATMScreen("Processing....");
        printBalanceToFile(to_string(balance));
    } else {
        drawATMScreen("Please take your receipt");
        drawPrintedReciept();
        vector<Button> buttons = {
            {{ static_cast<float>(atmX + 200), static_cast<float>(atmY + 450), 350.0f, 60.0f }, "Back to Main Menu", MainMenu }
        };
        drawButtons(buttons);
    }
    drawCashSlot("INSERTED CARD");
    drawSideButtons({});
    drawKeypadAndCardBackground();
    drawKeypad(handleInput);
    drawCashSlot("INSERT CARD HERE");
}

void fontSizes(){

}

const std::unordered_map<Screen, std::function<void()>> screens = {
    {WaitingForCard, drawWaitingForCard},
    {EnterPin, atmLayout},
    {MainMenu, displayTransactionChoices},
    {Withdraw, drawWithdrawMenu},
    {Balance, drawBalanceChoices},
    {BalanceAmount, viewBalance},
    {Deposit, drawDepositMenu},
    {PrintBalance, printBalance},

    {displayOptions, fontSizes}
};

void screenManager() {
    ClearBackground(ATM_BACKGROUND);
    screens.at(screen)();
}
