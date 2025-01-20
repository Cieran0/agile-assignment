#include <iostream>
#include <string>
#include "raylib.h"

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

int main() {
    ATM transaction1;

    const int screen_width = 1400;
    const int screen_height = 1400;
    InitWindow(screen_width, screen_height, "I dunno what this game is yet");
    ToggleFullscreen();
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(WHITE);
        DrawCircle(screen_width/2, screen_height/2, 30, RED);
        EndDrawing();

    }

    CloseWindow();
    return 0;
}
