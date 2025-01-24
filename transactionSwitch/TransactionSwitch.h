// TransactionSwitch.h
#ifndef TRANSACTIONSWITCH_H
#define TRANSACTIONSWITCH_H

#include <string>
#include "TransactionLogr.h"

class TransactionSwitch {
public:
    // constructor & destructor
    TransactionSwitch(const char* network_sim_ip, int network_sim_port);
    ~TransactionSwitch();

    // start server
    void startServer();

private:
    // process incoming transaction
    std::string processTransaction(const std::string& request);

    // memb vars
    TransactionLogger logger;
};

#endif // TRANSACTIONSWITCH_H