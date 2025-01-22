#include <iostream>
#include <string>
#include <cstring>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

// Struct format matching the server's struct definition
struct Transaction {
    char cardNumber[20];
    char expiryDate[6];
    uint64_t atmID;
    uint64_t uniqueTransactionID;
    char pinNo[5];
    double withdrawalAmount;
};

struct Response {
    int succeeded;
    double new_balance;
};

void forwardToSocket(string cardNumber, string expiryDate, string transactionID, string atmID, string pin, double withdrawalAmount) {
    const char *host = "127.0.0.1"; // Server address
    const int port = 65431;         // Server port

    // Initialise OpenSSL
    SSL_library_init();
    SSL_load_error_strings();
    const SSL_METHOD *method = TLS_client_method();
    SSL_CTX *ctx = SSL_CTX_new(method);

    if (!ctx) {
        cerr << "Failed to create SSL context" << endl;
        return;
    }

    // Create a socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        cerr << "Failed to create socket" << endl;
        SSL_CTX_free(ctx);
        return;
    }

    // Set up the server address struct
    sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, host, &server_addr.sin_addr) <= 0) {
        cerr << "Invalid server address" << endl;
        close(sock);
        SSL_CTX_free(ctx);
        return;
    }

    // Connect to the server
    if (connect(sock, (sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        cerr << "Failed to connect to the server" << endl;
        close(sock);
        SSL_CTX_free(ctx);
        return;
    }

    // Wrap the socket with SSL
    SSL *ssl = SSL_new(ctx);
    SSL_set_fd(ssl, sock);

    if (SSL_connect(ssl) <= 0) {
        cerr << "TLS handshake failed" << endl;
        SSL_free(ssl);
        close(sock);
        SSL_CTX_free(ctx);
        return;
    }

    cout << "Connected to the server via TLS" << endl;

    // Prepare the transaction struct
    Transaction transaction;
    memset(&transaction, 0, sizeof(transaction));
    strncpy(transaction.cardNumber, cardNumber.c_str(), sizeof(transaction.cardNumber) - 1);
    strncpy(transaction.expiryDate, expiryDate.c_str(), sizeof(transaction.expiryDate) - 1);
    transaction.atmID = stoull(atmID);
    transaction.uniqueTransactionID = stoull(transactionID);
    strncpy(transaction.pinNo, pin.c_str(), sizeof(transaction.pinNo) - 1);
    transaction.withdrawalAmount = withdrawalAmount;

    // Send the transaction struct to the server
    if (SSL_write(ssl, &transaction, sizeof(transaction)) <= 0) {
        cerr << "Failed to send transaction to the server" << endl;
        SSL_free(ssl);
        close(sock);
        SSL_CTX_free(ctx);
        return;
    }

    // Receive the response struct from the server
    Response response;
    if (SSL_read(ssl, &response, sizeof(response)) <= 0) {
        cerr << "Failed to receive response from the server" << endl;
        SSL_free(ssl);
        close(sock);
        SSL_CTX_free(ctx);
        return;
    }

    // Print the response
    cout << "Transaction Response:" << endl;
    cout << "  Succeeded: " << response.succeeded << endl;
    cout << "  New Balance: " << response.new_balance << endl;

    // Clean up
    SSL_free(ssl);
    close(sock);
    SSL_CTX_free(ctx);
}

int main() {
    // Example usage
    forwardToSocket("1234567890123456", "0125", "1001", "2001", "1234", 100.0);
    return 0;
}
