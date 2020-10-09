//
//  ServerSocket.cpp
//  cs6650_assignment1
//
//  Created by 蔡佳豪 on 2020/10/4.
//  Copyright © 2020 Jiahao Cai. All rights reserved.
//

#include "ServerSocket.h"

#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

ServerSocket::ServerSocket(int given_port) {
    sockfd =  socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cerr << "ServerSocket: Fail to create a socket." << std::endl;
    }
    
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET; // IPv4
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(given_port); // converts host byte order to network byte order
    
    // bind it to given port
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
             sizeof(serv_addr)) == -1) {
        close(sockfd);
        std::cerr << "ClientSocket: Fail to bind." << std::endl;
    }
    
    if (listen(sockfd, SOMAXCONN) == -1) {
        std::cerr << "ClientSocket: Fail to listen." << std::endl;
        exit(1);
    }
};


ServerSocket::~ServerSocket() {
    close(sockfd);
};

int ServerSocket::ser_sock_accept() {
    struct sockaddr_in client_addr;
    socklen_t sin_size = sizeof(client_addr);
    int new_fd = accept(sockfd, (struct sockaddr *)&client_addr, &sin_size);
    if (new_fd == -1) {
        std::cerr << "ClientSocket: Fail to accept." << std::endl;
        exit(1);
    }
    return new_fd;
};
