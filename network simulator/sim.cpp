#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>

struct Transaction{
    char cardNumber[20];
    char expiryDate[6];
    uint64_t atmID;
    uint64_t uniqueTransactionID;
    char pinNo[5];
    double withdrawalAmount;
};

const int PORT = 8000;

void handleClient(int connfd){
    char buff[2048];
    int n;

    while(true){
        std::memset(buff, 0, 2048);

        n = read(connfd, buff, sizeof(buff));

        std::cout << "recieved: " << buff << std::endl;
        
        write(connfd, buff, n);
    }
}

int main(){
    int sockfd, connfd;
    sockaddr_in servaddr ={0};
    sockaddr_in cli = {0};

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd == -1){
        std::cout << "Failed to create Socket" << std::endl;
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    if(bind(sockfd, (sockaddr*)&servaddr, sizeof(servaddr)) != 0) {
        std::cout << "Failed to bind Socket" << std::endl;
    }

    listen(sockfd, 5);

    socklen_t len = sizeof(cli);

    connfd = accept(sockfd, (sockaddr*)&cli, &len);

    handleClient(connfd);

    return 0;
}
