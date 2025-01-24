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
}

void atmLayout() {
    drawATMScreen(displayText.c_str());
    updatePinDisplay();
    if (!pinDisplay.empty()) {
        DrawText(pinDisplay.c_str(), atmX + 20, atmY + 60, 30, ATM_TEXT);
    }
    drawKeypad(handleInput);
    drawCashSlot("INSERTED CARD");
}

void displayTransactionChoices() {
    drawATMScreen("");
    for (int i = 0; i < 4; i++) {
        DrawRectangle(atmX + 203, atmY + 253 + i*80, 350, 60, DARKGRAY);
    }
    vector<Button> buttons = {
        {{(float)(atmX + 200), (float)(atmY + 250), 350, 60}, "Balance Inquiry", Balance},
        {{(float)(atmX + 200), (float)(atmY + 330), 350, 60}, "Cash Withdrawal", Withdraw},
        {{(float)(atmX + 200), (float)(atmY + 410), 350, 60}, "Deposit", Deposit},
        {{(float)(atmX + 200), (float)(atmY + 490), 350, 60}, "Exit", WaitingForCard}
    };
    drawButtons(buttons);
}

void drawWithdrawMenu() {
    drawATMScreen(withdrawlText.c_str());
    drawMoney(input);
    drawKeypad(handleWithdrawInput);
}

void drawBalanceChoices() {
    drawATMScreen("Select balance option");
    vector<Button> buttons = {
        {{static_cast<float>(atmX + 200), static_cast<float>(atmY + 300), 350.0f, 60.0f}, "View Balance", BalanceAmount},
        {{static_cast<float>(atmX + 200), static_cast<float>(atmY + 380), 350.0f, 60.0f}, "Print Balance", PrintBalance},
        {{static_cast<float>(atmX + 200), static_cast<float>(atmY + 460), 350.0f, 60.0f}, "Back to Main Menu", MainMenu}
    };    
    drawButtons(buttons);
    drawCashSlot("INSERTED CARD");
}

void viewBalance() {
    drawATMScreen("");
    DrawText("Your Current Balance:", atmX + 250, atmY + 100, 20, ATM_TEXT);
    DrawText(("£" + to_string(balance)).c_str(), atmX + 250, atmY + 120, 40, ATM_TEXT);

    vector<Button> buttons = {
        {{ static_cast<float>(atmX + 200), static_cast<float>(atmY + 450), 350.0f, 60.0f }, "Back to Main Menu", MainMenu }
    };
    drawButtons(buttons);
}

void drawDepositMenu() {
    drawATMScreen("Enter Deposit Amount:");
    drawMoney(input);
    drawKeypad(handleDepositInput);
    drawCashSlot("INSERT CASH HERE");
}

void printBalance() {
    static int counter = 0;
    if (counter < 60 * 5) { 
        counter++;
        drawATMScreen("Processing....");
        printBalanceToFile(to_string(balance));
    } else {
        drawATMScreen("Please take your receipt");
        vector<Button> buttons = {
            {{ static_cast<float>(atmX + 200), static_cast<float>(atmY + 450), 350.0f, 60.0f }, "Back to Main Menu", MainMenu }
        };
        drawButtons(buttons);
    }
    drawCashSlot("INSERTED CARD");
}

const std::unordered_map<Screen, std::function<void()>> screens = {
    {WaitingForCard, drawWaitingForCard},
    {EnterPin, atmLayout},
    {MainMenu, displayTransactionChoices},
    {Withdraw, drawWithdrawMenu},
    {Balance, drawBalanceChoices},
    {BalanceAmount, viewBalance},
    {Deposit, drawDepositMenu},
    {PrintBalance, printBalance}
};

void screenManager() {
    ClearBackground(GetColor(background));
    screens.at(screen)();
}