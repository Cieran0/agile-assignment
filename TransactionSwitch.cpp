// TransactionSwitch.cpp
#include "TransactionSwitch.h"
#include <iostream>

TransactionSwitch::TransactionSwitch() {
}

TransactionSwitch::~TransactionSwitch() {
}

void TransactionSwitch::startServer() {
    std::cout << "Server started and listening for transactions..." << std::endl;

    // pretend transaction came through
    std::string incomingTransaction = "Transaction: Withdraw $100";
    std::string response = processTransaction(incomingTransaction);

    std::cout << "Response sent to ATM: " << response << std::endl;
}

std::string TransactionSwitch::processTransaction(const std::string& request) {
    std::cout << "Processing transaction: " << request << std::endl;

    // put in file
    logger.logTransaction(request);

    // send to network sim
    return router.routeTransaction(request);
}