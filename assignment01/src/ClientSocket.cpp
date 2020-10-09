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
        std::cerr << "ClientSocket: Fail to create a socket" << std::endl;
        exit(1);
    }
    memset(&servinfo, '\0', sizeof(servinfo));
    servinfo.sin_family = AF_INET;
    servinfo.sin_port = htons(port);
    servinfo.sin_addr.s_addr = inet_addr(ip.c_str());
}

ClientSocket::~ClientSocket() {
    close(sockfd);
}

void ClientSocket::cli_sock_connect() {
    if (connect(sockfd, (struct sockaddr *) &servinfo, sizeof(servinfo)) != 0) {
        std::cerr << "ClientSocket: Fail to connect." << std::endl;
        exit(1);
    }
}

void ClientSocket::cli_sock_close() {
    close(sockfd);
}


void ClientSocket::send_msg(char* buffer, std::size_t size) {
    if (send(sockfd, buffer, size, 0) == -1) {
        std::cerr << "ClientSocket: Fail to send." << std::endl;
        close(sockfd);
        exit(1);
    }
}

int ClientSocket::recv_msg(char* buffer, std::size_t size) {
    int rv;
    if ((rv = recv(sockfd, buffer, size, 0)) == -1) {
        std::cerr << "ClientSocket: Fail to receive." << std::endl;
        close(sockfd);
        exit(1);
    }
    return rv;
}
