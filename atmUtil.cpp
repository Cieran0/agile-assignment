#include "atmUtil.h"

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cstring>
#include <random>


#include "net.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include <functional>

vector<string> inputs;
string displayText = "Please enter your PIN:"; // Global display text
string balanceText = "Enter amount for withdrawl";
string withdrawlText = "Enter withdrawal amount";

string pinDisplay = ""; // To show asterisks for PIN
string enteredPIN;
static string withdrawInput = "";
bool validatedPIN = false;
int screenWidth = GetMonitorWidth(0);  
int screenHeight = GetMonitorHeight(0);

static string depositInput = "";

#define ROW_COUNT 4
#define COLUMN_COUNT 4


#define ATM_BACKGROUND CLITERAL(Color){ 45, 49, 66, 255 }     // Dark blue-gray
#define ATM_DISPLAY_BG CLITERAL(Color){ 200, 247, 197, 255 }  // Light green
#define ATM_BUTTON_BG CLITERAL(Color){ 77, 83, 110, 255 }     // Medium blue-gray
#define ATM_BUTTON_HOVER CLITERAL(Color){ 137, 142, 163, 255 }
#define ATM_TEXT CLITERAL(Color){ 33, 37, 41, 255 }           // Dark gray

#define RAYGUI_BUTTON_BORDER_WIDTH   2
#define RAYGUI_BUTTON_TEXT_PADDING   10

void drawWaitingForCard();

void handleDepositInput(const string& buttonPressed);
void drawDepositMenu();

enum Screen {
    WaitingForCard = 0,
    EnterPin = 1,
    MainMenu = 2,
    Withdraw = 3,
    Balance = 4,
    BalanceAmount = 5,
    Deposit = 6,
    PrintBalance = 7
};

enum Screen screen = WaitingForCard;

Account a1 = {cardNumber:"5030153826527268", expiryDate: "06/28"};

string keyPad[4][4] = {{"1", "2", "3", "cancel"},
                    {"4", "5", "6", "clear"},
                    {"7", "8", "9", "enter"},
                    {" ", "0", " ", " "}};

    // Function to update the display text
void setDisplayText(const string& text) {
    displayText = text;
}

/*very basic function to simulate the PIN being checked once entered.
  Will be replaced by switch and network simulator
*/
bool validatePIN(std::string enteredPIN)
{
    if (enteredPIN == a1.PIN)
    {
        cout << "match " << endl;
        screen = MainMenu;
        return true;
    }  
    
    return false;
}

void resetGlobalTextVariables() {
    inputs.clear();
    enteredPIN = "";
    pinDisplay = "";
    displayText = "Please enter your PIN:";
}

// Modified handleInput function
void handleInput(string buttonPressed) {
    if (buttonPressed == "clear") {
        inputs.clear();
        pinDisplay = "";
    }
    
    if (buttonPressed == "cancel") {
        inputs.clear();
        pinDisplay = "";
        setDisplayText("Please enter your PIN:");  
        screen = WaitingForCard;
    }

    if (inputs.size() < 4 && buttonPressed.length() == 1) {
        inputs.push_back(buttonPressed);
        updatePinDisplay();
        setDisplayText("Please enter your PIN:");
    }

    if (buttonPressed == "enter" && inputs.size() == 4) {
        setDisplayText("Processing PIN...");
        // put fake delay or something cba rn
        
        enteredPIN = string(1, inputs[0][0]) + string(1, inputs[1][0]) + string(1, inputs[2][0]) + string(1, inputs[3][0]);
        //validatedPIN = validatePIN(enteredPIN);     
        Response r = forwardToSocket(a1.cardNumber, a1.expiryDate,  "2001", enteredPIN, 0.0);

        if(r.succeeded == 0) {
            screen = MainMenu;
            a1.balance = r.new_balance;
        }
        else{
            setDisplayText("Incorrect pin. try again");
            inputs.clear();
            updatePinDisplay();
            screen = EnterPin;
        }
    }

}

    //return false;// Function to update PIN display
void updatePinDisplay() {
    pinDisplay = string(inputs.size(), '*'); // Convert PIN digits to asterisks
}


// Function to get the display text
string getDisplayText() {
    return displayText;
}

