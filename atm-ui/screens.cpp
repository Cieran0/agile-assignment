#include "atmUtil.h"
#include "raygui.h"
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
    drawSideButtons();
    drawKeypadAndCardBackground();
    drawKeypad(doNothing);
    drawCashSlot("INSERT CARD HERE");
}

void atmLayout() {
    drawAtmCasing();
    drawATMScreen(displayText.c_str());
    updatePinDisplay();
    // int textSize = MeasureText(pinDisplay.c_str(), 30);
    if (!pinDisplay.empty()) {
        int textWidth = MeasureText(pinDisplay.c_str(), 75);
        int textX = atmX + (atmWidth - textWidth) / 2;
        int textY = atmY + (atmHeight / 3); 
        DrawText(pinDisplay.c_str(), textX, textY, 75, ATM_TEXT); 
    }
    drawSideButtons();
    drawKeypadAndCardBackground();
    drawKeypad(handleInput);
    drawCashSlot("INSERT CARD HERE");
}

void displayTransactionChoices() {
    drawATMScreen("");

    int buttonWidth = 350;
    int buttonHeight = 60;
    int buttonSpacing = 110; 

    int totalButtonHeight = (4 * buttonHeight) + (3 * buttonSpacing);
    int startY = atmY + ((atmHeight - totalButtonHeight) / 2);
    int startX = atmX + 50;

    vector<Button> buttons = {

        {{(float)startX, (float)startY, (float)buttonWidth, (float)buttonHeight}, "Balance Inquiry", Balance},
        {{(float)startX, (float)(startY + (buttonHeight + buttonSpacing)), (float)buttonWidth, (float)buttonHeight}, "Cash Withdrawal", Withdraw},
        {{(float)startX, (float)(startY + 2 * (buttonHeight + buttonSpacing)), (float)buttonWidth, (float)buttonHeight}, "Deposit", Deposit},
        {{(float)startX, (float)(startY + 3 * (buttonHeight + buttonSpacing)), (float)buttonWidth, (float)buttonHeight}, "Exit", WaitingForCard}

    };
 

    drawButtons(buttons); 
    drawSideButtons();    
    drawKeypadAndCardBackground();
    drawKeypad(doNothing);
    drawCashSlot("INSERT CARD HERE");
}


void drawWithdrawMenu() {
    drawATMScreen(withdrawlText.c_str());
    drawMoney(input);
    drawSideButtons();
    drawKeypadAndCardBackground();
    drawCashSlot("INSERT CARD HERE");
    drawKeypad(handleWithdrawInput);
}

void drawBalanceChoices() {
    drawATMScreen("Select balance option");

    int buttonWidth = 350;
    int buttonHeight = 60;
    int buttonSpacing = 110;

    int totalButtonHeight = (4 * buttonHeight) + (3 * buttonSpacing);
    int startY = atmY + ((atmHeight - totalButtonHeight) / 2);
    int startX = atmX + 50; 

    vector<Button> buttons = {
        {{(float)startX, (float)startY, (float)buttonWidth, (float)buttonHeight}, "View Balance", BalanceAmount},
        {{(float)startX, (float)(startY + (buttonHeight + buttonSpacing)), (float)buttonWidth, (float)buttonHeight}, "Print Balance", PrintBalance},
        {{(float)startX, (float)(startY + 2 * (buttonHeight + buttonSpacing)), (float)buttonWidth, (float)buttonHeight}, "Back to Main Menu", MainMenu}
    };

    drawButtons(buttons);
    drawSideButtons();    
    drawKeypadAndCardBackground();
    drawKeypad(doNothing);
    drawCashSlot("INSERT CARD HERE");
}


void viewBalance() {
    drawATMScreen("");
    DrawText("Your Current Balance:", atmX + 250, atmY + 100, 20, ATM_TEXT);
    DrawText((currencySymbol + to_string(balance)).c_str(), atmX + 250, atmY + 120, 40, ATM_TEXT);

    vector<Button> buttons = {
        {{ static_cast<float>(atmX + 200), static_cast<float>(atmY + 450), 350.0f, 60.0f }, "Back to Main Menu", MainMenu }
    };
    drawButtons(buttons);
    drawSideButtons();
    drawKeypadAndCardBackground();
    drawKeypad(doNothing);
    drawCashSlot("INSERT CARD HERE");
}

void drawDepositMenu() {
    drawATMScreen("Enter Deposit Amount:");
    drawMoney(input);
    drawCashSlot("INSERT CASH HERE");
    drawSideButtons();
    drawKeypadAndCardBackground();
    drawKeypad(handleDepositInput);
    drawCashSlot("INSERT CARD HERE");
}

void drawLanguageSelect() {
    drawATMScreen("Select a language:");
    drawLanguages();
    drawSideButtons();
    drawKeypadAndCardBackground();
    drawKeypad(handleInput);
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
    drawSideButtons();
    drawKeypadAndCardBackground();
    drawKeypad(doNothing);
    drawCashSlot("INSERT CARD HERE");
}


void screenSizes(){

    drawATMScreen("Select an option:");
    int buttonWidth = 350;
    int buttonHeight = 60;
    int buttonSpacing = 110; 

    int totalButtonHeight = (4 * buttonHeight) + (3 * buttonSpacing);
    int startY = atmY + ((atmHeight - totalButtonHeight) / 2);
    int startX = atmX + 50;

    vector<Button> sizeOptionbuttons = {

        {{(float)startX, (float)startY, (float)buttonWidth, (float)buttonHeight}, "Small", displayOptions},
        {{(float)startX, (float)(startY + (buttonHeight + buttonSpacing)), (float)buttonWidth, (float)buttonHeight}, "Medium", displayOptions},
        {{(float)startX, (float)(startY + 2 * (buttonHeight + buttonSpacing)), (float)buttonWidth, (float)buttonHeight}, "Big", displayOptions},
        {{(float)startX, (float)(startY + 3 * (buttonHeight + buttonSpacing)), (float)buttonWidth, (float)buttonHeight}, "Exit", WaitingForCard}

    }; 
    for (auto&button : sizeOptionbuttons){
        if (GuiButton(button.bounds, button.text)){
            if (button.text == "Small"){
              SetWindowSize(800,500);
              atmHeight = 500;
              atmWidth = 375;
            }
            else if (button.text == "Medium"){

              SetWindowSize(1600,1000);
              atmHeight = 1000;
              atmWidth = 750;             
            }
            else if (button.text == "Big"){
              SetWindowSize(1920,1200);
              atmHeight = 1200;
              atmWidth = 900;
            }
            else{
                setScreen(WaitingForCard);
            }

        }
    }
    drawButtons(sizeOptionbuttons);
     
}



const std::unordered_map<Screen, std::function<void()>> screens = {
    {WaitingForCard, drawWaitingForCard},
    {LanguageSelect, drawLanguageSelect},
    {EnterPin, atmLayout},
    {MainMenu, displayTransactionChoices},
    {Withdraw, drawWithdrawMenu},
    {Balance, drawBalanceChoices},
    {BalanceAmount, viewBalance},
    {Deposit, drawDepositMenu},
    {PrintBalance, printBalance},
    {displayOptions, screenSizes}
};

void screenManager() {
    ClearBackground(ATM_BACKGROUND);
    screens.at(screen)();
}
