#ifndef ATMUTIL_H
#define ATMUTIL_H

#include <iostream>
#include <unistd.h>
#include <cstdint>
#include "raylib.h"
#include "transaction.h"
#include <vector>
#include <functional>
#include <ctime>
#include "Currency.hpp"
#include <map>

extern int screenWidth;  
extern int screenHeight; 

using namespace std;

#define ATM_ID 2001

#define ATM_BACKGROUND CLITERAL(Color){ 45, 49, 66, 255 }     // Dark blue-gray
#define ATM_DISPLAY_BG CLITERAL(Color){ 200, 247, 197, 255 }  // Light green
#define ATM_BUTTON_BG CLITERAL(Color){ 77, 83, 110, 255 }     // Medium blue-gray
#define ATM_BUTTON_HOVER CLITERAL(Color){ 137, 142, 163, 255 }
#define ATM_TEXT CLITERAL(Color){ 33, 37, 41, 255 }           // Dark gray


#define RAYGUI_BUTTON_BORDER_WIDTH   2
#define RAYGUI_BUTTON_TEXT_PADDING   10

enum Language {
    ENGLISH,
    SPANISH,
    FRENCH,
    GERMAN
};

enum Screen {
    WaitingForCard,
    LanguageSelect,
    EnterPin,
    MainMenu,
    Withdraw,
    Balance,
    BalanceAmount,
    Deposit,
    PrintBalance,
    displayOptions,
    currencySelect
};

// bad name probably but idk
enum class StringType {
    PIN_PROMPT,
    WITHDRAWAL_PROMPT,
    BALANCE_TEXT,
    INSERTED_CARD_TEXT,
    INSERT_HERE_TEXT,
    PROCESSING_TEXT,
    PRINTING_SUCCESSFUL_TEXT,
    INCORRECT_PIN_TEXT,
    INSUFFICIENT_FUNDS_TEXT,
    VIEW_BALANCE_TEXT,
    PRINT_BALANCE_TEXT,
    MAIN_MENU_TEXT,
    BALANCE_BTN_TEXT,
    DEPOSIT_BTN_TEXT,
    EXIT_BTN_TEXT,
    TAKE_RECEIPT_TEXT,
    WITHDRAWAL_TEXT,
    DEPOSIT_TEXT,
    BALANCE_OPTION_TEXT
};


struct Button {
    Rectangle bounds; 
    const char *text;
    Screen nextScreen;
};

struct testAccount {
std::string localPin;
double testBalance;
};


void updatePinDisplay();
void screenManager();
void resetGlobalTextVariables();
void printBalance();
void printBalanceToFile(string balance);
void processingScreen(string messageToPrint);
void screenInit();
void drawKeypad(const std::function<void(const string&)>& handleInput);
void drawPrintedReceipt();
string getStringInLanguage(string prompt);
void setLanguage(Language language);

void setCurrency(Currency currency);

void handleInput(string buttonPressed);
void handleWithdrawInput(const string& buttonPressed); 
void handleDepositInput(const string& buttonPressed);
void doNothing(string buttonPressed);
double getFormattedBalance(Response r);

void drawCashSlot(const char* text);
void drawButtons(vector<Button> buttons);
void drawMoney(std::string str);
void drawCardSlot();
void drawATMScreen(const char* text);
void drawLanguages();
void drawAtmCasing();
void drawSideButtons(const vector<Button>& buttons);
void drawKeypadAndCardBackground();

void drawATM(const char* text);

extern vector<string> inputs;
extern string displayText;
extern string balanceText;
extern string withdrawlText;
extern string pinDisplay;
extern string enteredPIN;
extern string input;
extern string keyPad[5][3];
extern std::string cardNumber;
extern std::string expiryDate;
extern std::string currencySymbol;
extern Currency currentCurrency;
extern double balance;
extern enum Screen screen;
extern enum Currency currency;
extern uint64_t atmID;
extern std::map<std::string, std::string> englishStrings;
extern enum Language defaultLanguage;

extern int atmWidth;
extern int atmHeight;
extern int atmX;
extern int atmY;
extern int defaultButtonTextSize;

extern int background;

void setScreen(Screen s);

extern Font mainFont;

void DrawTextB(const char *text, int posX, int posY, int fontSize, Color color);
#endif
