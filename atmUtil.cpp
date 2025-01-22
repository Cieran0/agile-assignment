#include "atmUtil.h"

vector<string> inputs;
string displayText = "Please enter your PIN:"; // Global display text
string balanceText = "Enter amount for withdrawl";

string pinDisplay = ""; // To show asterisks for PIN
string enteredPIN;
static string withdrawInput = "";
bool validatedPIN = false;

static string depositInput = "";

#define ROW_COUNT 4
#define COLUMN_COUNT 4

#define ATM_BACKGROUND CLITERAL(Color){ 45, 49, 66, 255 }     // Dark blue-gray
#define ATM_DISPLAY_BG CLITERAL(Color){ 200, 247, 197, 255 }  // Light green
#define ATM_BUTTON_BG CLITERAL(Color){ 77, 83, 110, 255 }     // Medium blue-gray
#define ATM_BUTTON_HOVER CLITERAL(Color){ 137, 142, 163, 255 }
#define ATM_TEXT CLITERAL(Color){ 33, 37, 41, 255 }           // Dark gray
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

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

void displayTransactionChoices(){
    // Draw ATM frame
    DrawRectangle(400, 100, 550, 600, ATM_BACKGROUND);
    DrawRectangleLines(400, 100, 550, 600, DARKGRAY);
    
    // Draw screen area
    DrawRectangle(450, 150, 450, 500, ATM_DISPLAY_BG);
    DrawText("Please select your transaction", 500, 170, 20, ATM_TEXT);

    // Add shadow effect to buttons
    for (int i = 0; i < 4; i++) {
        DrawRectangle(503, 253 + (i * 80), 350, 60, DARKGRAY);  // Shadow
    }

    Button buttons[] = {
        {{500, 250, 350, 60}, "Balance Inquiry"},
        {{500, 330, 350, 60}, "Cash Withdrawal"},
        {{500, 410, 350, 60}, "Deposit"},
        {{500, 490, 350, 60}, "Exit"}
    };

    int buttonCount = sizeof(buttons) / sizeof(buttons[0]);

    for (size_t i = 0; i < buttonCount; i++) {
        // Draw button highlight/glow effect when hovered
        Rectangle btnRect = buttons[i].bounds;
        if (CheckCollisionPointRec(GetMousePosition(), btnRect)) {
            DrawRectangleRec((Rectangle){btnRect.x - 2, btnRect.y - 2, btnRect.width + 4, btnRect.height + 4}, 
                            Fade(LIGHTGRAY, 0.3f));
        }
        
        if (GuiButton(buttons[i].bounds, buttons[i].text)) {
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

// Helper function to draw keypad using GuiButton - add this near other helper functions
void drawKeypad(int startX, int startY, const std::function<void(const string&)>& handleInput) {
    int buttonWidth = 80;
    int buttonHeight = 60;
    int spacing = 20;

    // Draw the buttons
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            if (keyPad[row][col] != " ") {  // Skip empty spaces in keypad
                float x = startX + (col * (buttonWidth + spacing));
                float y = startY + (row * (buttonHeight + spacing));
                
                Rectangle btnRect = {
                    x,
                    y,
                    static_cast<float>(buttonWidth),
                    static_cast<float>(buttonHeight)
                };

                if (GuiButton(btnRect, keyPad[row][col].c_str())) {
                    handleInput(keyPad[row][col]);
                }
            }
        }
    }
}

void atmLayout() {
    // Draw ATM outer casing
    DrawRectangle(400, 100, 550, 650, ATM_BACKGROUND);
    DrawRectangleLines(400, 100, 550, 650, DARKGRAY);
    
    // Draw screen bezel
    DrawRectangle(450, 150, 450, 200, DARKGRAY);
    DrawRectangle(460, 160, 430, 180, ATM_DISPLAY_BG);
    
    DrawText(displayText.c_str(), 470, 170, 20, ATM_TEXT);
    
    if (!pinDisplay.empty()) {
        DrawText(pinDisplay.c_str(), 470, 200, 30, ATM_TEXT);
    }

    // Draw keypad using GuiButton
    drawKeypad(500, 400, handleInput);

    // Draw card slot
    DrawRectangle(450, 370, 200, 10, DARKGRAY);
    DrawText("INSERTED CARD", 480, 355, 15, LIGHTGRAY);
}

void screenManager(){
    switch (screen) {
        case WaitingForCard:
            drawWaitingForCard();
            break;
        case EnterPin:
            atmLayout();
            cout << "on enter pin menu" << endl;
            break;
        case MainMenu:
            displayTransactionChoices();
            cout << "on main menu" << endl;
            break;
        case Withdraw:
            drawWithdrawMenu();
            cout << "on withdraw screen" << endl;
            break;
        case Balance:
            drawBalanceChoices();
            cout << "on the balance choice screen " << endl;
            break;
        case BalanceAmount:
            viewBalance();
            cout << "view balance screen " << endl;
            break;
        case Deposit:
            drawDepositMenu();
            cout << "on deposit screen" << endl;
            break;
        case PrintBalance:
            cout << "print balance screen " << endl;
            printBalance();
            break;
    }
}

void drawBalanceChoices() {
    // Draw ATM frame
    DrawRectangle(400, 100, 550, 600, ATM_BACKGROUND);
    DrawRectangleLines(400, 100, 550, 600, DARKGRAY);
    
    // Draw screen area
    DrawRectangle(450, 150, 450, 200, DARKGRAY);
    DrawRectangle(460, 160, 430, 180, ATM_DISPLAY_BG);
    DrawText("Select Balance Option", 500, 170, 20, ATM_TEXT);

    Button buttons[] = {
        {{500, 400, 350, 60}, "View Balance"},
        {{500, 480, 350, 60}, "Print Balance"},
        {{500, 560, 350, 60}, "Back to Main Menu"}
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
    DrawRectangle(450, 370, 200, 10, DARKGRAY);
    DrawText("CARD INSERTED", 480, 355, 15, LIGHTGRAY);
}

void viewBalance() {
    // Draw ATM frame
    DrawRectangle(400, 100, 550, 600, ATM_BACKGROUND);
    DrawRectangleLines(400, 100, 550, 600, DARKGRAY);
    
    // Draw screen area
    DrawRectangle(450, 150, 450, 400, ATM_DISPLAY_BG);
    
    DrawText("Your Current Balance:", 470, 170, 20, ATM_TEXT);
    DrawText(("£" + to_string(a1.balance)).c_str(), 470, 220, 40, ATM_TEXT);
    
    // Back button using GuiButton
    Rectangle backBtn = {500, 450, 350, 60};
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
        DrawText("Processing... ", 200, 500, 40, BLACK);
    } else {
        DrawText("Printing successful. ", 200, 500, 40, BLACK);
        if (GuiButton({200, 600, 200, 50}, "Back")) {
            screen = MainMenu; 
        }
    }
}

void printBalance() {
    // Draw ATM frame
    DrawRectangle(400, 100, 550, 600, ATM_BACKGROUND);
    DrawRectangleLines(400, 100, 550, 600, DARKGRAY);
    
    // Draw screen area
    DrawRectangle(450, 150, 450, 200, DARKGRAY);
    DrawRectangle(460, 160, 430, 180, ATM_DISPLAY_BG);
    DrawText("Printing Balance", 500, 170, 20, ATM_TEXT);
    
    static int counter = 0;
    if (counter < 60 * 3) {  // 3 seconds
        counter++;
        DrawText("Processing...", 500, 250, 30, ATM_TEXT);
        printFunction(to_string(a1.balance));
    } else {
        DrawText("Print Complete", 500, 250, 30, ATM_TEXT);
        DrawText("Please take your receipt", 500, 300, 20, ATM_TEXT);
        
        // Back button using GuiButton
        Rectangle backBtn = {500, 450, 350, 60};
        if (GuiButton(backBtn, "Back to Main Menu")) {
            screen = MainMenu;
            counter = 0;  // Reset counter for next time
        }
    }
    
    // Draw card slot
    DrawRectangle(450, 370, 200, 10, DARKGRAY);
    DrawText("CARD INSERTED", 480, 355, 15, LIGHTGRAY);
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
        return;
    }

    if (buttonPressed == "enter") {
        if (!withdrawInput.empty()) {
            float amount = std::stof(withdrawInput);
            if (amount <= a1.balance) {
                a1.balance -= amount;
            }
            else {
                cout << "not enough money :()" << endl;
            }
        }
        withdrawInput.clear();
        screen = MainMenu;
    }
}

