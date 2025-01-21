#include <sqlite3.h>
#include <cstdint>

struct Transaction {
    char cardNumber[20];
    char expiryDate[6];
    uint64_t atmID;
    uint64_t uniqueTransactionID;
    char pinNo[5];
    double withdrawalAmount;
};

static int callback(void* data, int argc, char** argv, char** azColName);
int doTransaction(Transaction t, double &balance, sqlite3* db);
int init(sqlite3& db);
