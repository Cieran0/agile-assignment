#include "TransactionSwitchSecure.cpp"

int main(int argc, char *argv[]) {

    const char* ip_address; 
    int port;

    if(argc == 1) {
        ip_address = "127.0.0.1";
        port = 6668;
    } 
    else if (argc == 3) {
        ip_address = argv[1];
        port = std::stoi(argv[2]);
    }
    else {
        std::cerr << "Usage: " << argv[0] << " <IP_ADDRESS> <PORT>" << std::endl;
        return EXIT_FAILURE;
    }

    // Create the server instance
    TransactionSwitch server(ip_address, port);

    try {
        server.startServer();
    } catch (const std::exception &e) {
        std::cerr << "Error starting server: " << e.what() << std::endl;
    }

    return 0;
}
