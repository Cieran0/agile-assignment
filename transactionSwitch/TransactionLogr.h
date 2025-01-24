// TransactionLogr.h
#ifndef TRANSACTIONLOGR_H
#define TRANSACTIONLOGR_H

#include <string>
#include <fstream>
#include "Transaction.h"

class TransactionLogger {
public:
    // constructor & destructor
    TransactionLogger();
    ~TransactionLogger();

    // save transaction to a log file
    void logTransaction(const Transaction& transaction);

private:
    // shiz
    std::ofstream logFile;
};

#endif // TRANSACTIONLOGR_H
