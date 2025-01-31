#include "raylib.h"
#include <vector>
#include <functional>
#include <string>
#include <iostream>
#include "Components.hpp"
#include "Screens.hpp"
#include "LocalString.hpp"
#include "Currency.hpp"
#include <future>
#include "Transaction.hpp"
#include <cstring>

std::string cardNumber = "";
std::string expiryDate = "";

std::vector<std::string> cardNumbers = {
    "5030153826527268", 
    "9307315805858911",
    "0832820726785104",
};

std::vector<std::string> expiryDates = {
    "06/28",
    "08/26",
    "05/26",
};

const ScreenButton emptyButton = {
    .text = std::string(),
    .func = [](){}
};

Transaction currentTransaction;

AtmID atmID;

AtmCurrency balance = 0;
DecimalPosition dotPosition = 0;

bool gotResponse = false;
Response response;

int64_t fastPower(int64_t base, int64_t exp) {
    int64_t result = 1;
    while (exp > 0) {
        if (exp % 2 == 1) {
            result *= base;
        }
        base *= base;
        exp /= 2;
    }
    return result;
}

std::string balanceAsString() {
    int64_t afterDot = dotPosition - 2;
    int64_t divisor = fastPower(10, dotPosition);
    int64_t wholeNumber = balance/divisor;

    if(afterDot == 0) {
        return currencySymbol + std::to_string(wholeNumber);
    }

    int64_t leftOver = (balance % divisor) / fastPower(10, afterDot);
    return currencySymbol + std::to_string(wholeNumber) + "." + std::to_string(leftOver);
}

uint64_t randomID() {
    srand((unsigned int)time(NULL));
    uint64_t r = 0;
    for(int i = 0; i < 64; i += 15){
        r = r * ((uint64_t)RAND_MAX + 1) + rand();
    }
    return r % ((uint64_t)1 << 63);
}


bool transactionStarted = false;
void processTransaction(TransactionType type, AtmCurrency amount) {
    std::memset(&currentTransaction, 0, sizeof(Transaction));
    std::memset(&response, 0, sizeof(Response));
    
    gotResponse = false;

    UniqueTranscationID id = randomID();

    currentTransaction = {
        .type = type,
        .id = id,
        .atmID = atmID,
        .currency = currentCurrency,
        .amount = amount,
    };

    strncpy(currentTransaction.cardNumber, cardNumber.c_str(), 19);
    strncpy(currentTransaction.expiryDate, expiryDate.c_str(), 5);
    strncpy(currentTransaction.pinNo, enteredPin.c_str(), 4);

    transactionStarted = false;

    screen = processingTransaction;
}

void drawATMButtons(const std::vector<ScreenButton>& screenButtons) {
    int buttonWidth = (atmWidth*3)/7;
    int buttonHeight = 120;
    int buttonSpacing = 50;

    int startXLeft = atmX + 30;
    int startXRight = atmX + atmWidth - buttonWidth - 30;

    int totalButtonHeight = (4 * buttonHeight) + (3 * buttonSpacing);
    int startY = atmY + ((atmHeight - totalButtonHeight) / 2);

    for (size_t i = 0; i < 8 && i < screenButtons.size(); i++)
    {
        int j = i;
        int xOffset = 0;
        if(i >= 4) {
            j = i-4;
            xOffset = startXRight-startXLeft;
        }

        float x = (float)startXLeft+xOffset;
        float y = (float)startY + j*((buttonHeight + buttonSpacing));

        if(GuiButton({x, y, (float)buttonWidth, (float)buttonHeight}, screenButtons[i].text, LIGHTGRAY, WHITE)) {
            screenButtons[i].func();
        }
    }
}

void drawScreen(std::string primaryText, std::string secondaryText, std::vector<ScreenButton> screenButtons, std::function<void(std::string)> keypadHandler, std::function<void()> insertCardHandler) {

    DrawRectangle(50, 50, screenWidth, screenHeight, CASING_BACK_COLOR);
    drawKeypadAndCardBackground();
    drawATMScreen(primaryText, secondaryText);
    drawKeypad(keypadHandler);
    drawSideButtons(screenButtons);
    drawCardSlot(inCurrentLanguage(LocalString::INSERT_CARD).c_str(), insertCardHandler);
    drawATMButtons(screenButtons);
}


