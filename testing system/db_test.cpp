#include "test.hpp"
#include "../network simulator/db.hpp"
#include <string>

bool operator==(const Transaction& t1, const Transaction& t2) {
    return std::string(t1.cardNumber) == std::string(t2.cardNumber) && std::string(t1.expiryDate) == std::string(t2.expiryDate) && t1.atmID == t2.atmID && t1.uniqueTransactionID == t2.uniqueTransactionID && std::string(t1.pinNo) == std::string(t2.pinNo) && t1.withdrawalAmount == t2.withdrawalAmount;
}

bool operator==(const Response& r1, const Response& r2){
    return r1.succeeded == r2.succeeded && r1.new_balance == r2.new_balance;
}

int main(){

}
