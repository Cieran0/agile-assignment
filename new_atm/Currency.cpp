#include "Currency.hpp"

Currency currentCurrency = GBP;
std::string currencySymbol = "£";

void setCurrency(Currency currency) {
    switch (currency) {
        case USD:
            currentCurrency = USD;
            currencySymbol = "$";
            break;
        case JPY:
            currentCurrency = JPY;
            currencySymbol = "¥";
            break;
        case EUR:
            currentCurrency = EUR;
            currencySymbol = "EUR ";
            break;
        case AUD:
            currentCurrency = AUD;
            currencySymbol = "AU$";
            break;
        case CAD:
            currentCurrency = CAD;
            currencySymbol = "CA$";
            break;
        case CHF:
            currentCurrency = CHF;
            currencySymbol = "CHF ";
            break;
        case CNH:
            currentCurrency = CNH;
            currencySymbol = "CN¥"; 
            break;
        case HKD:
            currentCurrency = HKD;
            currencySymbol = "HK$"; 
            break;
        case NZD:
            currentCurrency = NZD;
            currencySymbol = "NZ$"; 
            break;
        case GBP:
        default:
            currentCurrency = GBP;
            currencySymbol = "£";
            break;
    }
}