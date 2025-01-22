#include "TransactionSwitchSecure.cpp"

int main() {
    TransactionSwitch server;

    try {
        server.startServer();
    } catch (const std::exception &e) {
        std::cerr << "Error starting server: " << e.what() << std::endl;
    }

    return 0;
}
