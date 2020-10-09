//
//  ClientStub.cpp
//  cs6650_assignment1
//

#include "ClientStub.h"
#include "ClientSocket.h"
#include "RobotInfo.h"

#include <stdio.h>

// initilizez the client stub and establishes a new TCP connection
ClientStub::ClientStub(std::string ip, int port) : client_socket(ip, port) {
    client_socket.cli_sock_connect();
}

std::unique_ptr<RobotInfo> ClientStub::order(std::unique_ptr<Order> order) {
    // order details should be marshalled into a byte stream
    char * marshalled_order = order->marshal();
    
    // The byte stream is then sent through a socket connection to the server.
    client_socket.send_msg(marshalled_order, sizeof(sizeof(int) * 3));
    delete marshalled_order;
    // Next, the function should wait for a server response.
    size_t buff_size = sizeof(int) * 5;
    char buff[buff_size];
    client_socket.recv_msg(buff, buff_size);
    // Once the server responds back with robot information in a byte stream
    // format, the byte stream should be unmarshalled to robot information
    // and returned.
    std::unique_ptr<RobotInfo> robot = RobotInfo::unmarshal(buff);
    return robot;
}

void ClientStub::disconnect() {
    client_socket.cli_sock_close();   
}
