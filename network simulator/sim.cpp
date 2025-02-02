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
#include <filesystem>
#include "Conversion.hpp"

int PORT = 6668;
std::atomic<bool> serverRunning(true);
int sockfd = -1;
char* db_file;

// Signal handler for graceful shutdown
void handleSignal(int signal) {
    if (signal == SIGINT) {
        std::cout << "Shutting down server..." << std::endl;
        serverRunning = false;
        close(sockfd); // Close the main socket to stop accepting new connections
    }
}

void handleClient(SSL *ssl) {

    Transaction transaction;
    Response response;

    while (true) {
        std::memset(&transaction, 0, sizeof(transaction));

        int n = SSL_read(ssl, &transaction, sizeof(transaction));
        if (n <= 0) {
            break;
        }

        std::future<Response> responseFuture = enqueueTransaction(transaction);

        Response response = responseFuture.get();

        SSL_write(ssl, &response, sizeof(response));

        if (response.succeeded == 0) {
            log(transaction);
        }
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

void joinThreads(std::vector<std::thread>& threadPool) {
    queueCondition.notify_one();

    const int maxRetries = 10;
    const std::chrono::milliseconds retryInterval(100);  // Retry interval (100ms)
    
    for (std::thread& thread : threadPool) {
        if (thread.joinable()) {
            int retryCount = 0;
            while (retryCount < maxRetries && thread.joinable()) {
                thread.join();
                std::this_thread::sleep_for(retryInterval);  // Wait before retrying
                retryCount++;
            }

            // If thread is still joinable after retries, log a warning
            if (retryCount == maxRetries && thread.joinable()) {
                std::cerr << "Warning: thread didn't finish in time, force joining!" << std::endl;
                thread.join();  // Forcefully join the thread if it doesn't finish
            }
        }
    }

    std::cout << "All threads joined!" << std::endl;
}

bool isInteger(const char* str, int& value) {
    if (str == nullptr || *str == '\0') return false;

    char* end;
    long result = std::strtol(str, &end, 10);


    if (*end != '\0') return false;


    if (result < INT_MIN  || result > INT_MAX) return false;

    value = static_cast<int>(result);
    return true;
}

int main(int argc, char* argv[]) {
    if(argc != 3){
        std::cerr << "Please provide port and db file" << std::endl;
        return 1;
    }

    if(!isInteger(argv[1], PORT)){
        std::cerr << "PORT is not an integer" << std::endl;
        return 1;
    }

    db_file = argv[2];
    if(!std::filesystem::exists(db_file)){
        std::cerr << "Database does not exist" << std::endl;
        return 1;
    }

    std::signal(SIGINT, handleSignal);

    log_txt.open("sim_log.txt", std::ios::app);
    addLogger(DatabaseLogger);
    addLogger(ConsoleLogger);
    addLogger(txtLogger);

    SSL_CTX *ctx;
    sockaddr_in servaddr = {0};

    if (initSSLServer(ctx, sockfd, servaddr) != 0) {
        return -1;
    }

    if (listen(sockfd, 200) != 0) {
        std::cerr << "Failed to listen on socket" << std::endl;
        close(sockfd);
        SSL_CTX_free(ctx);
        return -1;
    }

    std::vector<std::thread> threadPool;
    for (int i = 0; i < MAX_THREADS - 1; ++i) {
        threadPool.emplace_back(threadPoolWorker, ctx);
    }

    threadPool.emplace_back(processTransactionQueue);

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
    joinThreads(threadPool);


    close(sockfd);
    SSL_CTX_free(ctx);
    EVP_cleanup();

    std::cout << "Server shutdown complete." << std::endl;
    return 0;
}
