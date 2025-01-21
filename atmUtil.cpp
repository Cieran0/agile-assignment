#include "atmUtil.h"


vector<string> inputs;
string displayText = "Please enter your PIN:"; // Global display text
string balanceText = "Enter amount for withdrawl";

string pinDisplay = ""; // To show asterisks for PIN
string enteredPIN;
bool validatedPIN = false;


enum Screen {
    EnterPin = 1,
    MainMenu = 2,
    Withdraw = 3,
    Balance = 4,
    BalanceAmount = 5,
    Deposit = 6
};

enum Screen screen = EnterPin;

Account a1 = {PIN: "1234", balance: 1000};

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
bool validatePIN(std::string enteredPIN){
        
        if (enteredPIN == a1.PIN)
        {
            cout << "match " << endl;
            screen = MainMenu;
            return true;
        }  
      
    return false;
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
    }

    if (inputs.size() < 4 && buttonPressed.length() == 1) {
        inputs.push_back(buttonPressed);
        updatePinDisplay();
    }

    if (buttonPressed == "enter" && inputs.size() == 4) {
        setDisplayText("Processing PIN...");
        // put fake delay or something cba rn
        setDisplayText("Pin of " + string(1, inputs[0][0]) + string(1, inputs[1][0]) + string(1, inputs[2][0]) + string(1, inputs[3][0]));
        enteredPIN = string(1, inputs[0][0]) + string(1, inputs[1][0]) + string(1, inputs[2][0]) + string(1, inputs[3][0]);
        validatedPIN = validatePIN(enteredPIN);     
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

void displayTransactionChoices(){
        Button buttons[] = {
                {{100, 100, 200, 50}, "Balance Inquiry"},
                {{100, 200, 200, 50}, "Cash Withdrawal"},
                {{100, 300, 200, 50}, "Deposit"}
        };

        int buttonCount = sizeof(buttons)/ sizeof(buttons[0]);

        for (size_t i = 0; i < buttonCount; i++)
        {
            if (GuiButton(buttons[i].bounds, buttons[i].text))
            {
                
                switch (i)
                {
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
                }
            
            }
        }
}

void drawMainMenu() {
        DrawRectangle(500 ,200, 350,150, GREEN);

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
}

void displayTerminal(){

    switch (screen) {
        case 1:
            drawMainMenu();
            cout << "on enter pin menu" << endl;
            break;
        case 2:
            displayTransactionChoices();
            cout << "on main menu" << endl;
            break;
        case 3:
            drawWithdrawMenu();
            cout << "on withdraw screen" << endl;
            break;
        case 4:
            drawBalance();
            break;
        case 5:
            presentBalance();
            break;
    }
}

void drawBalance(){
    ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
    
    //DrawText();

    typedef struct {
            Rectangle bounds; // Button position and size
            const char *text; // Button label
            } Button;

    Button buttons[] = {
                {{100, 100, 200, 50}, "View Balance"},
                {{100, 200, 200, 50}, "Print Balance"}
        };    

    int buttonCount = sizeof(buttons)/ sizeof(buttons[0]);

        for (int i = 0; i < buttonCount; i++) {
        if (GuiButton(buttons[i].bounds, buttons[i].text)) {
            
            switch (i) {
                case 0:
                    cout << "View Balance button pressed" << endl;
                    screen = BalanceAmount;
                    break;
                case 1:
                    cout << "Print Balance button pressed" << endl;
                    break;
                default:
                    break;
            }
        }
    }

    
}

void presentBalance() {
    ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

    DrawText(to_string(a1.balance).c_str(), 200, 500, 40, BLACK);

    if (GuiButton({200, 600, 200, 50}, "Back")) {
        screen = MainMenu; // Change screen only when "Back" is clicked
    }

}


void drawWithdrawMenu() {
    ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

    DrawRectangle(500, 200, 350, 150, GREEN);

    DrawText("Enter Withdrawal Amount:", 510, 210, 20, BLACK);

    int offsetX = 0;
    int offsetY = 0;
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            offsetX += 100;
            Rectangle btnRect = {
                static_cast<float>(600 + offsetX),
                static_cast<float>(400 + offsetY),
                80.0f,
                40.0f
            };
            if (GuiButton(btnRect, keyPad[row][col].c_str())) {
                handleInput(keyPad[row][col]);
            }
        }
        offsetY += 50;
        offsetX = 0;
    }

    if (!pinDisplay.empty()) {
        DrawText(pinDisplay.c_str(), 510, 300, 30, BLACK);
    }
}