void mainMenu() {

    ScreenButton balanceButton = {
        .text = inCurrentLanguage(LocalString::BALANCE_INQUIRY),
        .func = [](){ screen = balanceInquiryScreen; }
    };

    ScreenButton withdrawalButton = {
        .text = inCurrentLanguage(LocalString::WITHDRAWAL_MSG),
        .func = [](){ screen = withdrawalScreen;  }
    };

    ScreenButton depositButton = {
        .text = inCurrentLanguage(LocalString::DEPOSIT_MSG),
        .func = [](){ screen = depositScreen; }
    };

    ScreenButton exitButton = {
        .text = inCurrentLanguage(LocalString::EXIT),
        .func = [](){ screen = selectLanguageScreen; enteredPin = ""; }
    };

    std::vector<ScreenButton> screenButtons = {balanceButton,withdrawalButton,depositButton, exitButton};

    drawScreen(inCurrentLanguage(SELECT_OPTION), "", screenButtons, [](std::string str){ if (str == "cancel") screen = insertCardScreen; }, [](){});
}

void enterPinScreen() {
    std::string pinDisplay = std::string(enteredPin.size(), '*');

    drawScreen(inCurrentLanguage(ENTER_PIN), pinDisplay, {}, [](std::string str){ 
        if(enteredPin.length() == 4 && str.length() == 1)
            return;

        if(str.length() == 1) {
            enteredPin+=str;
            return;
        }

        if(str == inCurrentLanguage(LocalString::CANCEL)) {
            enteredPin = "";
            screen = selectLanguageScreen;
        } else if (str == inCurrentLanguage(LocalString::CLEAR)) {
            enteredPin = "";
        } else if (str == inCurrentLanguage(LocalString::ENTER) && enteredPin.length() == 4) {
            processTransaction(TransactionType::PIN_CHECK, 0);
        }

    }, [](){});


}

void drawWallet() {
    Rectangle rect = {static_cast<float>(atmX+atmWidth/6), static_cast<float>(atmY + atmHeight/7), static_cast<float>(atmWidth*2/3), static_cast<float>(atmHeight*5/6)};

    DrawRectangleRec(rect, BROWN);
    DrawRectangleLinesEx(rect, 1, BLACK);
    Rectangle cards[3] = {0};
    Color cardColors[3] = {RED, BLUE, LIME};

    for (int i = 0; i < 3; i++)
    {
        int cardWidth = (rect.width*2)/3;
        int cardHeight = (cardWidth/2);
        int cardX = rect.x + (rect.width-cardWidth)/2;
        int cardY = (rect.y + rect.height/10) + (((cardHeight*2)/3 + 10)*i);

        Rectangle card = {static_cast<float>(cardX), static_cast<float>(cardY), static_cast<float>(cardWidth), static_cast<float>(cardHeight)};

        DrawRectangleRounded(card, 0.1, 10, cardColors[i] );
        DrawRectangleRoundedLines(card, 0.1, 10, BLACK);

        DrawRectangle(cardX, cardY+(cardHeight/2 - cardHeight/8), cardWidth, cardHeight/4, BLACK);

        int textLen = MeasureTextEx(mainFont, cardNumbers[i].c_str(), cardHeight/5, 1.0).x;
        DrawTextFont(cardNumbers[i].c_str(), cardX + (cardWidth - textLen)/2,  cardY+(cardHeight/2 - cardHeight/8) + (cardHeight/4 - cardHeight/5)/2, cardHeight/5, WHITE);
    
        if(cardNumber == cardNumbers[i]) {
            DrawTextFont("X", cardX + cardHeight/10, cardY + cardHeight/10, cardHeight/5, BLACK);
        }

        cards[i] = card;
    }
    
    if(!IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        return;
    }
    
    int mouseX = GetMouseX();
    int mouseY = GetMouseY();

    for (int i = 0; i < 3; i++)
    {
        if(inBounds(cards[i], mouseX, mouseY)) {
            cardNumber = cardNumbers[i];
            expiryDate = expiryDates[i];
        }
    }
    
}

void insertCardScreen() {

    drawScreen(inCurrentLanguage(LocalString::INSERT_CARD), "", {}, [](std::string str){}, [](){
        if(!cardNumber.empty()) {
            screen = enterPinScreen;
        }
    });

    drawWallet();
}

