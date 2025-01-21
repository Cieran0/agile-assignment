#include <iostream>
#include <cstring>
#include <sstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "TransactionLogr.h"
#include "NetworkRouter.h"

class TransactionSwitch {
public:
    TransactionSwitch() : logger(), router() {}

    void startServer() {
        int server_sock = createSocket(6667);  // IRC default port
        std::cout << "IRC-compatible Transaction Switch running on port 6667...\n";

        while (true) {
            sockaddr_in client_addr;
            socklen_t addr_len = sizeof(client_addr);
            int client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_len);
            if (client_sock < 0) {
                perror("Unable to accept connection");
                continue;
            }

            handleClient(client_sock);
            close(client_sock);
        }

        close(server_sock);
    }

private:
    TransactionLogger logger;
    NetworkRouter router;

    int createSocket(int port) {
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
            perror("Unable to create socket");
            exit(EXIT_FAILURE);
        }

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = INADDR_ANY;

        if (bind(sock, (sockaddr*)&addr, sizeof(addr)) < 0) {
            perror("Unable to bind");
            exit(EXIT_FAILURE);
        }

        if (listen(sock, 5) < 0) {
            perror("Unable to listen");
            exit(EXIT_FAILURE);
        }

        return sock;
    }

    void handleClient(int client_sock) {
    char buffer[512];
    std::string nick; // Track the nickname of the user

    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytes = recv(client_sock, buffer, sizeof(buffer), 0);
        if (bytes <= 0) break; // Connection closed or error

        std::string message(buffer);
        std::cout << "Received: " << message << std::endl;

        if (message.find("NICK") == 0) {
            // Handle NICK command
            nick = parseParameter(message);
            std::string response = ":localhost NOTICE * :Welcome, " + nick + "!\r\n";
            send(client_sock, response.c_str(), response.size(), 0);

        } else if (message.find("USER") == 0) {
            // Handle USER command
            std::string user = parseParameter(message);
            std::string response = ":localhost NOTICE * :User registered as " + user + ".\r\n";
            send(client_sock, response.c_str(), response.size(), 0);

        } else if (message.find("PRIVMSG") == 0) {
            // Handle PRIVMSG (transaction request)
            std::string request = parseMessage(message); // Extract transaction details
            std::string response = handleTransactionRequest(request); // Process transaction
            send(client_sock, response.c_str(), response.size(), 0);

        } else if (message.find("PING") == 0) {
            // Respond to PING
            std::string response = "PONG :localhost\r\n";
            send(client_sock, response.c_str(), response.size(), 0);

        } else {
            // Handle unknown commands
            std::string response = ":localhost 421 Unknown command\r\n";
            send(client_sock, response.c_str(), response.size(), 0);
        }
    }
}



    std::string parseParameter(const std::string& message) {
    size_t pos = message.find(' ');
    if (pos != std::string::npos) {
        return message.substr(pos + 1, message.find('\r', pos) - pos - 1);
    }
    return "";
}


    std::string parseMessage(const std::string& message) {
    size_t pos = message.find(':');
    if (pos != std::string::npos) {
        return message.substr(pos + 1, message.find('\r', pos) - pos - 1);
    }
    return "";
}



    std::string handleTransactionRequest(const std::string& request) {
    // Log the transaction request
    logger.logTransaction("Received transaction request: " + request);

    if (request.find("Withdrawal|") == 0) {
        // Parse the amount from the request
        size_t delimiter = request.find('|');
        std::string amountStr = request.substr(delimiter + 1);
        int amount = std::stoi(amountStr);

        // Simulate approval logic
        std::string response;
        if (amount <= 200) { // Example limit
            response = "Approved|Transaction successful!";
        } else {
            response = "Declined|Amount exceeds limit.";
        }

        // Log the transaction response
        logger.logTransaction("Transaction response: " + response);

        // Route the transaction (simulated)
        router.routeTransaction(request);

        return ":localhost NOTICE * :" + response + "\r\n";

    } else if (request == "BalanceInquiry") {
        std::string response = "Approved|Balance: 1000.0";
        logger.logTransaction("Transaction response: " + response);
        return ":localhost NOTICE * :" + response + "\r\n";

    } else {
        return ":localhost NOTICE * :Invalid transaction request.\r\n";
    }
}



};
