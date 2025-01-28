#include <sqlite3.h>
#include "Transaction.hpp"

Response processTransaction(Transaction transaction, sqlite3*& db);
int initDatabaseConnection(sqlite3*& db);