void setupGuiStyle() {
    // Set up button colors
    GuiSetStyle(DEFAULT, BORDER_COLOR_NORMAL, ColorToInt(DARKGRAY));
    GuiSetStyle(DEFAULT, BASE_COLOR_NORMAL, ColorToInt(ATM_BUTTON_BG));
    GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, ColorToInt(WHITE));
    
    // Hover colors
    GuiSetStyle(DEFAULT, BASE_COLOR_FOCUSED, ColorToInt(ATM_BUTTON_HOVER));
    GuiSetStyle(DEFAULT, TEXT_COLOR_FOCUSED, ColorToInt(WHITE));
    
    // Pressed colors
    GuiSetStyle(DEFAULT, BASE_COLOR_PRESSED, ColorToInt(DARKGRAY));
    GuiSetStyle(DEFAULT, TEXT_COLOR_PRESSED, ColorToInt(WHITE));
    
    // Border width
    GuiSetStyle(DEFAULT, BORDER_WIDTH, RAYGUI_BUTTON_BORDER_WIDTH);
    
    // Text alignment
    GuiSetStyle(DEFAULT, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
    
    // Button padding
    GuiSetStyle(DEFAULT, TEXT_PADDING, RAYGUI_BUTTON_TEXT_PADDING);
    
    // Button rounding
    GuiSetStyle(DEFAULT, BORDER_WIDTH, 2);
    GuiSetStyle(BUTTON, BORDER_WIDTH, 2);
    GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL, ColorToInt(DARKGRAY));
    GuiSetStyle(BUTTON, BORDER_COLOR_FOCUSED, ColorToInt(LIGHTGRAY));
    GuiSetStyle(BUTTON, BORDER_COLOR_PRESSED, ColorToInt(WHITE));
}

void displayTransactionChoices()
{
    // 1) Draw the ATM frame
    int atmWidth  = 750;
    int atmHeight = 900;
    int atmX = screenWidth / 2.5;
    int atmY = screenHeight / 5;

    DrawRectangle(atmX, atmY, atmWidth, atmHeight, ATM_BACKGROUND);
    DrawRectangleLines(atmX, atmY, atmWidth, atmHeight, DARKGRAY);

    DrawRectangle(atmX, atmY, atmWidth, 200, DARKGRAY);
    DrawRectangle(atmX + 10, atmY + 10, atmWidth - 20, 180, ATM_DISPLAY_BG);

    for (int i = 0; i < 4; i++) {
        DrawRectangle(
            atmX + 203,           
            atmY + 253 + i*80, 
            350, 
            60, 
            DARKGRAY
        );
    }

    Button buttons[] = {
        {{(float)(atmX + 200), (float)(atmY + 250), 350, 60}, "Balance Inquiry"},
        {{(float)(atmX + 200), (float)(atmY + 330), 350, 60}, "Cash Withdrawal"},
        {{(float)(atmX + 200), (float)(atmY + 410), 350, 60}, "Deposit"},
        {{(float)(atmX + 200), (float)(atmY + 490), 350, 60}, "Exit"}
    };

    int buttonCount = sizeof(buttons)/sizeof(buttons[0]);

    for (int i = 0; i < buttonCount; i++)
    {
        Rectangle btnRect = buttons[i].bounds;

        if (CheckCollisionPointRec(GetMousePosition(), btnRect)) {
            DrawRectangleRec(
                (Rectangle){btnRect.x - 2, btnRect.y - 2, btnRect.width + 4, btnRect.height + 4}, 
                Fade(LIGHTGRAY, 0.3f)
            );
        }
        
        if (GuiButton(btnRect, buttons[i].text))
        {
            switch (i) {
                case 0:
                    screen = Balance;
                    cout << "balance" << endl;
                    break;
                case 1:
                    screen = Withdraw;
                    cout << "withdraw" << endl;
                    break;
                case 2:
                    screen = Deposit;
                    cout << "deposit" << endl;
                    break;
                case 3:
                    screen = WaitingForCard;
                    resetGlobalTextVariables();
                    cout << "exit" << endl;
                    break;
            }
        }
    }
}

void drawKeypad(int startX, int startY, const std::function<void(const string&)>& handleInput) {
    int buttonWidth = 80;
    int buttonHeight = 60;
    int spacing = 20;

    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            string keypadButtonText = keyPad[row][col];
            if (keypadButtonText != " ") { 
                float x = startX + (col * (buttonWidth + spacing));
                float y = startY + (row * (buttonHeight + spacing));
                
                Rectangle btnRect = {
                    x,
                    y,
                    static_cast<float>(buttonWidth),
                    static_cast<float>(buttonHeight)
                };


                if (keypadButtonText == "cancel") {
                    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(RED));
                } else if (keypadButtonText == "clear") {
                    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(YELLOW));
                } else if (keypadButtonText == "enter") {
                    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(GREEN));
                } else {
                    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(GRAY));
                }

                if (GuiButton(btnRect, keyPad[row][col].c_str())) {
                    handleInput(keyPad[row][col]);
                }
            }
        }
    }
}

