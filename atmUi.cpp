#include <iostream>
#include <string>
#include "raylib.h"

#include <sstream>

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include <vector>

using namespace std;

vector<string> inputs;

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

bool handleInput(string buttonPressed){

     if (inputs.size() < 4 && buttonPressed.length() == 1)
    {
        inputs.push_back(buttonPressed);
        return true;
    }

    cout << " too many inputs" << endl;

    return false;
}

int main()
{
    InitWindow(1920, 1080, "raygui - controls test suite");
    ToggleFullscreen();
    SetTargetFPS(60);

    bool showMessageBox = false;

    string keyPad[4][4] = {{"1", "2", "3", "cancel"},
                                {"4", "5", "6", "clear"},
                                {"7", "8", "9", "enter"},
                                {" ", "0", " ", " "}};

 

    while (!WindowShouldClose())
    {

        BeginDrawing();

            DrawText("hello",0,0, 30, BLACK);


            DrawRectangle(500 ,200, 350,150, GREEN);

            ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

            int offsetX = 0;
            int offsetY = 0;
            for (int row = 0; row < 4; row++)
            { 
                for(int col = 0; col < 4; col ++){
                    offsetX += 100;
                    if (GuiButton((Rectangle){600 + offsetX, 400 + offsetY, 80, 40},keyPad[row][col].c_str()))
                    {
                        DrawText("*",0,0, 30, BLACK);
                        if(handleInput(keyPad[row][col]) == false){
                        }
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

