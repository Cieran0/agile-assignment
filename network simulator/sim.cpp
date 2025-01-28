#include <iostream>
#include "sim.hpp"
#include <thread>
#include <vector>
#include <mutex>
#include <csignal>
#include <atomic>
#include <condition_variable>
#include <queue>
#include "log.hpp"
#include "sim.hpp"
#include <fstream>
#include "Conversion.hpp"

const int PORT = 6668;
std::atomic<bool> serverRunning(true);
int sockfd = -1;

// Thread-safe OpenSSL setup
#include <pthread.h>
static pthread_mutex_t *mutex_buf = nullptr;

static void locking_function(int mode, int n, const char *file, int line) {
    if (mode & CRYPTO_LOCK)
        pthread_mutex_lock(&mutex_buf[n]);
    else
        pthread_mutex_unlock(&mutex_buf[n]);
}

static void thread_setup() {
    mutex_buf = (pthread_mutex_t *)malloc(CRYPTO_num_locks() * sizeof(pthread_mutex_t));
    for (int i = 0; i < CRYPTO_num_locks(); i++)
        pthread_mutex_init(&mutex_buf[i], NULL);
    CRYPTO_set_locking_callback(locking_function);
}

static void thread_cleanup() {
    CRYPTO_set_locking_callback(NULL);
    for (int i = 0; i < CRYPTO_num_locks(); i++)
        pthread_mutex_destroy(&mutex_buf[i]);
    free(mutex_buf);
}

// Signal handler for graceful shutdown
void handleSignal(int signal) {
    if (signal == SIGINT) {
        std::cout << "Shutting down server..." << std::endl;
        serverRunning = false;
        close(sockfd); // Close the main socket to stop accepting new connections
    }
}

void handleClient(SSL *ssl) {
    std::cout << "Handling new client connection" << std::endl;
    sqlite3* db = nullptr;
    if (initDatabaseConnection(db) != SQLITE_OK) {
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

        // std::cout << "Response {" << std::endl;
        // std::cout << "\tsucceeded: " << response.succeeded << std::endl;
        // std::cout << "\tnew_balance: " << response.new_balance << std::endl;
        // std::cout << "}" << std::endl;

        if (response.succeeded == 0) {
            log(transaction);
        }

        SSL_write(ssl, okResponse, sizeof(okResponse));
    }

    sqlite3_close(db);
    SSL_free(ssl);
    std::cout << "Client connection closed" << std::endl;
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
        std::cerr << "Failed to bind socket" << std::endl;
        return -1;
    }

    return 0;
}

void clientThreadFunction(SSL_CTX *ctx, int connfd) {
    SSL *ssl = SSL_new(ctx);
    if (!ssl) {
        std::cerr << "Failed to create SSL object" << std::endl;
        close(connfd);
        return;
    }

    SSL_set_fd(ssl, connfd);

    if (SSL_accept(ssl) <= 0) {
        std::cerr << "SSL handshake failed" << std::endl;
        ERR_print_errors_fp(stderr);
        SSL_free(ssl);
        close(connfd);
        return;
    }

    handleClient(ssl);
    //SSL_free(ssl);
    close(connfd);
}

std::mutex mtx;
std::condition_variable cv;
std::queue<int> clientQueue;
const int MAX_THREADS = 10; // Adjust based on system capacity

void threadPoolWorker(SSL_CTX *ctx) {
    while (serverRunning) {
        int connfd;
        {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [] { return !clientQueue.empty() || !serverRunning; });
            if (!serverRunning && clientQueue.empty()) break;
            connfd = clientQueue.front();
            clientQueue.pop();
        }
        clientThreadFunction(ctx, connfd);
    }
}

int main() {
    // Set up signal handling for graceful shutdown
    std::signal(SIGINT, handleSignal);

    // Initialize thread-safe OpenSSL
    thread_setup();

    log_txt.open("sim_log.txt", std::ios::app);
    addLogger(DatabaseLogger);
    addLogger(ConsoleLogger);
    addLogger(txtLogger);

    SSL_CTX *ctx;
    sockaddr_in servaddr = {0};

    if (initSSLServer(ctx, sockfd, servaddr) != 0) {
        thread_cleanup();
        return -1;
    }

    if (listen(sockfd, 5) != 0) {
        std::cerr << "Failed to listen on socket" << std::endl;
        close(sockfd);
        SSL_CTX_free(ctx);
        thread_cleanup();
        return -1;
    }

    std::vector<std::thread> threadPool;
    for (int i = 0; i < MAX_THREADS; ++i) {
        threadPool.emplace_back(threadPoolWorker, ctx);
    }

    while (serverRunning) {
        sockaddr_in cli = {0};
        socklen_t len = sizeof(cli);
        int connfd = accept(sockfd, (sockaddr *)&cli, &len);
        if (connfd < 0) {
            if (serverRunning) {
                std::cerr << "Failed to accept connection" << std::endl;
            }
            continue;
        }

        {
            std::lock_guard<std::mutex> lock(mtx);
            clientQueue.push(connfd);
        }
        cv.notify_one();
    }

    // Clean up
    cv.notify_all(); // Wake up all threads
    for (std::thread &thread : threadPool) {
        if (thread.joinable()) {
            thread.join();
        }
    }


    close(sockfd);
    SSL_CTX_free(ctx);
    EVP_cleanup();
    thread_cleanup();

    std::cout << "Server shutdown complete." << std::endl;
    return 0;
}