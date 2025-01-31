#pragma once
#include <string>

enum Language {
    ENGLISH,
    FRENCH,
    SPANISH,
    GERMAN
};

enum LocalString {
    INSERT_CARD,
    WITHDRAWAL_MSG,
    ENTER_PIN,
    CANCEL,
    CLEAR,
    ENTER,
    BALANCE_INQUIRY,
    DEPOSIT_MSG,
    EXIT,
    SELECT_LANG,
    L_ENGLISH, 
    L_FRENCH,
    L_SPANISH, 
    L_GERMAN,
    SELECT_OPTION,
    PRINT_BALANCE,
    VIEW_BALANCE,
    BACK,
    YOUR_BALANCE,
    PROCESSING,
    TAKE_RECEIPT,
    CUSTOM_AMOUNT,
    TRANSACTION_SUCCESS,
    WITHDRAWAL_AMOUNT,
    DEPOSIT_AMOUNT,
    GENERIC_ERROR_MSG,
    INSUFFICIENT_FUNDS_MSG,
    INCORRECT_PIN_MSG,
    CASH,
};

std::string inCurrentLanguage(LocalString str);

extern Language currentLanguage;