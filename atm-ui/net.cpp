#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#endif

#include "net.h"
#include <iostream>
#include <openssl/ssl.h>
#include <openssl/err.h>



const char *host;
int port;

void close_socket(int sock) {
    #ifdef _WIN32
    closesocket(sock);
    #else
    close(sock);
    #endif
}

bool initialize_winsock() {
    #ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed" << std::endl;
        return false;
    }
    #endif
    return true;
}

void cleanup_winsock() {
    #ifdef _WIN32
    WSACleanup();
    #endif
}

Response forwardToSocket(TranscationType type, AtmID atmID, Currency currency, AtmCurrency amount, char cardNumber[20], char expiryDate[6], char pinNo[5]) {
    Response response;

    if (!initialize_winsock()) return NETWORK;

    SSL_library_init();
    SSL_load_error_strings();
    const SSL_METHOD *method = TLS_client_method();
    SSL_CTX *ctx = SSL_CTX_new(method);

    if (!ctx) {
        std::cerr << "Failed to create SSL context" << std::endl;
        cleanup_winsock();
        return NETWORK;
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "Failed to create socket" << std::endl;
        SSL_CTX_free(ctx);
        cleanup_winsock();
        return NETWORK;
    }

    sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, host, &server_addr.sin_addr) <= 0) {
        std::cerr << "Invalid server address" << std::endl;
        close_socket(sock);
        SSL_CTX_free(ctx);
        cleanup_winsock();
        return NETWORK;
    }

    if (connect(sock, (sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Failed to connect to the server" << std::endl;
        close_socket(sock);
        SSL_CTX_free(ctx);
        cleanup_winsock();
        return NETWORK;
    }

    SSL *ssl = SSL_new(ctx);
    SSL_set_fd(ssl, sock);

    if (SSL_connect(ssl) <= 0) {
        std::cerr << "TLS handshake failed" << std::endl;
        SSL_free(ssl);
        close_socket(sock);
        SSL_CTX_free(ctx);
        cleanup_winsock();
        return NETWORK;
    }

    Transaction transaction;
    transaction.type = type;
    transaction.id = rand_uint64();
    transaction.atmID = atmID;
    transaction.currency = currency;
    transaction.amount = amount;
    strncpy(transaction.cardNumber, cardNumber, sizeof(transaction.cardNumber) - 1);
    strncpy(transaction.expiryDate, expiryDate, sizeof(transaction.expiryDate) - 1);
    strncpy(transaction.pinNo, pinNo, sizeof(transaction.pinNo) - 1);

    if (SSL_write(ssl, &transaction, sizeof(transaction)) <= 0) {
        std::cerr << "Failed to send transaction to the server" << std::endl;
        SSL_free(ssl);
        close_socket(sock);
        SSL_CTX_free(ctx);
        cleanup_winsock();
        return NETWORK;
    }

    if (SSL_read(ssl, &response, sizeof(response)) <= 0) {
        std::cerr << "Failed to receive response from the server" << std::endl;
        SSL_free(ssl);
        close_socket(sock);
        SSL_CTX_free(ctx);
        cleanup_winsock();
        return NETWORK;
    }

    SSL_free(ssl);
    close_socket(sock);
    SSL_CTX_free(ctx);
    cleanup_winsock();

    return response;
}