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

uint64_t rand_uint64() {
    srand((unsigned int)time(NULL));
    uint64_t r = 0;
    for (int i = 0; i < 64; i += 15) {
        r = r * ((uint64_t)RAND_MAX + 1) + rand();
    }
    // Ensure the value is less than or equal to int64_t max (2^63 - 1)
    return r % ((uint64_t)1 << 63);
}

const char *host;
int port;

// Helper function to close the socket
void close_socket(int sock) {
    #ifdef _WIN32
    closesocket(sock);
    #else
    close(sock);
    #endif
}

// Helper function to initialize Winsock on Windows
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

// Helper function to clean up Winsock on Windows
void cleanup_winsock() {
    #ifdef _WIN32
    WSACleanup();
    #endif
}

#define NO_NET

Response forwardToSocket(std::string cardNumber, std::string expiryDate, uint64_t atmID, std::string pin, double withdrawalAmount) {
    Response response;

	#ifdef NO_NET
		response.new_balance = 1000;
		response.succeeded = TRANSACTION_SUCESS;
		return response;
	#endif

    if (!initialize_winsock()) {
        return NETWORK_ERROR;
    }

    SSL_library_init();
    SSL_load_error_strings();
    const SSL_METHOD *method = TLS_client_method();
    SSL_CTX *ctx = SSL_CTX_new(method);

    if (!ctx) {
        std::cerr << "Failed to create SSL context" << std::endl;
        cleanup_winsock();
        return NETWORK_ERROR;
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "Failed to create socket" << std::endl;
        SSL_CTX_free(ctx);
        cleanup_winsock();
        return NETWORK_ERROR;
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
        return NETWORK_ERROR;
    }

    if (connect(sock, (sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Failed to connect to the server" << std::endl;
        close_socket(sock);
        SSL_CTX_free(ctx);
        cleanup_winsock();
        return NETWORK_ERROR;
    }

    SSL *ssl = SSL_new(ctx);
    SSL_set_fd(ssl, sock);

    if (SSL_connect(ssl) <= 0) {
        std::cerr << "TLS handshake failed" << std::endl;
        SSL_free(ssl);
        close_socket(sock);
        SSL_CTX_free(ctx);
        cleanup_winsock();
        return NETWORK_ERROR;
    }

    Transaction transaction = {0};

    strncpy(transaction.cardNumber, cardNumber.c_str(), sizeof(transaction.cardNumber) - 1);
    strncpy(transaction.expiryDate, expiryDate.c_str(), sizeof(transaction.expiryDate) - 1);
    strncpy(transaction.pinNo, pin.c_str(), sizeof(transaction.pinNo) - 1);

    transaction.atmID = atmID;
    transaction.uniqueTransactionID = rand_uint64();
    transaction.withdrawalAmount = withdrawalAmount;

    if (SSL_write(ssl, &transaction, sizeof(transaction)) <= 0) {
        std::cerr << "Failed to send transaction to the server" << std::endl;
        SSL_free(ssl);
        close_socket(sock);
        SSL_CTX_free(ctx);
        cleanup_winsock();
        return NETWORK_ERROR;
    }

    if (SSL_read(ssl, &response, sizeof(response)) <= 0) {
        std::cerr << "Failed to receive response from the server" << std::endl;
        SSL_free(ssl);
        close_socket(sock);
        SSL_CTX_free(ctx);
        cleanup_winsock();
        return NETWORK_ERROR;
    }

    SSL_free(ssl);
    close_socket(sock);
    SSL_CTX_free(ctx);
    cleanup_winsock();

    return response;
}