void selectLanguageScreen() {

    ScreenButton englishButton = {
        .text = inCurrentLanguage(LocalString::L_ENGLISH),
        .func = [](){ screen = insertCardScreen; currentLanguage = Language::ENGLISH; }
    };

    ScreenButton frenchButton = {
        .text = inCurrentLanguage(LocalString::L_FRENCH),
        .func = [](){ screen = insertCardScreen; currentLanguage = Language::FRENCH; }
    };

    ScreenButton spanishButton = {
        .text = inCurrentLanguage(LocalString::L_SPANISH),
        .func = [](){ screen = insertCardScreen; currentLanguage = Language::SPANISH; }
    };

    ScreenButton germanButton = {
        .text = inCurrentLanguage(LocalString::L_GERMAN),
        .func = [](){ screen = insertCardScreen; currentLanguage = Language::GERMAN; }
    };

    drawScreen(inCurrentLanguage(LocalString::SELECT_LANG), "", {englishButton,frenchButton,spanishButton,germanButton}, [](std::string str){}, [](){ });
}

void balanceInquiryScreen() {
    ScreenButton balanceButton = {
        .text = inCurrentLanguage(LocalString::VIEW_BALANCE),
        .func = [](){ screen = balanceScreen; }
    };

    ScreenButton printButton = {
        .text = inCurrentLanguage(LocalString::PRINT_BALANCE),
        .func = [](){ screen = printBalanceScreen;  }
    };

    ScreenButton backButton = {
        .text = inCurrentLanguage(LocalString::BACK),
        .func = [](){ screen = mainMenu; }
    };

    drawScreen(inCurrentLanguage(LocalString::SELECT_OPTION), "", {balanceButton,printButton,backButton}, [](std::string str){}, [](){});
}

void printBalanceScreen() {
    static int counter = 0;
    std::string processing = inCurrentLanguage(LocalString::PROCESSING);
    std::string complete = inCurrentLanguage(LocalString::TAKE_RECEIPT);

    ScreenButton backButton = {
        .text = inCurrentLanguage(LocalString::BACK),
        .func = [](){ screen = mainMenu; counter = 0; resetReciept(); }
    };

    drawScreen(counter > 200 ? complete : processing, "", {emptyButton, emptyButton, emptyButton, backButton}, [](std::string str){}, [](){});

    if(counter > 200 ){
        drawPrintedReciept();
    } else {
        counter++;
    }
}

void balanceScreen() {

    ScreenButton backButton = {
        .text = inCurrentLanguage(LocalString::BACK),
        .func = [](){ screen = mainMenu; }
    };

    drawScreen(inCurrentLanguage(LocalString::YOUR_BALANCE), balanceAsString(), {emptyButton,emptyButton,emptyButton,backButton}, [](std::string str){}, [](){});
}


void processingTransaction() {
    static std::future<Response> futureResponse;

    std::string processing = inCurrentLanguage(LocalString::PROCESSING);
    std::string complete = inCurrentLanguage(LocalString::TRANSACTION_SUCCESS);
    std::string genericError = inCurrentLanguage(LocalString::GENERIC_ERROR_MSG);
    std::string insufficient_funds = inCurrentLanguage(LocalString::INSUFFICIENT_FUNDS_MSG);
    std::string incorrectPin = inCurrentLanguage(LocalString::INCORRECT_PIN_MSG);
    std::vector<ScreenButton> buttons;
    std::string text = processing;

    ScreenButton backButton = {
        .text = inCurrentLanguage(LocalString::BACK),
        .func = [](){ screen = mainMenu; }
    };

    ScreenButton exitButton = {
        .text = inCurrentLanguage(LocalString::EXIT),
        .func = [](){ screen = selectLanguageScreen; enteredPin = ""; }
    };

    ScreenButton retryButton = {
        .text = inCurrentLanguage(LocalString::BACK),
        .func = [](){ screen = enterPinScreen; enteredPin = ""; }
    };

    if (!transactionStarted) {
        transactionStarted = true;
        futureResponse = std::async(std::launch::async, forwardToSocket, currentTransaction);
    }

    if(futureResponse.valid() && futureResponse.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready) {
        response = futureResponse.get();
        gotResponse = true;
    }

    if (gotResponse) {

        buttons = {emptyButton, emptyButton, emptyButton, backButton};
        if (response.succeeded == ResponseType::SUCCESS) {
            text = complete;
            balance = response.newBalance;
            dotPosition = response.dotPosition;

            if(currentTransaction.type == TransactionType::PIN_CHECK) {
                screen = mainMenu;
                mainMenu();
                return;
            } else if (currentTransaction.type == TransactionType::BALANCE_CHECK) {
                screen = balanceScreen;
                balanceScreen();
                return;
            }

        } else if(response.succeeded == ResponseType::INSUFFICIENT_FUNDS) {
            text = insufficient_funds;
        } else if (response.succeeded == ResponseType::INCORRECT_PIN) {
            text = incorrectPin;
            buttons = {emptyButton, emptyButton, emptyButton, retryButton};
        } else {
            text = genericError;
            buttons = {emptyButton, emptyButton, emptyButton, exitButton};
        }

    }

    drawScreen(text, "", buttons, [](std::string str){}, [](){});
}