void atmLayout() 
{
    int atmWidth  = 750;
    int atmHeight = 900;
    int atmX = screenWidth / 2.5f;
    int atmY = screenHeight / 5;

    DrawRectangle(atmX, atmY, atmWidth, atmHeight, ATM_BACKGROUND);
    DrawRectangleLines(atmX, atmY, atmWidth, atmHeight, DARKGRAY);

    DrawRectangle(atmX, atmY, atmWidth, 200, DARKGRAY);
    DrawRectangle(atmX + 10, atmY + 10, atmWidth - 20, 180, ATM_DISPLAY_BG);
    DrawText(displayText.c_str(), atmX + 20, atmY + 20, 20, ATM_TEXT);
    if (!pinDisplay.empty()) {
        DrawText(pinDisplay.c_str(), atmX + 20, atmY + 60, 30, ATM_TEXT);
    }

    int keypadWidth  = 300;
    int keypadHeight = 300;

    int keypadX = (atmX + atmWidth/2) - (keypadWidth/2);
    int keypadY = (atmY + atmHeight/2) - (keypadHeight/2);

    drawKeypad(keypadX, keypadY, handleInput);

    DrawRectangle(atmX + 20, atmY + 250, 200, 10, DARKGRAY);
    DrawText("INSERTED CARD", atmX + 20, atmY + 230, 15, LIGHTGRAY);
}


void screenInit() {
    InitWindow(0, 0, "raygui - NCR ATM");

    setupGuiStyle();
    ToggleFullscreen();
    SetTargetFPS(60);
    screenWidth = GetMonitorWidth(0);  
    screenHeight = GetMonitorHeight(0);
    GuiSetStyle(DEFAULT,TEXT_SIZE ,screenHeight/100);
}

void screenManager(){

    ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

    switch (screen) {
        case WaitingForCard:
            drawWaitingForCard();
            break;
        case EnterPin:
            atmLayout();
            break;
        case MainMenu:
            displayTransactionChoices();
            break;
        case Withdraw:
            drawWithdrawMenu();
            break;
        case Balance:
            drawBalanceChoices();
            break;
        case BalanceAmount:
            viewBalance();
            break;
        case Deposit:
            drawDepositMenu();
            break;
        case PrintBalance:
            printBalance();
            break;
    }
}

void drawBalanceChoices() {
    int atmWidth  = 750;
    int atmHeight = 900;
    int atmX = screenWidth / 2.5f;
    int atmY = screenHeight / 5;

    DrawRectangle(atmX, atmY, atmWidth, atmHeight, ATM_BACKGROUND);
    DrawRectangleLines(atmX, atmY, atmWidth, atmHeight, DARKGRAY);

    DrawRectangle(atmX, atmY, atmWidth, 200, DARKGRAY);
    DrawRectangle(atmX + 10, atmY + 10, atmWidth - 20, 180, ATM_DISPLAY_BG);
    DrawText("Select balance option", atmX + 20, atmY + 20, 20, ATM_TEXT);

    Button buttons[] = {
        {{static_cast<float>(atmX + 200), static_cast<float>(atmY + 300), 350.0f, 60.0f}, "View Balance"},
        {{static_cast<float>(atmX + 200), static_cast<float>(atmY + 380), 350.0f, 60.0f}, "Print Balance"},
        {{static_cast<float>(atmX + 200), static_cast<float>(atmY + 460), 350.0f, 60.0f}, "Back to Main Menu"}
    };    

    int buttonCount = sizeof(buttons) / sizeof(buttons[0]);

    for (size_t i = 0; i < buttonCount; i++) {
        if (GuiButton(buttons[i].bounds, buttons[i].text)) {
            switch (i) {
                case 0:
                    screen = BalanceAmount;
                    cout << "View Balance button pressed" << endl;
                    break;
                case 1:
                    screen = PrintBalance;
                    cout << "Print Balance button pressed" << endl;
                    break;
                case 2:
                    screen = MainMenu;
                    cout << "Back to main menu" << endl;
                    break;
            }
        }
    }

    // Draw card slot
    DrawRectangle(atmX + 20, atmY + 250, 200, 10, DARKGRAY);
    DrawText("INSERTED CARD", atmX + 20, atmY + 230, 15, LIGHTGRAY);
}



