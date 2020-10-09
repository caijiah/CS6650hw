//
//  ClientStub.cpp
//  cs6650_assignment1
//

#include "ClientStub.h"
#include "ClientSocket.h"
#include "RobotInfo.h"

// initilizez the client stub and establishes a new TCP connection
void ClientStub::Init(std::string ip, int port) {
    client_socket = ClientSocket(ip, port);
    client_socket.cli_sock_connect();
}
std::unique_ptr<RobotInfo> ClientStub::order(std::unique_ptr<Order> order) {
    // order details should be marshalled into a byte stream
    char * marshalled_order = order->marshal();
    
    // The byte stream is then sent through a socket connection to the server.
    client_socket.send_msg(marshalled_order, sizeof(marshalled_order));
    delete marshalled_order;
    // Next, the function should wait for a server response.
    char buff[sizeof(int) * 5];
    client_socket.recv_msg(buff);
    // Once the server responds back with robot information in a byte stream
    // format, the byte stream should be unmarshalled to robot information
    // and returned.
    std::unique_ptr<RobotInfo> robot = RobotInfo::unmarshal(buff);
    return robot;
}

void ClientStub::disconnect() {
    
}