std::string withdrawlAmount = "";
void customWithdrawalScreen() {
    drawScreen(inCurrentLanguage(LocalString::WITHDRAWAL_AMOUNT), currencySymbol + withdrawlAmount, {}, [](std::string str){
        if(withdrawlAmount.size() >= 9 && str.length() == 1)
            return;

        if(str.length() == 1) {
            if(withdrawlAmount.empty() && str == "0")
                return;

            withdrawlAmount+=str;
            return;
        }

        if(str == inCurrentLanguage(LocalString::CANCEL)) {
            withdrawlAmount = "";
            screen = mainMenu;
        } else if (str == inCurrentLanguage(LocalString::CLEAR)) {
            withdrawlAmount = "";
        } else if (str == inCurrentLanguage(LocalString::ENTER) && !enteredPin.empty()) {
            AtmCurrency amount = std::stol(withdrawlAmount);
            withdrawlAmount = "";
            processTransaction(TransactionType::WITHDRAWAL, amount);
        }
    }, [](){});
}

std::string depositAmount = "";
void depositScreen() {
    drawScreen(inCurrentLanguage(LocalString::DEPOSIT_AMOUNT), currencySymbol + depositAmount, {}, [](std::string str){
        if(depositAmount.size() >= 9 && str.length() == 1)
            return;

        if(str.length() == 1) {
            if(depositAmount.empty() && str == "0")
                return;

            depositAmount+=str;
            return;
        }

        if(str == inCurrentLanguage(LocalString::CANCEL)) {
            depositAmount = "";
            screen = mainMenu;
        } else if (str == inCurrentLanguage(LocalString::CLEAR)) {
            depositAmount = "";
        } else if (str == inCurrentLanguage(LocalString::ENTER) && !enteredPin.empty()) {
            AtmCurrency amount = std::stol(depositAmount);
            processTransaction(TransactionType::DEPOSIT, amount);

            depositAmount = "";
        }
    }, [](){});
}

void processWithdrawal(AtmCurrency amount) {
    processTransaction(TransactionType::WITHDRAWAL, amount);
}

AtmCurrency getLocalValue(int amount) {
    return (fastPower(10, 4 - dotPosition)) * amount;
}

void withdrawalScreen() {

    ScreenButton five = {
        .text = currencySymbol + std::to_string(getLocalValue(5)),
        .func = [](){ processWithdrawal(getLocalValue(5)); }
    };

    ScreenButton ten = {
        .text = currencySymbol + std::to_string(getLocalValue(10)),
        .func = [](){ processWithdrawal(getLocalValue(10)); }
    };

    ScreenButton twenty = {
        .text = currencySymbol + std::to_string(getLocalValue(20)),
        .func = [](){ processWithdrawal(getLocalValue(20)); }
    };

    ScreenButton fifty = {
        .text = currencySymbol + std::to_string(getLocalValue(50)),
        .func = [](){ processWithdrawal(getLocalValue(50)); }
    };

    ScreenButton hundred = {
        .text = currencySymbol + std::to_string(getLocalValue(100)),
        .func = [](){ processWithdrawal(getLocalValue(100)); }
    };

    ScreenButton hundredFifty = {
        .text = currencySymbol + std::to_string(getLocalValue(150)),
        .func = [](){ processWithdrawal(getLocalValue(150)); }
    };

    ScreenButton custom = {
        .text = inCurrentLanguage(LocalString::CUSTOM_AMOUNT),
        .func = []() { withdrawlAmount = ""; screen = customWithdrawalScreen; }
    };

    ScreenButton backButton = {
        .text = inCurrentLanguage(LocalString::BACK),
        .func = [](){ screen = mainMenu; }
    };

    drawScreen(inCurrentLanguage(LocalString::SELECT_OPTION), "", {five,ten,twenty,backButton,fifty,hundred,hundredFifty,custom}, [](std::string str){}, [](){});
}