#include "atmUtil.h"



vector<string> inputs;
string displayText = "Please enter your PIN:"; // Global display text
string balanceText = "Enter amount for withdrawl";

string pinDisplay = ""; // To show asterisks for PIN
string enteredPIN;
static string withdrawInput = "";
bool validatedPIN = false;

#define ROW_COUNT 4
#define COLUMN_COUNT 4

enum Screen {
    EnterPin = 1,
    MainMenu = 2,
    Withdraw = 3,
    Balance = 4,
    BalanceAmount = 5,
    Deposit = 6,
    PrintBalance = 7
};

enum Screen screen = EnterPin;

Account a1 = {PIN: "5541", cardNumber:"5030153826527268", expiryDate: "06/28",balance: 1000};

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
        //validatedPIN = validatePIN(enteredPIN);     
        Response r = forwardToSocket(a1.cardNumber, a1.expiryDate, "1", "2001", a1.PIN, 0.0);

        if(r.succeeded == 0) {
            screen = MainMenu;
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

void displayTransactionChoices(){

        Button buttons[] = {
                {{100, 100, 200, 50}, "Balance Inquiry"},
                {{100, 200, 200, 50}, "Cash Withdrawal"},
                {{100, 300, 200, 50}, "Deposit"},
                {{100, 400, 200, 50}, "Exit"}
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
                case 3:
                    screen = EnterPin;
                    cout << "Enter Pin" << endl;
                    resetGlobalTextVariables();
                    break;
                }
            }
        }
}

void atmLayout() {

        DrawRectangle(screenWidth/4 ,screenHeight/6, screenWidth/2,screenHeight/4, GREEN);

        // Draw the display text
        DrawText(displayText.c_str(), screenWidth/4 + (5) ,screenHeight/6 + (5), screenHeight/30, BLACK);
        
        // Draw the PIN display
        if (!pinDisplay.empty()) {
            DrawText(pinDisplay.c_str(), 510, 240, 30, BLACK);
        }

        int offsetX = 0;
        int offsetY = 0;
        for (int row = 0; row < ROW_COUNT; row++)
        { 
            for(int col = 0; col < COLUMN_COUNT; col ++){
                offsetX += screenWidth/10;
                // wouldnt compile on mac without being casted to a float...
                Rectangle btnRect = {
                    static_cast<float>(screenWidth/4 + offsetX),
                    static_cast<float>(screenHeight/2 + offsetY),
                    screenWidth/12,
                    screenHeight/24
                };
                if (keyPad[row][col] == "cancel") {
                    GuiSetStyle(DEFAULT, BASE_COLOR_NORMAL, ColorToInt(RED));
                }
                else if(keyPad[row][col] == "clear")
                {
                    GuiSetStyle(DEFAULT, BASE_COLOR_NORMAL, ColorToInt(YELLOW));

                }
                else if(keyPad[row][col] == "enter"){
                    GuiSetStyle(DEFAULT, BASE_COLOR_NORMAL, ColorToInt(GREEN));     
                }
                  else {
                    GuiSetStyle(DEFAULT, BASE_COLOR_NORMAL, ColorToInt(GRAY));
                }
                if (GuiButton(btnRect, keyPad[row][col].c_str()))
                {
                    handleInput(keyPad[row][col]);
                }


            }
            offsetY += screenHeight/15;
            offsetX = 0;
        }
}

void screenManager(){

    switch (screen) {
        case 1:
            atmLayout();
            //cout << "on enter pin menu" << endl;
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
            drawBalanceChoices();
            cout << "on the balance choice screen " << endl;
            break;
        case 5:
            viewBalance();
            cout << "view balance screen " << endl;
            break;
        case 7:
            cout << "print balance screen " << endl;
            printBalance();
            break;
    }
}

void drawBalanceChoices(){
    ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
    
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
                    screen = PrintBalance;
                    break;
                default:
                    break;
            }
        }
    }    
}

