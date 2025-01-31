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
    drawATMScreen(getStringInLanguage("INSERT_CARD_TEXT").c_str());
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
    drawATMScreen(getStringInLanguage("SELECT_OPTION_TEXT").c_str());

    int buttonWidth = 350;
    int buttonHeight = 60;
    int buttonSpacing = 110; 

    int totalButtonHeight = (4 * buttonHeight) + (3 * buttonSpacing);
    int startY = atmY + ((atmHeight - totalButtonHeight) / 2);
    int startX = atmX + 50;

    vector<Button> sideButtons = {
        {{}, getStringInLanguage("BALANCE_BTN_TEXT").c_str(), Balance},
        {{}, getStringInLanguage("WITHDRAWAL_TEXT").c_str(), Withdraw},
        {{}, getStringInLanguage("DEPOSIT_BTN_TEXT").c_str(), Deposit},
        {{}, getStringInLanguage("EXIT_BTN_TEXT").c_str(), WaitingForCard},
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
        {{}, getStringInLanguage("VIEW_BALANCE_TEXT").c_str(), BalanceAmount},
        {{}, getStringInLanguage("PRINT_BALANCE_TEXT").c_str(), PrintBalance},
        {{}, getStringInLanguage("MAIN_MENU_TEXT").c_str(), MainMenu},
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
    int textWidth = MeasureText(getStringInLanguage("BALANCE_TEXT").c_str(), 30);
    int textX = atmX + (atmWidth - textWidth) / 2; 
    int textY = atmY + 40; 

    DrawTextB(getStringInLanguage("BALANCE_TEXT").c_str(), textX, textY, 30, ATM_TEXT);

    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << balance;
    DrawTextB((currencySymbol + ss.str()).c_str(), textX + 40, textY + 100, 50, ATM_TEXT);

    float buttonWidth  = 350;
    float buttonHeight = 60;
    float buttonX = atmX + (atmWidth - buttonWidth) / 2;
    float buttonY = textY + 200;
    
    vector<Button> buttons = {
        {{ buttonX, buttonY, buttonWidth, buttonHeight }, getStringInLanguage("MAIN_MENU_TEXT").c_str(), MainMenu}
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

    drawATMScreen(finished ? getStringInLanguage("TAKE_RECEIPT_TEXT").c_str() : getStringInLanguage("PROCESSING_TEXT").c_str());

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

    drawATMScreen(getStringInLanguage("SELECT_OPTION_TEXT").c_str());
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
