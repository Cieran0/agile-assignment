#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/crypto.h>

struct Transaction {
    char cardNumber[20];
    char expiryDate[6];
    uint64_t atmID;
    uint64_t uniqueTransactionID;
    char pinNo[5];
    double withdrawalAmount;
};

const int PORT = 8000;

void handleClient(SSL *ssl) {
    char buff[2048];
    int n;

    while (true) {
        std::memset(buff, 0, 2048);

        n = SSL_read(ssl, buff, sizeof(buff));
        if (n <= 0) {
            std::cerr << "SSL read error" << std::endl;
            break;
        }

        std::cout << "received: " << buff << std::endl;

        SSL_write(ssl, buff, n);
    }
}

int main() {
    SSL_CTX *ctx;
    SSL *ssl;
    int sockfd, connfd;
    sockaddr_in servaddr = {0};
    sockaddr_in cli = {0};

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
        std::cout << "Failed to create Socket" << std::endl;
        return -1;
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    if (bind(sockfd, (sockaddr*)&servaddr, sizeof(servaddr)) != 0) {
        std::cout << "Failed to bind Socket" << std::endl;
        return -1;
    }

    listen(sockfd, 5);

    socklen_t len = sizeof(cli);
    connfd = accept(sockfd, (sockaddr*)&cli, &len);

    // Create SSL object
    ssl = SSL_new(ctx);
    SSL_set_fd(ssl, connfd);

    // Perform SSL handshake
    if (SSL_accept(ssl) <= 0) {
        std::cerr << "SSL handshake failed" << std::endl;
        ERR_print_errors_fp(stderr);
        return -1;
    }

    handleClient(ssl);

    // Clean up
    SSL_free(ssl);
    close(connfd);
    close(sockfd);
    SSL_CTX_free(ctx);
    EVP_cleanup();

    return 0;
}
