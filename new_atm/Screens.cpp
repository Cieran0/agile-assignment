#include "raylib.h"
#include <vector>
#include <functional>
#include <string>
#include <iostream>
#include "Components.hpp"
#include "Screens.hpp"

void drawATMButtons(const std::vector<ScreenButton>& screenButtons) {
    int buttonWidth = (atmWidth*3)/7;
    int buttonHeight = 120;
    int buttonSpacing = 50;

    int startXLeft = atmX + 30;
    int startXRight = atmX + atmWidth - buttonWidth - 30;

    int totalButtonHeight = (4 * buttonHeight) + (3 * buttonSpacing);
    int startY = atmY + ((atmHeight - totalButtonHeight) / 2);

    for (int i = 0; i < 8 && i < screenButtons.size(); i++)
    {
        int j = i;
        int xOffset = 0;
        if(i >= 4) {
            j = i-4;
            xOffset = startXRight-startXLeft;
        }

        float x = (float)startXLeft+xOffset;
        float y = (float)startY + j*((buttonHeight + buttonSpacing));

        if(GuiButton({x, y, (float)buttonWidth, (float)buttonHeight}, screenButtons[i].text, ATM_BUTTON_BG, WHITE)) {
            screenButtons[i].func();
        }
    }
}

void drawScreen(std::string primaryText, std::string secondaryText, std::vector<ScreenButton> screenButtons, std::function<void(std::string)> keypadHandler, std::function<void()> insertCardHandler) {

    DrawRectangle(50, 50, screenWidth, screenHeight, CASING_BACK_COLOR);
    drawKeypadAndCardBackground();
    drawATMScreen(primaryText, secondaryText);
    drawKeypad(keypadHandler);
    drawSideButtons();
    drawCardSlot("Insert Card", insertCardHandler);
    drawATMButtons(screenButtons);
}


void mainMenu() {

    ScreenButton withdrawlButton = {
        .text = "Withdrawal",
        .func = [](){  }
    };

    std::vector<ScreenButton> screenButtons = {withdrawlButton,withdrawlButton,withdrawlButton,withdrawlButton,withdrawlButton,withdrawlButton,withdrawlButton,withdrawlButton};

    drawScreen("Hey!", "", screenButtons, [](std::string str){ if (str == "cancel") screen = insertCardScreen; }, [](){});
}

void enterPinScreen() {
    drawScreen("Enter Pin", enteredPin, std::vector<ScreenButton>(), [](std::string str){ 
        if(enteredPin.length() == 4 && str.length() == 1)
            return;

        if(str.length() == 1) {
            enteredPin+=str;
            return;
        }

        if(str == "cancel") {
            enteredPin = "";
            screen = insertCardScreen;
        } else if (str == "clear") {
            enteredPin = "";
        } else if (str == "enter" && enteredPin.length() == 4) {
            screen = mainMenu;
        }

    }, [](){});
}

void insertCardScreen() {
    drawScreen("Insert Card", "", std::vector<ScreenButton>(), [](std::string str){ std::cout << str << std::endl;}, [](){ screen = enterPinScreen; });
}