void viewBalance() {
    int atmWidth  = 750;
    int atmHeight = 900;
    int atmX = screenWidth / 2.5f;
    int atmY = screenHeight / 5;

    DrawRectangle(atmX, atmY, atmWidth, atmHeight, ATM_BACKGROUND);
    DrawRectangleLines(atmX, atmY, atmWidth, atmHeight, DARKGRAY);

    DrawRectangle(atmX, atmY, atmWidth, 200, DARKGRAY);
    DrawRectangle(atmX + 10, atmY + 10, atmWidth - 20, 180, ATM_DISPLAY_BG);
    DrawText("Your Current Balance:", atmX + 250, atmY + 100, 20, ATM_TEXT);
    DrawText(("£" + to_string(a1.balance)).c_str(), atmX + 250, atmY + 120, 40, ATM_TEXT);
    
    Rectangle backBtn = {
        static_cast<float>(atmX + 200), 
        static_cast<float>(atmY + 450), 
        350.0f, 
        60.0f
    };
    if (GuiButton(backBtn, "Back to Main Menu")) {
        screen = MainMenu;
    }
}

void printFunction(string balance){
    ofstream balancePrint;
    balancePrint.open("printedBalance.txt");
    balancePrint << "account balance" << balance << endl;
    balancePrint.close();
}

void processingScreen(string messageToPrint){
    static int counter = 0;
    bool printingComplete = false;

    if (counter < 60*5)
    {
        printingComplete = true;
        counter++;
        DrawText("Processing...", 470, 170, 20, ATM_TEXT);
    } else {
        DrawText("Printing successful. ", 200, 500, 40, BLACK);
        if (GuiButton({200, 600, 200, 50}, "Back")) {
            screen = MainMenu; 
        }
    }
}

void printBalance() {
   int atmWidth  = 750;
   int atmHeight = 900;
   int atmX = screenWidth / 2.5f;
   int atmY = screenHeight / 5;

   DrawRectangle(atmX, atmY, atmWidth, atmHeight, ATM_BACKGROUND);
   DrawRectangleLines(atmX, atmY, atmWidth, atmHeight, DARKGRAY);

   DrawRectangle(atmX, atmY, atmWidth, 200, DARKGRAY);
   DrawRectangle(atmX + 10, atmY + 10, atmWidth - 20, 180, ATM_DISPLAY_BG);
   DrawText("Printing Balance", atmX + 275, atmY + 150, 20, ATM_TEXT);
   
   static int counter = 0;
   if (counter < 60 * 5) { 
       counter++;
       DrawText("Processing....", atmX + 20, atmY + 20, 20, ATM_TEXT);
       printFunction(to_string(a1.balance));
   } else {
       DrawText("Please take your receipt", atmX + 20, atmY + 20, 20, ATM_TEXT);
       
       Rectangle backBtn = {
           static_cast<float>(atmX + 200), 
           static_cast<float>(atmY + 450), 
           350.0f, 
           60.0f
       };
       if (GuiButton(backBtn, "Back to Main Menu")) {
           screen = MainMenu;
           counter = 0;  
       }
   }
   
   // Draw card slot
   DrawRectangle(atmX + 20, atmY + 250, 200, 10, DARKGRAY);
    DrawText("INSERTED CARD", atmX + 20, atmY + 230, 15, LIGHTGRAY);
}

void handleWithdrawInput(const string& buttonPressed) {
    if (buttonPressed == "clear") {
        withdrawInput.clear();
        return;
    }

    if (buttonPressed == "cancel") {
        withdrawInput.clear();
        screen = MainMenu;
        return;
    }

    if (buttonPressed.size() == 1 && isdigit(buttonPressed[0])) {
        withdrawInput.push_back(buttonPressed[0]);
        withdrawlText = "Please enter amount";
        return;
    }

    if (buttonPressed == "enter") {
        if (!withdrawInput.empty()) {
            double amount = std::stof(withdrawInput);
            if (amount > a1.balance)
            {
                withdrawlText = "Insufficient funds";
                withdrawInput.clear();
            }
            else{
                Response r = forwardToSocket(a1.cardNumber, a1.expiryDate, "2001", enteredPIN, amount);
                if(r.succeeded == 0) {
                a1.balance = r.new_balance;
                withdrawInput.clear();
                screen = MainMenu;
            }
          }      
        }
    }
}

