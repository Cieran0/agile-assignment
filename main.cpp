// main.cpp
#include <iostream>
#include "TransactionSwitch.h"
using namespace std;

int main() {
    // create instance of el server
    TransactionSwitch server;

    try {
        // start el server to listen for incoming transactions
        server.startServer();
    }
    catch (const exception& e) { // standard error message shiz
        cerr << "Error starting server: " << e.what() << endl;
    }

    return 0;
}