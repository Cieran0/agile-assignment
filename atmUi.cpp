#include <iostream>
#include <string>
#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

class ATM {
private:
    std::string cardNumber;      
    std::string expiryDate;      
    std::string atmID;          
    std::string transactionID;  
    int pin; 
    double withdrawalAmount;     

public:
    
    //constructor initialised to 0
    ATM() : cardNumber(" "), expiryDate(" "), atmID(" "), pin(0), transactionID(" "), withdrawalAmount(0.0) {}

    //setters

    void setCardNumber(const std::string& card) {
        cardNumber = card;
    }

    void setExpiryDate(const std::string& expiry) {
        expiryDate = expiry;
    }

    void setAtmID(const std::string& id) {
        atmID = id;
    }

    void setTransactionID(const std::string& transaction) {
        transactionID = transaction;
    }

    void setPIN(int enteredPin) {
        pin = enteredPin;
    }

    void setWithdrawalAmount(double amount) {
        if (amount >= 0) {
            withdrawalAmount = amount;
        } else {
            std::cerr << "Invalid withdrawal amount!" << std::endl;
        }
    }

    // Getters
    std::string getCardNumber() const {
        return cardNumber;
    }

    std::string getExpiryDate() const {
        return expiryDate;
    }

    std::string getAtmID() const {
        return atmID;
    }

    std::string getTransactionID() const {
        return transactionID;
    }

    int getPIN() const {
        return pin;
    }

    double getWithdrawalAmount() const {
        return withdrawalAmount;
    }

   
    void displayTransactionDetails() const {
        std::cout << "Transaction Details:" << std::endl;
        std::cout << "Card Number: " << cardNumber << std::endl;
        std::cout << "Expiry Date: " << expiryDate << std::endl;
        std::cout << "ATM ID: " << atmID << std::endl;
        std::cout << "Transaction ID: " << transactionID << std::endl;
        std::cout << "Withdrawal Amount: £" << withdrawalAmount << std::endl;
    }
};


int main()
{
    InitWindow(1920, 1080, "raygui - controls test suite");
    ToggleFullscreen();
    SetTargetFPS(60);

    bool showMessageBox = false;

    std::string keyPad[4][4] = {{"1", "2", "3", "cancel"},
                                {"4", "5", "6", "clear"},
                                {"7", "8", "9", "enter"},
                                {" ", "0", " ", " "}};

 

    while (!WindowShouldClose())
    {

        BeginDrawing();

            DrawRectangle(GetScreenHeight()/2 ,GetScreenWidth()/1, 300,100, GREEN);

            ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

            for (auto & button : keyPad)
            {
                if (GuiButton((Rectangle){ 24, 24, 120, 30 }, "#191#Show Message")) showMessageBox = true;

            if (showMessageBox)
            {
                int result = GuiMessageBox((Rectangle){ 85, 70, 250, 100 },
                    "#191#Message Box", "Hi! This is a message!", "Nice;Cool");

                if (result >= 0) showMessageBox = false;
            }
            }
            

            if (GuiButton((Rectangle){ 24, 24, 120, 30 }, "#191#Show Message")) showMessageBox = true;

            if (showMessageBox)
            {
                int result = GuiMessageBox((Rectangle){ 85, 70, 250, 100 },
                    "#191#Message Box", "Hi! This is a message!", "Nice;Cool");

                if (result >= 0) showMessageBox = false;
            }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}

