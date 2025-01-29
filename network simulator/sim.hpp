#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/crypto.h>
#include <atomic>

void handleClient(SSL* ssl);
int initSSLServer(SSL_CTX *&ctx, int &sockfd, sockaddr_in &servaddr);
void clientThreadFunction(SSL_CTX *ctx, int connfd);


extern std::atomic<bool> serverRunning;