//
//  ServerStub.cpp
//  cs6650_assignment1
//
//

#include "ServerStub.h"
#include "string.h"
#include <unistd.h>
#include <sys/socket.h>
#include <memory>

ServerStub::ServerStub() {}

// takes a connect socket fd
void ServerStub::Init(int socket_fd) {
    ser_sock = socket_fd;
}

ServerStub::~ServerStub() {
    close(ser_sock);
}

std::unique_ptr<Order> ServerStub::ReceiveOrder() {
    std::size_t order_size = sizeof(int) * 3; // since 3 fields in an order
    char buffer[order_size];
    memset(&buffer, '\0', sizeof(buffer));
    int numbytes = -1;
    // waits for client order to arrive through the socket connection
    // number of bytes received.
    while (numbytes < 0) {
        numbytes = recv(ser_sock, buffer, order_size, 0);
    }
    if (numbytes == 0) {
        throw "Client disconnected";
    }
    // unmarshalled to an order and should be returned.
    return Order::unmarshal(buffer);
}


void ServerStub::ShipRobot(RobotInfo robot_info) {
    char* marshalled_robot_info = robot_info.marshal();
    // sends the robot information to the client who ordered it.
    send(ser_sock, marshalled_robot_info, sizeof(int) * 5, 0);
    delete marshalled_robot_info;
}

void ServerStub::sendErrorMsg(char* msg) {
    send(ser_sock, msg, (int) strlen(msg), 400);
    delete msg;
}

void ServerStub::disconnect() {
    close(ser_sock);
}
