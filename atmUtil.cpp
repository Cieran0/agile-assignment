#include "atmUtil.h"


vector<string> inputs;
string displayText = "Please enter your PIN:"; // Global display text

string pinDisplay = ""; // To show asterisks for PIN
string enteredPIN;
bool validatedPIN = false;

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
    string storedPIN[5] = {"1234", "1243", "1345", "1678"};

    for (auto &pins : storedPIN)
    {
        if (enteredPIN == pins)
        {
            cout << "match " << endl;
            return true;
        }  
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

        typedef struct {
            Rectangle bounds; // Button position and size
            const char *text; // Button label
        } Button;

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
                    cout << "balance" << endl;
                    break;
                case 1:
                    cout << "withdraw" << endl;
                    break;
                case 2:
                    cout << "deposit" << endl;
                    break;
                }
            
            }

        }
}

void displayTerminal(){
    if (validatedPIN == false)
        {
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
        else{
            displayTransactionChoices();
        }
}