void viewBalance() {
    ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

    DrawText(to_string(a1.balance).c_str(), 200, 500, 40, BLACK);

    if (GuiButton({200, 600, 200, 50}, "Back")) {
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

void printBalance(){
    ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
    
    processingScreen(to_string(a1.balance));
    printFunction(to_string(a1.balance));
 
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
    ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
    DrawRectangle(500, 200, 350, 150, GREEN);
    DrawText("Enter Withdrawal Amount:", 510, 210, 20, BLACK);

    if (!withdrawInput.empty()) {
        DrawText(withdrawInput.c_str(), 510, 300, 30, BLACK);
    }

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
                handleWithdrawInput(keyPad[row][col]);
            }
        }
        offsetY += 50;
        offsetX = 0;
    }
}

Response forwardToSocket(string cardNumber, string expiryDate, string transactionID, string atmID, string pin, double withdrawalAmount) {
    const char *host = "127.0.0.1"; // Server address
    const int port = 6667;         // Server port

    // Initialise OpenSSL
    SSL_library_init();
    SSL_load_error_strings();
    const SSL_METHOD *method = TLS_client_method();
    SSL_CTX *ctx = SSL_CTX_new(method);

    if (!ctx) {
        cerr << "Failed to create SSL context" << endl;
        Response response;
        response.succeeded = DATABASE_ERROR;
        return response;
    }

    // Create a socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        cerr << "Failed to create socket" << endl;
        SSL_CTX_free(ctx);
        Response response;

        response.succeeded = DATABASE_ERROR;
        return response;
    }

    // Set up the server address struct
    sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, host, &server_addr.sin_addr) <= 0) {
        cerr << "Invalid server address" << endl;
        close(sock);
        SSL_CTX_free(ctx);
        Response response;

        response.succeeded = DATABASE_ERROR;
        return response;
    }

    // Connect to the server
    if (connect(sock, (sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        cerr << "Failed to connect to the server" << endl;
        close(sock);
        SSL_CTX_free(ctx);
        Response response;

        response.succeeded = DATABASE_ERROR;
        return response;
    }

    // Wrap the socket with SSL
    SSL *ssl = SSL_new(ctx);
    SSL_set_fd(ssl, sock);

    if (SSL_connect(ssl) <= 0) {
        cerr << "TLS handshake failed" << endl;
        SSL_free(ssl);
        close(sock);
        SSL_CTX_free(ctx);
        Response response;

        response.succeeded = DATABASE_ERROR;
        return response;
    }

    cout << "Connected to the server via TLS" << endl;

    // Prepare the transaction struct
    Transaction transaction;
    memset(&transaction, 0, sizeof(transaction));
    strncpy(transaction.cardNumber, cardNumber.c_str(), sizeof(transaction.cardNumber) - 1);
    strncpy(transaction.expiryDate, expiryDate.c_str(), sizeof(transaction.expiryDate) - 1);
    transaction.atmID = stoull(atmID);
    transaction.uniqueTransactionID = stoull(transactionID);
    strncpy(transaction.pinNo, pin.c_str(), sizeof(transaction.pinNo) - 1);
    transaction.withdrawalAmount = withdrawalAmount;

    // Send the transaction struct to the server
    if (SSL_write(ssl, &transaction, sizeof(transaction)) <= 0) {
        cerr << "Failed to send transaction to the server" << endl;
        SSL_free(ssl);
        close(sock);
        SSL_CTX_free(ctx);
        Response response;

        response.succeeded = DATABASE_ERROR;
        return response;
    }

    // Receive the response struct from the server
    Response response;
    if (SSL_read(ssl, &response, sizeof(response)) <= 0) {
        cerr << "Failed to receive response from the server" << endl;
        SSL_free(ssl);
        close(sock);
        SSL_CTX_free(ctx);
        response.succeeded = DATABASE_ERROR;
        return response;
    }

    // Print the response
    cout << "Transaction Response:" << endl;
    cout << "  Succeeded: " << response.succeeded << endl;
    cout << "  New Balance: " << response.new_balance << endl;

    // Clean up
    SSL_free(ssl);
    close(sock);
    SSL_CTX_free(ctx);

    return response;
}