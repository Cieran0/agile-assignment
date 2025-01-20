#include <iostream>
#include <string>

#include <sstream>

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include <vector>

using namespace std;

vector<string> inputs;
string displayText = "Please enter your PIN:"; // Global display text
string pinDisplay = ""; // To show asterisks for PIN

// Function to update the display text
void setDisplayText(const string& text) {
    displayText = text;
}

// Function to get the display text
string getDisplayText() {
    return displayText;
}

// Function to update PIN display
void updatePinDisplay() {
    pinDisplay = string(inputs.size(), '*'); // Convert PIN digits to asterisks
}

// Modified handleInput function
bool handleInput(string buttonPressed) {
    if (buttonPressed == "clear") {
        inputs.clear();
        pinDisplay = "";
        return true;
    }
    
    if (buttonPressed == "cancel") {
        inputs.clear();
        pinDisplay = "";
        setDisplayText("Please enter your PIN:");
        return true;
    }

    if (inputs.size() < 4 && buttonPressed.length() == 1) {
        inputs.push_back(buttonPressed);
        updatePinDisplay();
        return true;
    }

    if (buttonPressed == "enter" && inputs.size() == 4) {
        setDisplayText("Processing PIN...");
        // put fake delay or something cba rn
        setDisplayText("Pin of " + string(1, inputs[0][0]) + string(1, inputs[1][0]) + string(1, inputs[2][0]) + string(1, inputs[3][0]));
        return true;
    }

    return false;
}

class ATM {
private:
    string cardNumber;      
    string expiryDate;      
    string atmID;          
    string transactionID;  
    int pin; 
    double withdrawalAmount;     

public:
    
    //constructor initialised to 0
    ATM() : cardNumber(" "), expiryDate(" "), atmID(" "), pin(0), transactionID(" "), withdrawalAmount(0.0) {}

    //setters

    void setCardNumber(const string& card) {
        cardNumber = card;
    }

    void setExpiryDate(const string& expiry) {
        expiryDate = expiry;
    }

    void setAtmID(const string& id) {
        atmID = id;
    }

    void setTransactionID(const string& transaction) {
        transactionID = transaction;
    }

    void setPIN(int enteredPin) {
        pin = enteredPin;
    }

    void setWithdrawalAmount(double amount) {
        if (amount >= 0) {
            withdrawalAmount = amount;
        } else {
            cerr << "Invalid withdrawal amount!" << endl;
        }
    }

    // Getters
    string getCardNumber() const {
        return cardNumber;
    }

    string getExpiryDate() const {
        return expiryDate;
    }

    string getAtmID() const {
        return atmID;
    }

    string getTransactionID() const {
        return transactionID;
    }

    int getPIN() const {
        return pin;
    }

    double getWithdrawalAmount() const {
        return withdrawalAmount;
    }

   
    void displayTransactionDetails() const {
        cout << "Transaction Details:" << endl;
        cout << "Card Number: " << cardNumber << endl;
        cout << "Expiry Date: " << expiryDate << endl;
        cout << "ATM ID: " << atmID << endl;
        cout << "Transaction ID: " << transactionID << endl;
        cout << "Withdrawal Amount: £" << withdrawalAmount << endl;
    }
};

int main()
{
    int screenWidth = GetMonitorWidth(0);  
    int screenHeight = GetMonitorHeight(0); 
    InitWindow(screenWidth, screenHeight, "raygui - NCR ATM");
    ToggleFullscreen();
    SetTargetFPS(60);

    string keyPad[4][4] = {{"1", "2", "3", "cancel"},
                          {"4", "5", "6", "clear"},
                          {"7", "8", "9", "enter"},
                          {" ", "0", " ", " "}};

    while (!WindowShouldClose())
    {
        BeginDrawing();
        // wtf happend to the indenting??
            DrawRectangle(500 ,200, 350,150, GREEN);
            ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

            // Draw the display text
            DrawText(displayText.c_str(), 510, 210, 20, BLACK);
            
            // Draw the PIN display
            if (!pinDisplay.empty()) {
                DrawText(pinDisplay.c_str(), 510, 240, 30, BLACK);
            }

            int offsetX = 0;
            int offsetY = 0;
            for (int row = 0; row < 4; row++)
            { 
                for(int col = 0; col < 4; col ++){
                    offsetX += 100;
                    // wouldnt compile on mac without being casted to a float...
                    Rectangle btnRect = {
                        static_cast<float>(600 + offsetX),
                        static_cast<float>(400 + offsetY),
                        80.0f,
                        40.0f
                    };
                    if (GuiButton(btnRect, keyPad[row][col].c_str()))
                    {
                        handleInput(keyPad[row][col]);
                    }                
                }

                offsetY += 50;
                offsetX = 0;
            }
            
        EndDrawing();
    }

    CloseWindow();
    return 0;
}

