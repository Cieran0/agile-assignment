#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

const std::string SERVER_IP = "172.17.0.1";
int NUMBER_OF_ATMS = 100;
int SERVER_PORT = 6667;
int TRANSACTIONS_PER_MINUTE = 2;
int SIMULATION_DURATION_MINUTES = 5;

// Initialize OpenSSL
SSL_CTX* create_ssl_context() {
    SSL_CTX *ctx = SSL_CTX_new(TLS_client_method());
    if (!ctx) {
        std::cerr << "Error creating SSL context\n";
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
    return ctx;
}

// ATM Client Function (TLS-secured)
void atm_client(int atm_id, SSL_CTX *ctx) {
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        std::cerr << "ATM " << atm_id << ": Error creating socket\n";
        return;
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP.c_str(), &server_addr.sin_addr);

    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "ATM " << atm_id << ": Connection failed\n";
        close(client_socket);
        return;
    }

    // Create SSL structure
    SSL *ssl = SSL_new(ctx);
    SSL_set_fd(ssl, client_socket);

    if (SSL_connect(ssl) <= 0) {
        std::cerr << "ATM " << atm_id << ": SSL handshake failed\n";
        ERR_print_errors_fp(stderr);
        SSL_free(ssl);
        close(client_socket);
        return;
    }

    std::cout << "ATM " << atm_id << " connected to server securely\n";

    int total_transactions = TRANSACTIONS_PER_MINUTE * SIMULATION_DURATION_MINUTES;

    for (int i = 1; i <= total_transactions; ++i) {
        std::string transaction = "ATM " + std::to_string(atm_id) + " Transaction " + std::to_string(i);
        SSL_write(ssl, transaction.c_str(), transaction.size());

        char buffer[1024];
        memset(buffer, 0, sizeof(buffer));
        SSL_read(ssl, buffer, sizeof(buffer));

        std::cout << "ATM " << atm_id << " received response: " << buffer << std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(60000 / TRANSACTIONS_PER_MINUTE));
    }

    SSL_shutdown(ssl);
    SSL_free(ssl);
    close(client_socket);
    std::cout << "ATM " << atm_id << " finished transactions and disconnected\n";
}

// Run Simulation
void run_atm_simulation(int num_atms) {
    SSL_CTX *ctx = create_ssl_context();
    std::vector<std::thread> atm_threads;

    for (int i = 1; i <= num_atms; ++i) {
        atm_threads.emplace_back(atm_client, i, ctx);
    }

    for (auto& t : atm_threads) {
        if (t.joinable()) t.join();
    }

    SSL_CTX_free(ctx);  // Cleanup SSL context
    std::cout << "ATM simulation completed.\n";
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        std::cout << "Usage: " << argv[0] << " <number_of_atms> <transactions_per_minute> <simulation_duration_minutes>\n";
        return 1;
    }

    // OpenSSL Global Initialization
    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();

    NUMBER_OF_ATMS = std::atoi(argv[1]);
    TRANSACTIONS_PER_MINUTE = std::atoi(argv[2]);
    SIMULATION_DURATION_MINUTES = std::atoi(argv[3]);

    run_atm_simulation(NUMBER_OF_ATMS);

    // Cleanup OpenSSL
    EVP_cleanup();

    return 0;
}







