#include "atmUtil.h"
#include "raygui.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <string>
#include <fstream>
#include <cstring>
#include <random>
#include <unordered_map>

void drawWaitingForCard() {
    drawATMScreen(getStringInLanguage("INSERT_HERE_TEXT").c_str());
    drawCardSlot();
    drawSideButtons({});
    drawKeypadAndCardBackground();
    drawKeypad(handleInput);
    drawCashSlot(getStringInLanguage("INSERT_HERE_TEXT").c_str());
}

void atmLayout() {
    drawAtmCasing();
    drawATMScreen(displayText.c_str());
    updatePinDisplay();
    if (!pinDisplay.empty()) {
        int textWidth = MeasureText(pinDisplay.c_str(), 75);
        int textX = atmX + (atmWidth - textWidth) / 2;
        int textY = atmY + (atmHeight / 3); 
        DrawTextB(pinDisplay.c_str(), textX, textY, 75, ATM_TEXT); 
    }
    drawSideButtons({});
    drawKeypadAndCardBackground();
    drawKeypad(handleInput);
    drawCashSlot(getStringInLanguage("INSERT_HERE_TEXT").c_str());
}

void displayTransactionChoices() {
    drawATMScreen("Select an option from below.");

    int buttonWidth = 350;
    int buttonHeight = 60;
    int buttonSpacing = 110; 

    int totalButtonHeight = (4 * buttonHeight) + (3 * buttonSpacing);
    int startY = atmY + ((atmHeight - totalButtonHeight) / 2);
    int startX = atmX + 50;

    vector<Button> sideButtons = {
        {{}, "Balance Inquiry", Balance},
        {{}, "Cash Withdrawal", Withdraw},
        {{}, "Deposit", Deposit},
        {{}, "Exit", WaitingForCard},
        {{}, "", {}},
        {{}, "", {}},
        {{}, "", {}},
        {{}, "", {}}

    };

    drawSideButtons(sideButtons);
    drawKeypadAndCardBackground();
    drawKeypad(doNothing);
    drawCashSlot(getStringInLanguage("INSERT_HERE_TEXT").c_str());
}

void drawWithdrawMenu() {
    drawATMScreen(withdrawlText.c_str());
    drawMoney(input);
    drawSideButtons({});
    drawKeypadAndCardBackground();
    drawCashSlot(getStringInLanguage("INSERT_HERE_TEXT").c_str());
    drawKeypad(handleWithdrawInput);
}

void drawBalanceChoices() {
    drawATMScreen(getStringInLanguage("BALANCE_OPTION_TEXT").c_str());

    int buttonWidth = 350;
    int buttonHeight = 60;
    int buttonSpacing = 110;

    int totalButtonHeight = (4 * buttonHeight) + (3 * buttonSpacing);
    int startY = atmY + ((atmHeight - totalButtonHeight) / 2);
    int startX = atmX + 50; 

    vector<Button> sideButtons = {
        {{}, "View Balance", BalanceAmount},
        {{}, "Print Balance", PrintBalance},
        {{}, "Back to Main Menu", MainMenu},
        {{}, "", {}},
        {{}, "", {}},
        {{}, "", {}},
        {{}, "", {}},
        {{}, "", {}},

    };

    drawSideButtons(sideButtons);
    drawKeypadAndCardBackground();
    drawKeypad(doNothing);
    drawCashSlot(getStringInLanguage("INSERT_HERE_TEXT").c_str());
}

void viewBalance() {
    drawATMScreen("");
    int textWidth = MeasureText("Your Current Balance", 30);
    int textX = atmX + (atmWidth - textWidth) / 2; 
    int textY = atmY + 40; 

    DrawTextB("Your Current Balance:", textX, textY, 30, ATM_TEXT);

    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << balance;
    DrawTextB((currencySymbol + ss.str()).c_str(), textX + 40, textY + 100, 50, ATM_TEXT);

    float buttonWidth  = 350;
    float buttonHeight = 60;
    float buttonX = atmX + (atmWidth - buttonWidth) / 2;
    float buttonY = textY + 200;
    
    vector<Button> buttons = {
        {{ buttonX, buttonY, buttonWidth, buttonHeight }, "Back to Main Menu", MainMenu}
    };

    drawButtons(buttons);
    drawSideButtons({});
    drawKeypadAndCardBackground();
    drawKeypad(doNothing);
    drawCashSlot(getStringInLanguage("INSERT_HERE_TEXT").c_str());

}

void drawDepositMenu() {
    drawATMScreen(getStringInLanguage("DEPOSIT_TEXT").c_str());
    drawMoney(input);
    drawSideButtons({});
    drawCashSlot("INSERT CASH HERE");
    drawKeypadAndCardBackground();
    drawKeypad(handleDepositInput);
    drawCashSlot(getStringInLanguage("INSERT_HERE_TEXT").c_str());
}

void drawLanguageSelect() {
    drawATMScreen("Select a language:");
    drawLanguages();
    drawSideButtons({});
    drawKeypadAndCardBackground();
    drawKeypad(handleInput);
}

void printBalance() {
    static int counter = 0;

    bool finished = (counter >= 60 * 5);

    drawATMScreen(finished ? "Please take your receipt" : "Processing....");

    if (!finished) {
        counter++;
        printBalanceToFile(to_string(balance));
    }

    drawCashSlot("INSERTED CARD");
    drawSideButtons({});
    drawKeypadAndCardBackground();
    drawKeypad(handleInput);
    drawCashSlot(getStringInLanguage("INSERT_HERE_TEXT").c_str());

    if (finished) {
        drawPrintedReceipt(); 
        float buttonWidth  = 350;
        float buttonHeight = 60;
        float buttonX = (atmX + (atmWidth - buttonWidth) / 2) - 25;
        float buttonY = atmY + 250;
    
    vector<Button> buttons = {
        {{ buttonX, buttonY, buttonWidth, buttonHeight }, "Back to Main Menu", MainMenu}
    };

    drawButtons(buttons);
    }
}

void fontSizes(){
    // guibutton
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
