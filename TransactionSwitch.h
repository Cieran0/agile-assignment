// TransactionSwitch.h
#ifndef TRANSACTIONSWITCH_H
#define TRANSACTIONSWITCH_H

#include <string>
#include "TransactionLogr.h"
#include "NetworkRouter.h"

class TransactionSwitch {
public:
    // constructor & destructor
    TransactionSwitch();
    ~TransactionSwitch();

    // start server
    void startServer();

private:
    // process incoming transaction
    std::string processTransaction(const std::string& request);

    // memb vars
    TransactionLogger logger;
    NetworkRouter router;
};

#endif // TRANSACTIONSWITCH_H