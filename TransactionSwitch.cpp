#include <iostream>
#include <cstring>
#include <sstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <vector>
#include "TransactionLogr.h"
#include "Transaction.h"
#include "NetworkRouter.h"

class TransactionSwitch {
public:
    TransactionSwitch() : logger(), router() {}

    void startServer() {
        int server_sock_6667 = createSocket(6667);//for atm
        int server_sock_6668 = createSocket(6668);//for network

        std::cout << "Running on ports 6667 and 6668...\n";

        std::vector<int> server_sockets = {server_sock_6667, server_sock_6668};

        while (true)
        {
            int atm_socket = accept(server_sock_6667,  nullptr, nullptr);
          if (atm_socket < 0) {
            std::cerr << "Accept failed!" << std::endl;
            exit(EXIT_FAILURE);
           }
           handleAtmRequest(atm_socket);
           close(atm_socket);
        }
        
        
        // while (true) {
        //     fd_set read_fds;
        //     FD_ZERO(&read_fds);

        //     int max_fd = 0;
        //     for (int sock : server_sockets) {
        //         FD_SET(sock, &read_fds);
        //         if (sock > max_fd) max_fd = sock;
        //     }

        //   //  Wait for activity on any socket
        //     int activity = select(max_fd + 1, &read_fds, nullptr, nullptr, nullptr);
        //     if (activity < 0) {
        //         perror("Select error");
        //         continue;
        //     }

        //     for (int sock : server_sockets) {
        //         if (FD_ISSET(sock, &read_fds)) {
        //             sockaddr_in client_addr;
        //             socklen_t addr_len = sizeof(client_addr);
        //             int client_sock = accept(sock, (struct sockaddr*)&client_addr, &addr_len);
        //             if (client_sock < 0) {
        //                 perror("Cannot accept connection");
        //                 continue;
        //             }

        //             std::cout << "Connection accepted on port "
        //                       << (sock == server_sock_6667 ? "6667" : "6668") << "\n";

        //             handleAtmRequest(client_sock);
        //             close(client_sock);
        //         }
        //     }
        // }



        close(server_sock_6667);
        close(server_sock_6668);
    }

private:
    TransactionLogger logger;
    NetworkRouter router;

    int createSocket(int port) {
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
            perror("Cannot create socket");
            exit(EXIT_FAILURE);
        }

        sockaddr_in addr = {};
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

        std::cout << "Listening on port " << port << "\n";
        return sock;
    }

    int connectToNetwork(int port) {
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        sockaddr_in serv_addr;
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(port);
        inet_pton(AF_INET, "10.201.102.155", &serv_addr.sin_addr);

        if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
             std::cerr << "Failed to connect to Network\n";
             return -1;
         }
       return sock;
     }

    void handleAtmRequest(int atm_sock) {
        char buffer[512];
        //std::string nick;

        while (true) {
            Transaction message;
            memset(buffer, 0, sizeof(buffer));
            int bytes = recv(atm_sock, &message, sizeof(Transaction), 0);
            if (bytes <= 0) break;

        
            std::cout << "Received: " << message.cardNumber << std::endl;
            
            Response response = {0};
            int network_socket = connectToNetwork(6668);

            send(network_socket, &message, sizeof(Transaction), 0);

            int answer = recv(network_socket, &response, sizeof(Response), 0);
            if (answer <= 0) break;
            close(network_socket);
            send(atm_sock, &answer, sizeof(Response), 0);
        }
    }

    // std::string parseParameter(const std::string& message) {
    //     size_t pos = message.find(' ');
    //     if (pos != std::string::npos) {
    //         return message.substr(pos + 1, message.find('\r', pos) - pos - 1);
    //     }
    //     return "";
    // }

    // std::string parseMessage(const std::string& message) {
    //     size_t pos = message.find(':');
    //     if (pos != std::string::npos) {
    //         std::string parsed = message.substr(pos + 1);
    //         parsed.erase(parsed.find_last_not_of("\r\n") + 1); // Trim trailing newline and carriage return
    //         return parsed;
    //     }
    //     return "";
    // }

    // std::string handleTransactionRequest(const std::string& request) {
    //     logger.logTransaction("Received transaction request: " + request);

    //     if (request.find("Withdrawal|") == 0) {
    //         size_t delimiter = request.find('|');
    //         std::string amountStr = request.substr(delimiter + 1);
    //         int amount = std::stoi(amountStr);

    //         std::string response;
    //         if (amount <= 200) {
    //             response = "Approved|Transaction successful!";
    //         } else {
    //             response = "Declined|Amount exceeds limit.";
    //         }

    //         logger.logTransaction("Transaction response: " + response);
    //         router.routeTransaction(request);

    //         return ":localhost NOTICE * :" + response + "\r\n";
    //     } else if (request == "BalanceInquiry") {
    //         std::string response = "Approved|Balance: 1000.0";
    //         logger.logTransaction("Transaction response: " + response);
    //         return ":localhost NOTICE * :" + response + "\r\n";
    //     } else {
    //         return ":localhost NOTICE * :Invalid transaction request.\r\n";
    //     }
    // }
};
