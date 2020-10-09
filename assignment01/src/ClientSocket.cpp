//
//  ClientSocket.cpp
//  cs6650_assignment1
//
//

#include "ClientSocket.h"
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

ClientSocket::ClientSocket(std::string ip, int port) {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("server: socket");
        exit(1);
    }
    std::cout << "in socket creation" << std::endl;
    memset(&servinfo, '\0', sizeof(servinfo));
std::cout << "in socket creation2" << std::endl;
    servinfo.sin_family = AF_INET;
std::cout << "in socket creation3" << std::endl;
    servinfo.sin_port = htons(port);
std::cout << "in socket creation4" << std::endl;
    servinfo.sin_addr.s_addr = inet_addr(ip.c_str());
std::cout << "in socket creation5" << std::endl;
}

ClientSocket::~ClientSocket() {
    close(sockfd);
}

void ClientSocket::cli_sock_connect() {
    std::cout << "before connect" << std::endl;
    if (connect(sockfd, (struct sockaddr *) &servinfo, sizeof(servinfo)) != 0) {
        perror("client: connect");
        exit(1);
    }
}

void ClientSocket::cli_sock_close() {
    close(sockfd);
}


void ClientSocket::send_msg(char* buffer, std::size_t size) {
    if (send(sockfd, buffer, size, 0) == -1) {
        perror("client: send");
        close(sockfd);
        exit(1);
    }
}

int ClientSocket::recv_msg(char* buffer, std::size_t size) {
    int rv;
    if ((rv = recv(sockfd, buffer, size, 0)) == -1) {
        perror("client: recv");
        close(sockfd);
        exit(1);
    }
    return rv;
}