void drawWithdrawMenu() {
    int atmWidth  = 750;
    int atmHeight = 900;
    int atmX = screenWidth / 2.5f;
    int atmY = screenHeight / 5;

    DrawRectangle(atmX, atmY, atmWidth, atmHeight, ATM_BACKGROUND);
    DrawRectangleLines(atmX, atmY, atmWidth, atmHeight, DARKGRAY);

    DrawRectangle(atmX, atmY, atmWidth, 200, DARKGRAY);
    DrawRectangle(atmX + 10, atmY + 10, atmWidth - 20, 180, ATM_DISPLAY_BG);
    DrawText(withdrawlText.c_str(), atmX + 20, atmY + 20, 20, ATM_TEXT);

    if (!withdrawInput.empty()) {
        DrawText(("£" + withdrawInput).c_str(), atmX + 20, atmY + 40, 30, ATM_TEXT);
    }

    int keypadWidth  = 300;
    int keypadHeight = 300;

    int keypadX = (atmX + atmWidth/2) - (keypadWidth/2);
    int keypadY = (atmY + atmHeight/2) - (keypadHeight/2);

    drawKeypad(keypadX, keypadY, handleWithdrawInput);
}

void drawWaitingForCard() {
    int atmWidth  = 750;
    int atmHeight = 900;
    int atmX = (screenWidth - atmWidth) / 2;  // Updated
    int atmY = (screenHeight - atmHeight) / 2; // Updated
    
    DrawRectangle(atmX, atmY, atmWidth, atmHeight, ATM_BACKGROUND);
    DrawRectangleLines(atmX, atmY, atmWidth, atmHeight, DARKGRAY);
    
    int screenX = atmX + 50;
    int screenY = atmY + 50;
    int screenW = atmWidth - 100;
    int screenH = 200;
    DrawRectangle(screenX, screenY, screenW, screenH, ATM_DISPLAY_BG);
    
    DrawText("Please Insert Card", screenX + 70, screenY + 70, 20, ATM_TEXT);
    
    int slotWidth  = 200;
    int slotHeight = 10;
    int slotX = atmX + (atmWidth - slotWidth) / 2;
    int slotY = atmY + atmHeight - 80; 
    DrawRectangle(slotX, slotY, slotWidth, slotHeight, DARKGRAY);

    Rectangle cardBtn = {
        (float)slotX,
        (float)(slotY - 40),
        (float)slotWidth,
        30
    };
    if (GuiButton(cardBtn, "INSERT CARD")) {
        screen = EnterPin;
        resetGlobalTextVariables();
    }
}



void handleDepositInput(const string& buttonPressed) {
    if (buttonPressed == "clear") {
        depositInput.clear();
        return;
    }

    if (buttonPressed == "cancel") {
        depositInput.clear();
        screen = MainMenu;
        return;
    }

    if (buttonPressed.size() == 1 && isdigit(buttonPressed[0])) {
        depositInput.push_back(buttonPressed[0]);
        return;
    }

    if (buttonPressed == "enter") {
        if (!depositInput.empty()) {
            float amount = std::stof(depositInput);
            a1.balance += amount;
            cout << "Deposited: £" << amount << endl;
        }
        depositInput.clear();
        screen = MainMenu;
    }
}

void drawDepositMenu() {
    int atmWidth  = 750;
    int atmHeight = 900;
    int atmX = screenWidth / 2.5f;
    int atmY = screenHeight / 5;

    DrawRectangle(atmX, atmY, atmWidth, atmHeight, ATM_BACKGROUND);
    DrawRectangleLines(atmX, atmY, atmWidth, atmHeight, DARKGRAY);

    DrawRectangle(atmX, atmY, atmWidth, 200, DARKGRAY);
    DrawRectangle(atmX + 10, atmY + 10, atmWidth - 20, 180, ATM_DISPLAY_BG);
    DrawText("Enter Deposit Amount:", atmX + 20, atmY + 20, 20, ATM_TEXT);
    
    if (!depositInput.empty()) {
        DrawText(("£" + depositInput).c_str(), atmX + 20, atmY + 40, 30, ATM_TEXT);
    }

    int keypadWidth  = 300;
    int keypadHeight = 300;

    int keypadX = (atmX + atmWidth/2) - (keypadWidth/2);
    int keypadY = (atmY + atmHeight/2) - (keypadHeight/2);

    drawKeypad(keypadX, keypadY, handleDepositInput);

    DrawRectangle(atmX + 20, atmY + 250, 200, 10, DARKGRAY);
    DrawText("INSERT CASH HERE", atmX + 20, atmY + 230, 15, LIGHTGRAY);
}
void initializeATM() {
    setupGuiStyle();
}

