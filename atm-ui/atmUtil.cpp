#include "atmUtil.h"
#include "net.h"
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cstring>
#include <random>
#include <map>

bool validatedPIN = false;

string input = "";
uint64_t atmID = rand_uint64();
enum Screen screen = WaitingForCard;
enum Language defaultLanguage = ENGLISH;

std::string cardNumber = "5030153826527268";
std::string expiryDate = "06/28";
double balance = 0;

testAccount a1 = {localPin: "1234", testBalance : 1000};

// shouldve probably used an enum here but too late
std::map<std::string, std::string> englishStrings {
    {"PIN_PROMPT", "Please enter your PIN:"},
    {"WITHDRAWAL_PROMPT", "Enter amount for withdrawal:"},
    {"BALANCE_TEXT", "Account balance:"},
    {"INSERTED_CARD_TEXT", "Inserted card:"},
    {"INSERT_HERE_TEXT", "INSERT CARD HERE"},
    {"PROCESSING_TEXT", "Processing..."},
    {"PRINTING_SUCCESSFUL_TEXT", "Printing"},
    {"INCORRECT_PIN_TEXT", "Incorrect PIN. Please try again."},
    {"INSUFFICIENT_FUNDS_TEXT", "Insufficient funds..."},
    {"VIEW_BALANCE_TEXT", "View Balance"},
    {"PRINT_BALANCE_TEXT", "Print Balance"},
    {"MAIN_MENU_TEXT", "Back to Main Menu"},
    {"BALANCE_BTN_TEXT", "Balance Inquiry"},
    {"DEPOSIT_BTN_TEXT", "Deposit"},
    {"EXIT_BTN_TEXT", "Exit"},
    {"TAKE_RECEIPT_TEXT", "TAKE YOUR RECEIPT"},
    {"WITHDRAWAL_TEXT", "Cash Withdrawal"},
    {"DEPOSIT_TEXT", "Enter Deposit Amount"},
    {"BALANCE_OPTION_TEXT", "Select balance option"}
};

std::map<std::string, std::string> frenchStrings {
    {"PIN_PROMPT",               "Veuillez saisir votre code PIN :"},
    {"WITHDRAWAL_PROMPT",        "Entrez le montant du retrait :"},
    {"BALANCE_TEXT",             "Solde du compte :"},
    {"INSERTED_CARD_TEXT",       "Carte insérée :"},
    {"INSERT_HERE_TEXT",         "INSÉREZ LA CARTE ICI"},
    {"PROCESSING_TEXT",          "Traitement en cours..."},
    {"PRINTING_SUCCESSFUL_TEXT", "Impression en cours"},
    {"INCORRECT_PIN_TEXT",       "Code PIN incorrect. Veuillez réessayer."},
    {"INSUFFICIENT_FUNDS_TEXT",  "Fonds insuffisants..."},
    {"VIEW_BALANCE_TEXT",        "Afficher le solde"},
    {"PRINT_BALANCE_TEXT",       "Imprimer le solde"},
    {"MAIN_MENU_TEXT",           "Retour au menu principal"},
    {"BALANCE_BTN_TEXT",         "Consultation de solde"},
    {"DEPOSIT_BTN_TEXT",         "Dépôt"},
    {"EXIT_BTN_TEXT",            "Quitter"},
    {"TAKE_RECEIPT_TEXT",        "Prends ton reçu"},
    {"WITHDRAWAL_TEXT",          "Retrait en espèces"},
    {"DEPOSIT_TEXT",             "Entrez le montant du dépôt"},
    {"BALANCE_OPTION_TEXT",      "Sélectionnez l'option de solde"}
};

