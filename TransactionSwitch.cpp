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
        std::string nick;

        while (true) {
            memset(buffer, 0, sizeof(buffer));
            int bytes = recv(client_sock, buffer, sizeof(buffer), 0);
            if (bytes <= 0) break;

            std::string message(buffer);
            std::cout << "Received: " << message << std::endl;

            std::string response;
            try {
                if (message.find("NICK") == 0) {
                    nick = parseParameter(message);
                    response = ":localhost NOTICE * :Welcome, " + nick + "!\r\n";
                } else if (message.find("USER") == 0) {
                    std::string user = parseParameter(message);
                    response = ":localhost NOTICE * :User registered as " + user + ".\r\n";
                } else if (message.find("PRIVMSG") == 0) {
                    std::string request = parseMessage(message);
                    if (!request.empty()) {
                        std::cout << "Extracted transaction request: " << request << std::endl; // Debugging
                        response = handleTransactionRequest(request);
                    } else {
                        response = ":localhost NOTICE * :Invalid PRIVMSG format.\r\n";
                    }
                } else if (message.find("PING") == 0) {
                    response = "PONG :localhost\r\n";
                } else {
                    response = ":localhost 421 Unknown command boob\r\n";
                }

                std::cout << "Sending response: " << response << std::endl;
                send(client_sock, response.c_str(), response.size(), 0);
            } catch (const std::exception& e) {
                std::cerr << "Error handling request: " << e.what() << std::endl;
                response = ":localhost 400 Error processing request\r\n";
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
            std::string parsed = message.substr(pos + 1);
            parsed.erase(parsed.find_last_not_of("\r\n") + 1); // Trim trailing newline and carriage return
            std::cout << "Parsed message: " << parsed << std::endl; // Debugging
            return parsed;
        }
        return "";
    }

    std::string handleTransactionRequest(const std::string& request) {
        logger.logTransaction("Received transaction request: " + request);

        if (request.find("Withdrawal|") == 0) {
            size_t delimiter = request.find('|');
            std::string amountStr = request.substr(delimiter + 1);
            int amount = std::stoi(amountStr);

            std::string response;
            if (amount <= 200) {
                response = "Approved|Transaction successful!";
            } else {
                response = "Declined|Amount exceeds limit.";
            }

            logger.logTransaction("Transaction response: " + response);
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
