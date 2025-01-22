#include <iostream>
#include <cstring>
#include <sstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

void sendCommand(int sock, const std::string& command) {
    std::cout << "Sending command: " << command << std::endl; // Debug log
    send(sock, command.c_str(), command.size(), 0);
}

int main() {
    const char* server_ip = "127.0.0.1";
    const int server_port = 6667;

    std::cout << "Creating socket..." << std::endl;
    int client_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (client_sock < 0) {
        perror("Socket creation failed");
        return 1;
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    inet_pton(AF_INET, server_ip, &server_addr.sin_addr);

    std::cout << "Connecting to server..." << std::endl;
    if (connect(client_sock, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection to server failed");
        close(client_sock);
        return 1;
    }

    std::cout << "Connected to server.\n";

    // IRC setup commands
    sendCommand(client_sock, "NICK TestClient\r\n");
    sendCommand(client_sock, "USER test 0 * :Test Client\r\n");

    char buffer[512];
    while (true) { // menu thing
        std::cout << "Select an option:\n";
        std::cout << "1. Withdraw\n";
        std::cout << "2. Quit\n";
        std::cout << "Enter your choice: ";

        int choice;
        std::cin >> choice;

        if (choice == 2) {
            std::cout << "Quitting...\n";
            break;
        } else if (choice == 1) {
            std::cout << "Enter the amount to withdraw: ";
            int amount;
            std::cin >> amount;

            std::ostringstream request;
            request << "PRIVMSG :Withdrawal|" << amount << "\r\n";

            sendCommand(client_sock, request.str());

            std::cout << "Waiting for server response...\n";
            memset(buffer, 0, sizeof(buffer));
            int bytes_received = recv(client_sock, buffer, sizeof(buffer), 0);
            if (bytes_received > 0) {
                std::cout << "Server response: " << std::string(buffer, bytes_received) << "\n";
            } else {
                std::cout << "No response or connection closed.\n";
                break;
            }
        } else {
            std::cout << "Invalid choice. Please try again.\n";
        }
    }

    std::cout << "Closing connection.\n";
    close(client_sock);
    return 0;
}