void drawWithdrawMenu() {
    // Draw ATM frame
    DrawRectangle(400, 100, 550, 650, ATM_BACKGROUND);
    DrawRectangleLines(400, 100, 550, 650, DARKGRAY);
    
    // Draw screen area
    DrawRectangle(450, 150, 450, 200, DARKGRAY);
    DrawRectangle(460, 160, 430, 180, ATM_DISPLAY_BG);
    DrawText("Enter Withdrawal Amount:", 470, 170, 20, ATM_TEXT);
    
    if (!withdrawInput.empty()) {
        DrawText(("£" + withdrawInput).c_str(), 470, 200, 30, ATM_TEXT);
    }

    // Draw keypad using GuiButton
    drawKeypad(500, 400, handleWithdrawInput);
}

void drawWaitingForCard() {
    // Draw ATM outer casing
    DrawRectangle(400, 100, 550, 650, ATM_BACKGROUND);
    DrawRectangleLines(400, 100, 550, 650, DARKGRAY);
    
    // Draw screen bezel
    DrawRectangle(450, 150, 450, 200, DARKGRAY);
    
    // Draw screen display
    DrawRectangle(460, 160, 430, 180, ATM_DISPLAY_BG);
    DrawText("Please Insert Card", 570, 230, 20, ATM_TEXT);
    
    // Draw card slot
    DrawRectangle(450, 370, 200, 10, DARKGRAY);
    
    // Use GuiButton for card insertion
    Rectangle cardBtn = {450, 350, 200, 30};
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
    // Draw ATM frame
    DrawRectangle(400, 100, 550, 650, ATM_BACKGROUND);
    DrawRectangleLines(400, 100, 550, 650, DARKGRAY);
    
    // Draw screen area
    DrawRectangle(450, 150, 450, 200, DARKGRAY);
    DrawRectangle(460, 160, 430, 180, ATM_DISPLAY_BG);
    DrawText("Enter Deposit Amount:", 470, 170, 20, ATM_TEXT);
    
    if (!depositInput.empty()) {
        DrawText(("£" + depositInput).c_str(), 470, 200, 30, ATM_TEXT);
    }

    // Draw keypad using GuiButton
    drawKeypad(500, 400, handleDepositInput);

    // Draw deposit slot
    DrawRectangle(450, 370, 200, 10, DARKGRAY);
    DrawText("INSERT CASH HERE", 480, 355, 15, LIGHTGRAY);
}

void initializeATM() {
    setupGuiStyle();
}
