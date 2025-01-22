#include <openssl/ssl.h>
#include <openssl/err.h>
#include <iostream>
#include <cstring>
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

    struct SSLConnection {
        SSL *ssl;
        int sock;
    };

    SSLConnection connectToNetwork(int port) {
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
            std::cerr << "Failed to create socket\n";
            return {nullptr, -1};
        }

        sockaddr_in serv_addr;
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(port);

        if (inet_pton(AF_INET, "10.201.102.155", &serv_addr.sin_addr) <= 0) {
            std::cerr << "Invalid address\n";
            close(sock);
            return {nullptr, -1};
        }

        if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
            std::cerr << "Failed to connect to Network\n";
            close(sock);
            return {nullptr, -1};
        }

        SSL *ssl = SSL_new(ctx);
        SSL_set_fd(ssl, sock);

        if (SSL_connect(ssl) != 1) {
            std::cerr << "Failed to establish SSL connection to Network\n";
            ERR_print_errors_fp(stderr);
            SSL_free(ssl);
            close(sock);
            return {nullptr, -1};
        }

        return {ssl, sock};
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

        close(server_sock_6667);
    }

private:
    TransactionLogger logger;
    NetworkRouter router;
    SSL_CTX *ctx;

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

    void handleAtmRequest(SSL *ssl) {
        char buffer[sizeof(Transaction)];

        while (true) {
            memset(buffer, 0, sizeof(buffer));

            int bytes = SSL_read(ssl, buffer, sizeof(Transaction));
            if (bytes <= 0) break;

            Transaction message;
            memcpy(&message, buffer, sizeof(Transaction));

            std::cout << "Received: " << message.cardNumber << std::endl;

            Response response = sendNetworkMessage(message);       


            
                SSL_write(ssl, &response, sizeof(Response));
            

        }
    }

    Response sendNetworkMessage(Transaction message){
        const int DATABASE_ERROR = 1;
const char *host = "10.201.102.155"; // Server address
    const int port = 6668;         // Server port

    // Initialise OpenSSL
    SSL_library_init();
    SSL_load_error_strings();
    const SSL_METHOD *method = TLS_client_method();
    SSL_CTX *ctx = SSL_CTX_new(method);

    if (!ctx) {
        //cerr << "Failed to create SSL context" << endl;
        Response response;
        response.succeeded = DATABASE_ERROR;
        return response;
    }

    // Create a socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        //cerr << "Failed to create socket" << endl;
        SSL_CTX_free(ctx);
        Response response;

        response.succeeded = DATABASE_ERROR;
        return response;
    }

    // Set up the server address struct
    sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, host, &server_addr.sin_addr) <= 0) {
        //cerr << "Invalid server address" << endl;
        close(sock);
        SSL_CTX_free(ctx);
        Response response;

        response.succeeded = DATABASE_ERROR;
        return response;
    }

    // Connect to the server
    if (connect(sock, (sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        //cerr << "Failed to connect to the server" << endl;
        close(sock);
        SSL_CTX_free(ctx);
        Response response;

        response.succeeded = DATABASE_ERROR;
        return response;
    }

    // Wrap the socket with SSL
    SSL *ssl = SSL_new(ctx);
    SSL_set_fd(ssl, sock);

    if (SSL_connect(ssl) <= 0) {
        //cerr << "TLS handshake failed" << endl;
        SSL_free(ssl);
        close(sock);
        SSL_CTX_free(ctx);
        Response response;

        response.succeeded = DATABASE_ERROR;
        return response;
    }

    //cout << "Connected to the server via TLS" << endl;

    // Prepare the transaction struct

    // Send the transaction struct to the server
    if (SSL_write(ssl, &message, sizeof(message)) <= 0) {
        //cerr << "Failed to send transaction to the server" << endl;
        SSL_free(ssl);
        close(sock);
        SSL_CTX_free(ctx);
        Response response;

        response.succeeded = DATABASE_ERROR;
        return response;
    }

    // Receive the response struct from the server
    Response response;
    if (SSL_read(ssl, &response, sizeof(response)) <= 0) {
        //cerr << "Failed to receive response from the server" << endl;
        SSL_free(ssl);
        close(sock);
        SSL_CTX_free(ctx);
        response.succeeded = DATABASE_ERROR;
        return response;
    }

    // Print the response
    //cout << "Transaction Response:" << endl;
    //cout << "  Succeeded: " << response.succeeded << endl;
    //cout << "  New Balance: " << response.new_balance << endl;

    // Clean up
    SSL_free(ssl);
    close(sock);
    SSL_CTX_free(ctx);
    std::cout << "Response sent"<<std::endl;
    return response;

    }
};
