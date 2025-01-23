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
#include "TransactionLogr.h"
#include "Transaction.h"
#include "NetworkRouter.h"

class TransactionSwitch {
public:
    TransactionSwitch() : logger(), router() {
        SSL_library_init();
        SSL_load_error_strings();
        OpenSSL_add_all_algorithms();
        ctx = createSSLContext();
    }

    ~TransactionSwitch() {
        SSL_CTX_free(ctx);
        EVP_cleanup();
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

        if (listen(sock, 5) < 0) {
            perror("Unable to listen");
            exit(EXIT_FAILURE);
        }

        std::cout << "Listening on port " << port << "\n";
        return sock;
    }

    void startServer() {
        int server_sock_6667 = createSocket(6667);

        std::cout << "Running TLS server on port 6667...\n";

        while (true) {
            int atm_socket = accept(server_sock_6667, nullptr, nullptr);
            if (atm_socket < 0) {
                std::cerr << "Accept failed!" << std::endl;
                continue;
            }

            // Create a thread to handle the client connection
            std::thread(&TransactionSwitch::handleConnection, this, atm_socket).detach();
        }

        close(server_sock_6667);
    }

private:
    TransactionLogger logger;
    NetworkRouter router;
    SSL_CTX *ctx;
    std::mutex logger_mutex; // Mutex for thread-safe access to logger
    std::mutex router_mutex; // Mutex for thread-safe access to router

    SSL_CTX *createSSLContext() {
        const SSL_METHOD *method = TLS_server_method();
        SSL_CTX *ctx = SSL_CTX_new(method);

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

    void handleConnection(int atm_socket) {
        SSL *ssl = SSL_new(ctx);
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

    void handleAtmRequest(SSL *ssl) {
        char buffer[sizeof(Transaction)];

        while (true) {
            memset(buffer, 0, sizeof(buffer));

            int bytes = SSL_read(ssl, buffer, sizeof(Transaction));
            if (bytes <= 0) break;

            Transaction message;
            memcpy(&message, buffer, sizeof(Transaction));

            {
                std::lock_guard<std::mutex> lock(logger_mutex);
                std::cout << "Received: " << message.cardNumber << std::endl;
            }

            Response response = sendNetworkMessage(message);
            SSL_write(ssl, &response, sizeof(Response));
        }
    }

    Response sendNetworkMessage(Transaction message) {
        const int DATABASE_ERROR = 1;
        const char *host = "10.201.102.155"; // Server address
        const int port = 6668;              // Server port

        SSL_CTX *client_ctx = SSL_CTX_new(TLS_client_method());
        if (!client_ctx) {
            Response response;
            response.succeeded = DATABASE_ERROR;
            return response;
        }

        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
            SSL_CTX_free(client_ctx);
            Response response;
            response.succeeded = DATABASE_ERROR;
            return response;
        }

        sockaddr_in server_addr = {};
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        if (inet_pton(AF_INET, host, &server_addr.sin_addr) <= 0) {
            close(sock);
            SSL_CTX_free(client_ctx);
            Response response;
            response.succeeded = DATABASE_ERROR;
            return response;
        }

        if (connect(sock, (sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
            close(sock);
            SSL_CTX_free(client_ctx);
            Response response;
            response.succeeded = DATABASE_ERROR;
            return response;
        }

        SSL *ssl = SSL_new(client_ctx);
        SSL_set_fd(ssl, sock);

        if (SSL_connect(ssl) <= 0) {
            SSL_free(ssl);
            close(sock);
            SSL_CTX_free(client_ctx);
            Response response;
            response.succeeded = DATABASE_ERROR;
            return response;
        }

        if (SSL_write(ssl, &message, sizeof(message)) <= 0) {
            SSL_free(ssl);
            close(sock);
            SSL_CTX_free(client_ctx);
            Response response;
            response.succeeded = DATABASE_ERROR;
            return response;
        }

        Response response;
        if (SSL_read(ssl, &response, sizeof(response)) <= 0) {
            SSL_free(ssl);
            close(sock);
            SSL_CTX_free(client_ctx);
            response.succeeded = DATABASE_ERROR;
            return response;
        }

        SSL_free(ssl);
        close(sock);
        SSL_CTX_free(client_ctx);
        return response;
    }
};
