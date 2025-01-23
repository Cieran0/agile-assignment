#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#endif

#define TRANSACTION_SUCESS 0
#define INSUFFICIENT_FUNDS 1
#define DATABASE_ERROR 2
#define INCORRECT_PIN 3

#include "net.h"
#include <iostream>
#include <openssl/ssl.h>
#include <openssl/err.h>

uint64_t rand_uint64(void) {
  uint64_t r = 0;
  for (int i=0; i<64; i += 15 /*30*/) {
    r = r*((uint64_t)RAND_MAX + 1) + rand();
  }
  return r;
}

Response forwardToSocket(std::string cardNumber, std::string expiryDate, std::string atmID, std::string pin, double withdrawalAmount) {
    const char *host = "10.201.102.155";
    const int port = 6668;

#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed" << std::endl;
        Response response;
        response.succeeded = DATABASE_ERROR;
        return response;
    }
#endif

    SSL_library_init();
    SSL_load_error_strings();
    const SSL_METHOD *method = TLS_client_method();
    SSL_CTX *ctx = SSL_CTX_new(method);

    if (!ctx) {
        std::cerr << "Failed to create SSL context" << std::endl;
        Response response;
        response.succeeded = DATABASE_ERROR;
        return response;
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "Failed to create socket" << std::endl;
        SSL_CTX_free(ctx);
        Response response;
        response.succeeded = DATABASE_ERROR;
        return response;
    }

    sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, host, &server_addr.sin_addr) <= 0) {
        std::cerr << "Invalid server address" << std::endl;
#ifdef _WIN32
        closesocket(sock);
#else
        close(sock);
#endif
        SSL_CTX_free(ctx);
        Response response;
        response.succeeded = DATABASE_ERROR;
        return response;
    }

    if (connect(sock, (sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Failed to connect to the server" << std::endl;
#ifdef _WIN32
        closesocket(sock);
#else
        close(sock);
#endif
        SSL_CTX_free(ctx);
        Response response;
        response.succeeded = DATABASE_ERROR;
        return response;
    }

    SSL *ssl = SSL_new(ctx);
    SSL_set_fd(ssl, sock);

    if (SSL_connect(ssl) <= 0) {
        std::cerr << "TLS handshake failed" << std::endl;
        SSL_free(ssl);
#ifdef _WIN32
        closesocket(sock);
#else
        close(sock);
#endif
        SSL_CTX_free(ctx);
        Response response;
        response.succeeded = DATABASE_ERROR;
        return response;
    }

    std::cout << "Connected to the server via TLS" << std::endl;

    Transaction transaction;
    memset(&transaction, 0, sizeof(transaction));
    strncpy(transaction.cardNumber, cardNumber.c_str(), sizeof(transaction.cardNumber) - 1);
    strncpy(transaction.expiryDate, expiryDate.c_str(), sizeof(transaction.expiryDate) - 1);
    transaction.atmID = stoull(atmID);
    transaction.uniqueTransactionID = rand_uint64();
    strncpy(transaction.pinNo, pin.c_str(), sizeof(transaction.pinNo) - 1);
    transaction.withdrawalAmount = withdrawalAmount;

    if (SSL_write(ssl, &transaction, sizeof(transaction)) <= 0) {
        std::cerr << "Failed to send transaction to the server" << std::endl;
        SSL_free(ssl);
#ifdef _WIN32
        closesocket(sock);
#else
        close(sock);
#endif
        SSL_CTX_free(ctx);
        Response response;
        response.succeeded = DATABASE_ERROR;
        return response;
    }

    std::cout << "Withdrew: " << transaction.withdrawalAmount << std::endl;

    Response response;
    if (SSL_read(ssl, &response, sizeof(response)) <= 0) {
        std::cerr << "Failed to receive response from the server" << std::endl;
        SSL_free(ssl);
#ifdef _WIN32
        closesocket(sock);
#else
        close(sock);
#endif
        SSL_CTX_free(ctx);
        response.succeeded = DATABASE_ERROR;
        return response;
    }

    std::cout << "Transaction Response:" << std::endl;
    std::cout << "  Succeeded: " << response.succeeded << std::endl;
    std::cout << "  New Balance: " << response.new_balance << std::endl;

    SSL_free(ssl);
#ifdef _WIN32
    closesocket(sock);
    WSACleanup();
#else
    close(sock);
#endif
    SSL_CTX_free(ctx);

    return response;
}