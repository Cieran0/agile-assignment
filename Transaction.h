
#include <cstdint>
struct Transaction{
    char cardNumber[20];
    char expiryDate[6];
    uint64_t atmID;
    uint64_t uniquetransactionID;
    char pinNo[5];
    double withdrawalAmount;

};

struct Response{
    int succeeded;
    double new_balance;

};