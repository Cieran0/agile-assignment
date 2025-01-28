#include <sqlite3.h>
#include <cstdint>
#include <future>

struct Transaction {
    char cardNumber[20];
    char expiryDate[6];
    uint64_t atmID;
    uint64_t uniqueTransactionID;
    char pinNo[5];
    double withdrawalAmount;
};

#define TRANSACTION_SUCESS 0
#define INSUFFICIENT_FUNDS 1
#define DATABASE_ERROR 2
#define INCORRECT_PIN 3

struct Response {
    int succeeded;
    double new_balance;
};

std::future<Response> enqueueTransaction(Transaction transaction);
void processTransactionQueue();

using TransactionPromise = std::pair<Transaction, std::promise<Response>>;