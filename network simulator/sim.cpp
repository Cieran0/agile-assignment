#include <iostream>
#include "sim.hpp"
#include <thread>
#include <vector>
#include "log.hpp"

const int PORT = 8000;

void handleClient(SSL *ssl) {
    sqlite3* db = nullptr;
    if(initDatabaseConnection(db) != SQLITE_OK){
        std::cerr << "Failed to open database connection" << std::endl;
        return;
    }

    Transaction transaction;
    Response response;
    const char okResponse[] = "OK";
    int n;

    while (true) {
        std::memset(&transaction, 0, sizeof(transaction));

        n = SSL_read(ssl, &transaction, sizeof(transaction));
        if (n <= 0) {
            std::cerr << "Client Disconnected" << std::endl;
            break;
        }

        response = processTransaction(transaction, db);

        SSL_write(ssl, &response, sizeof(response));

        if(response.succeeded == 0){
            log(transaction);
        }

        SSL_write(ssl, okResponse, sizeof(okResponse));
    }

    

    SSL_free(ssl);
}

int initSSLServer(SSL_CTX *&ctx, int &sockfd, sockaddr_in &servaddr) {
    // Initialize OpenSSL
    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();

    // Create SSL context
    const SSL_METHOD *method = TLS_server_method();
    ctx = SSL_CTX_new(method);
    if (ctx == nullptr) {
        std::cerr << "Unable to create SSL context" << std::endl;
        ERR_print_errors_fp(stderr);
        return -1;
    }

    // Load server certificates
    if (SSL_CTX_use_certificate_file(ctx, "server.crt", SSL_FILETYPE_PEM) <= 0) {
        std::cerr << "Unable to load certificate" << std::endl;
        ERR_print_errors_fp(stderr);
        return -1;
    }

    if (SSL_CTX_use_PrivateKey_file(ctx, "server.key", SSL_FILETYPE_PEM) <= 0) {
        std::cerr << "Unable to load private key" << std::endl;
        ERR_print_errors_fp(stderr);
        return -1;
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        std::cerr << "Failed to create socket" << std::endl;
        return -1;
    }

    // Set SO_REUSEADDR to force unbind
    int opt = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        std::cerr << "Failed to set SO_REUSEADDR" << std::endl;
        return -1;
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    if (bind(sockfd, (sockaddr *)&servaddr, sizeof(servaddr)) != 0) {
        std::cout << "Failed to bind socket" << std::endl;
        return -1;
    }

    return 0;
}

void clientThreadFunction(SSL_CTX *ctx, int connfd) {
    // Create SSL object
    SSL *ssl = SSL_new(ctx);
    SSL_set_fd(ssl, connfd);

    // Perform SSL handshake
    if (SSL_accept(ssl) <= 0)
    {
        std::cerr << "SSL handshake failed" << std::endl;
        ERR_print_errors_fp(stderr);
        SSL_free(ssl);
        close(connfd);
        return;
    }

    handleClient(ssl);
    close(connfd);
}

int main() {
    addLogger(DatabaseLogger);
    addLogger(txtLogger);
    SSL_CTX *ctx;
    int sockfd = -1;
    sockaddr_in servaddr = {0};

    if (initSSLServer(ctx, sockfd, servaddr) != 0) {
        return -1;
    }

    listen(sockfd, 5);

    std::vector<std::thread> threads;

    while (true) {
        sockaddr_in cli = {0};
        socklen_t len = sizeof(cli);
        int connfd = accept(sockfd, (sockaddr *)&cli, &len);
        if (connfd < 0) {
            std::cerr << "Failed to accept connection" << std::endl;
            continue;
        }

        threads.emplace_back([ctx, connfd](){
            clientThreadFunction(ctx, connfd);
        });
    }

    // Clean up
    for (std::thread &thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    close(sockfd);
    SSL_CTX_free(ctx);
    EVP_cleanup();

    return 0;
}
