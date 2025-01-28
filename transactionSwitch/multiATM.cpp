#include <openssl/ssl.h>
#include <openssl/err.h>
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include "TransactionLogr.h"
#include "Transaction.h"
#include <queue>
#include <functional>
#include <chrono>

void simulateATM(const char* server_ip, int server_port, int atm_id) {
    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();

    SSL_CTX* ctx = SSL_CTX_new(TLS_client_method());
    if (!ctx) {
        std::cerr << "Error creating SSL context" << std::endl;
        return;
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "Error creating socket" << std::endl;
        SSL_CTX_free(ctx);
        return;
    }

    sockaddr_in server_addr = {};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        std::cerr << "Invalid server IP address" << std::endl;
        close(sock);
        SSL_CTX_free(ctx);
        return;
    }

    if (connect(sock, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Error connecting to server" << std::endl;
        close(sock);
        SSL_CTX_free(ctx);
        return;
    }

    SSL* ssl = SSL_new(ctx);
    SSL_set_fd(ssl, sock);

    if (SSL_connect(ssl) <= 0) {
        std::cerr << "SSL connection failed" << std::endl;
        ERR_print_errors_fp(stderr);
        SSL_free(ssl);
        close(sock);
        SSL_CTX_free(ctx);
        return;
    }

    std::cout << "ATM " << atm_id << " connected to server.\n";

    // Ensure connection lasts for 5 minutes
    auto start_time = std::chrono::steady_clock::now();
    int transaction_count = 0;

    while (std::chrono::steady_clock::now() - start_time < std::chrono::minutes(5)) {
        Transaction transaction = {};
        strcpy(transaction.cardNumber, "1234567890123456");
        strcpy(transaction.expiryDate, "0125");
        transaction.atmID = atm_id;
        transaction.uniquetransactionID = atm_id * 1000 + transaction_count;
        strcpy(transaction.pinNo, "1234");
        transaction.withdrawalAmount = 50.0;

        if (SSL_write(ssl, &transaction, sizeof(transaction)) <= 0) {
            std::cerr << "Failed to send transaction" << std::endl;
            ERR_print_errors_fp(stderr);
            break;
        }

        std::cout << "ATM " << atm_id << " sent transaction " << transaction.uniquetransactionID << "\n";

        Response response = {};
        if (SSL_read(ssl, &response, sizeof(response)) <= 0) {
            std::cerr << "Failed to receive response" << std::endl;
            ERR_print_errors_fp(stderr);
            break;
        }

        std::cout << "ATM " << atm_id << " received response: Success=" << response.succeeded
                  << ", New Balance=" << response.new_balance << "\n";

        transaction_count++;

        // Wait 30 seconds before sending the next transaction
        std::this_thread::sleep_for(std::chrono::seconds(30));
    }

    std::cout << "ATM " << atm_id << " completed transactions and disconnected.\n";

    SSL_free(ssl);
    close(sock);
    SSL_CTX_free(ctx);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <SERVER_IP> <SERVER_PORT>\n";
        return EXIT_FAILURE;
    }

    const char* server_ip = argv[1];
    int server_port = std::stoi(argv[2]);

    std::vector<std::thread> atm_threads;

    // Simulate 100 ATMs
    for (int i = 1; i <= 100; ++i) {
        atm_threads.emplace_back(simulateATM, server_ip, server_port, i);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));  // Small delay to stagger connections
    }

    // Wait for all ATMs to finish
    for (auto& thread : atm_threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    return 0;
}
