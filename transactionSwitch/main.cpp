#include "TransactionSwitchSecure.cpp"
#include <vector>
#include <iostream>

int main(int argc, char *argv[]) {

    std::vector<const char*> ip_addresses;
    std::vector<int> ports;

    if(argc == 1) {
        // Default values if no arguments are provided
        ip_addresses = {"127.0.0.1", "127.0.0.1", "127.0.0.1"};
        ports = {6668, 6669, 6670};
    } 
    else if (argc == 7) {
        // Expecting 3 IP addresses and 3 ports
        ip_addresses.push_back(argv[1]);
        ip_addresses.push_back(argv[3]);
        ip_addresses.push_back(argv[5]);
        ports.push_back(std::stoi(argv[2]));
        ports.push_back(std::stoi(argv[4]));
        ports.push_back(std::stoi(argv[6]));
    }
    else {
        std::cerr << "Usage: " << argv[0] << " <IP_ADDRESS_1> <PORT_1> <IP_ADDRESS_2> <PORT_2> <IP_ADDRESS_3> <PORT_3>" << std::endl;
        return EXIT_FAILURE;
    }

    // Create the server instances
    std::vector<TransactionSwitch> servers;
    for (size_t i = 0; i < ip_addresses.size(); ++i) {
        servers.emplace_back(ip_addresses[i], ports[i]);
    }

    try {
        for (auto& server : servers) {
            server.startServer();
        }
    } catch (const std::exception &e) {
        std::cerr << "Error starting server: " << e.what() << std::endl;
    }

    return 0;
}
