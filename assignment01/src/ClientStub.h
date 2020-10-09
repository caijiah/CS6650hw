//
//  ClientStub.h
//  cs6650_assignment1
//

#ifndef ClientStub_h
#define ClientStub_h

#include <stdio.h>
#include <string>

#include "Order.h"
#include "RobotInfo.h"
#include "ClientSocket.h"


class ClientStub {
    ClientSocket client_socket;
public:
    // initilizez the client stub and establishes a new TCP connection
    void Init(std::string ip, int port);
    std::unique_ptr<RobotInfo> order(std::unique_ptr<Order> order);
    void disconnect();
};

#endif /* ClientStub_h */
