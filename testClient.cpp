#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

// Connect to the server
int connectToServer(const std::string &server_ip, int port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        return -1;
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, server_ip.c_str(), &server_addr.sin_addr) <= 0) {
        perror("Invalid server IP address");
        close(sock);
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection to server failed");
        close(sock);
        return -1;
    }

    std::cout << "Connected to server at " << server_ip << ":" << port << std::endl;
    return sock;
}

// Send a message to the server and receive the response
void sendMessage(int sock, const std::string &message) {
    std::string formatted_message = "PRIVMSG localhost :" + message + "\r\n";
    if (send(sock, formatted_message.c_str(), formatted_message.length(), 0) < 0) {
        perror("Send failed");
        return;
    }

    std::cout << "Sent: " << formatted_message;

    char buffer[512] = {0};
    int bytes = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if (bytes > 0) {
        std::cout << "Received: " << std::string(buffer) << std::endl;
    } else if (bytes == 0) {
        std::cout << "Server closed the connection." << std::endl;
    } else {
        perror("Receive failed");
    }
}

int main() {
    std::string server_ip = "127.0.0.1"; // Localhost
    int port = 6667;                    // Same port as the server

    // Connect to the server
    int sock = connectToServer(server_ip, port);
    if (sock < 0) {
        return 1;
    }

    bool boob = true;

    // Main loop for sending requests
    std::string input;
    while (boob) {
        std::cout << "Enter transaction request (or type 'exit' to quit): ";
        std::getline(std::cin, input);

        if (input == "exit") {
            break;
        }

        sendMessage(sock, input);
    }

    close(sock);
    std::cout << "Disconnected from server." << std::endl;
    return 0;
}
