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

// Thread Pool Implementation
class ThreadPool {
public:
    explicit ThreadPool(size_t num_threads) : stop(false) {
        for (size_t i = 0; i < num_threads; ++i) {
            workers.emplace_back([this]() {
                while (true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(queue_mutex);
                        condition.wait(lock, [this]() { return stop || !tasks.empty(); });
                        if (stop && tasks.empty()) return;
                        task = std::move(tasks.front());
                        tasks.pop();
                    }
                    task();
                }
            });
        }
    }

    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            stop = true;
        }
        condition.notify_all();
        for (std::thread &worker : workers) {
            worker.join();
        }
    }

    void enqueue(std::function<void()> task) {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            tasks.emplace(std::move(task));
        }
        condition.notify_one();
    }

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;
};

// Update the TransactionSwitch class
class TransactionSwitch {
public:
    TransactionSwitch(std::vector<const char*> network_sim_ips, std::vector<int> network_sim_ports)
        : logger(), network_sim_ips(network_sim_ips), network_sim_ports(network_sim_ports), pool(100) {

        
        SSL_library_init();
        SSL_load_error_strings();
        OpenSSL_add_all_algorithms();
        ctx = createSSLContext();
    }
    
    ~TransactionSwitch() {
        SSL_CTX_free(ctx);
        EVP_cleanup();
    }

    void startServer() {
        int server_sock = createSocket(6667);

        std::cout << "Running TLS server on port 6667...\n";

        while (true) {
            int atm_socket = accept(server_sock, nullptr, nullptr);
            if (atm_socket < 0) {
                std::cerr << "Accept failed!" << std::endl;
                continue;
            }

            pool.enqueue([this, atm_socket]() { handleConnection(atm_socket); });
        }

        close(server_sock);
    }

private:
    std::vector<const char*> network_sim_ips;
    std::vector<int> network_sim_ports;
    TransactionLogger logger;
    SSL_CTX* ctx;
    ThreadPool pool;

    SSL_CTX* createSSLContext() {
        const SSL_METHOD* method = TLS_server_method();
        SSL_CTX* ctx = SSL_CTX_new(method);
        if (!ctx) {
            perror("Unable to create SSL context");
            exit(EXIT_FAILURE);
        }

        SSL_CTX_set_ecdh_auto(ctx, 1);

        if (SSL_CTX_use_certificate_file(ctx, "server.crt", SSL_FILETYPE_PEM) <= 0 ||
            SSL_CTX_use_PrivateKey_file(ctx, "server.key", SSL_FILETYPE_PEM) <= 0) {
            perror("Failed to load certificates");
            exit(EXIT_FAILURE);
        }

        return ctx;
    }

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

        if (listen(sock, 100) < 0) { // Allow up to 100 pending connections
            perror("Unable to listen");
            exit(EXIT_FAILURE);
        }

        return sock;
    }

    void handleConnection(int atm_socket) {
        SSL* ssl = SSL_new(ctx);
        SSL_set_fd(ssl, atm_socket);

        if (SSL_accept(ssl) <= 0) {
            ERR_print_errors_fp(stderr);
        } else {
            handleAtmRequest(ssl);
        }

        SSL_shutdown(ssl);
        SSL_free(ssl);
        close(atm_socket);
    }

    void handleAtmRequest(SSL* ssl) {
        Transaction transaction;
        logger.logTransaction(transaction);

        if (SSL_read(ssl, &transaction, sizeof(Transaction)) <= 0) {
            ERR_print_errors_fp(stderr);
            return;
        }
 
        Response response = sendNetworkMessage(transaction); // destination = messsage.cardnumber[0-3]
        SSL_write(ssl, &response, sizeof(Response));
    }

    Response sendNetworkMessage(Transaction message) {


    // get first digit
    char first_digit = message.cardNumber[0]; 
    int network = 0;
    
    if (first_digit >= '0' && first_digit <= '3') {
        network = 0;
    }
    else if (first_digit >= '4' && first_digit <= '6') {
        network = 1;
    }
    else {
        network = 2;
    }

    const char* host = network_sim_ips[network];
    int port = network_sim_ports[network];

    Response response;

    SSL_CTX *client_ctx = SSL_CTX_new(TLS_client_method());
    if (!client_ctx) {
        response.succeeded = NETWORK_ERROR;
        return response;
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        SSL_CTX_free(client_ctx);
        response.succeeded = NETWORK_ERROR;
        return response;
    }

    sockaddr_in server_addr = {};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, host, &server_addr.sin_addr) <= 0) {
        close(sock);
        SSL_CTX_free(client_ctx);
        response.succeeded = NETWORK_ERROR;
        return response;
    }

    if (connect(sock, (sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        close(sock);
        SSL_CTX_free(client_ctx);
        response.succeeded = NETWORK_ERROR;
        return response;
    }

    SSL *ssl = SSL_new(client_ctx);
    SSL_set_fd(ssl, sock);

    if (SSL_connect(ssl) <= 0) {
        SSL_free(ssl);
        close(sock);
        SSL_CTX_free(client_ctx);
        response.succeeded = NETWORK_ERROR;
        return response;
    }

    if (SSL_write(ssl, &message, sizeof(message)) <= 0) {
        SSL_free(ssl);
        close(sock);
        SSL_CTX_free(client_ctx);
        response.succeeded = NETWORK_ERROR;
        return response;
    }

    if (SSL_read(ssl, &response, sizeof(response)) <= 0) {
        SSL_free(ssl);
        close(sock);
        SSL_CTX_free(client_ctx);
        response.succeeded = NETWORK_ERROR;
        return response;
    }

    SSL_free(ssl);
    close(sock);
    SSL_CTX_free(client_ctx);
    return response;
}

};