std::map<std::string, std::string> spanishStrings {
    {"PIN_PROMPT",               "Por favor, introduzca su PIN:"},
    {"WITHDRAWAL_PROMPT",        "Ingrese la cantidad a retirar:"},
    {"BALANCE_TEXT",             "Saldo de la cuenta:"},
    {"INSERTED_CARD_TEXT",       "Tarjeta insertada:"},
    {"INSERT_HERE_TEXT",         "INSERTE LA TARJETA AQUÍ"},
    {"PROCESSING_TEXT",          "Procesando..."},
    {"PRINTING_SUCCESSFUL_TEXT", "Imprimiendo"},
    {"INCORRECT_PIN_TEXT",       "PIN incorrecto. Por favor, inténtelo de nuevo."},
    {"INSUFFICIENT_FUNDS_TEXT",  "Fondos insuficientes..."},
    {"VIEW_BALANCE_TEXT",        "Ver saldo"},
    {"PRINT_BALANCE_TEXT",       "Imprimir saldo"},
    {"MAIN_MENU_TEXT",           "Volver al menú principal"},
    {"BALANCE_BTN_TEXT",         "Consulta de saldo"},
    {"DEPOSIT_BTN_TEXT",         "Depósito"},
    {"EXIT_BTN_TEXT",            "Salir"},
    {"TAKE_RECEIPT_TEXT",        "Toma tu recibo"},
    {"WITHDRAWAL_TEXT",          "Retiro de efectivo"},
    {"DEPOSIT_TEXT",             "Ingrese el monto del depósito"},
    {"BALANCE_OPTION_TEXT",      "Seleccione opción de saldo"}
};

std::map<std::string, std::string> germanStrings {
    {"PIN_PROMPT",               "Bitte geben Sie Ihre PIN ein:"},
    {"WITHDRAWAL_PROMPT",        "Betrag für Auszahlung eingeben:"},
    {"BALANCE_TEXT",             "Kontostand:"},
    {"INSERTED_CARD_TEXT",       "Eingelegte Karte:"},
    {"INSERT_HERE_TEXT",         "KARTE HIER EINLEGEN"},
    {"PROCESSING_TEXT",          "Verarbeitung..."},
    {"PRINTING_SUCCESSFUL_TEXT", "Druckvorgang"},
    {"INCORRECT_PIN_TEXT",       "Falsche PIN. Bitte erneut versuchen."},
    {"INSUFFICIENT_FUNDS_TEXT",  "Nicht ausreichendes Guthaben..."},
    {"VIEW_BALANCE_TEXT",        "Kontostand anzeigen"},
    {"PRINT_BALANCE_TEXT",       "Kontostand drucken"},
    {"MAIN_MENU_TEXT",           "Zurück zum Hauptmenü"},
    {"BALANCE_BTN_TEXT",         "Kontostandabfrage"},
    {"DEPOSIT_BTN_TEXT",         "Einzahlung"},
    {"EXIT_BTN_TEXT",            "Beenden"},
    {"TAKE_RECEIPT_TEXT",        "Nehmen Sie Ihre Quittung"},
    {"WITHDRAWAL_TEXT",          "Bargeldauszahlung"},
    {"DEPOSIT_TEXT",             "Geben Sie den Einzahlungsbetrag ein"},
    {"BALANCE_OPTION_TEXT",      "Wählen Sie die Saldooption"}
};


string keyPad[5][3] = {
    {"1", "2", "3"},
    {"4", "5", "6"},
    {"7", "8", "9"},
    {" ", "0", " "},
    {"cancel", "clear", "enter"}
};

vector<string> inputs;
string displayText = getStringInLanguage("PIN_PROMPT");
string withdrawlText = getStringInLanguage("WITHDRAWAL_PROMPT");
string pinDisplay = "";
string enteredPIN;

string getStringInLanguage(string prompt) {
    switch(defaultLanguage) {
        case ENGLISH:
            return englishStrings.at(prompt);
        case SPANISH:
            return spanishStrings.at(prompt);
        case GERMAN:
            return germanStrings.at(prompt);
        case FRENCH:
            return frenchStrings.at(prompt);
    }
}

void resetGlobalTextVariables() {
    input.clear();
    pinDisplay = "";
    displayText = getStringInLanguage("PIN_PROMPT");
    withdrawlText = getStringInLanguage("WITHDRAWAL_PROMPT");
}

void updatePinDisplay() {
    pinDisplay = string(input.size(), '*');
}

void printBalanceToFile(string balance){
    ofstream balancePrint("printedBalance.txt");
    balancePrint << getStringInLanguage("BALANCE_TEXT") << balance << endl;
    balancePrint.close();
}

void setScreen(Screen s) {
    screen = s;
    resetGlobalTextVariables();
    if(s == Screen::EnterPin) {
        enteredPIN = "";
    }
}

void setLanguage(Language language) {
    defaultLanguage = language;
    resetGlobalTextVariables();
}