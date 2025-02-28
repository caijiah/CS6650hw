//
//  ServerStub.h
//  cs6650_assignment1
//
//

#ifndef ServerStub_h
#define ServerStub_h

#include <stdio.h>
#include <sys/socket.h>

#include "Order.h"
#include "RobotInfo.h"
#include "ServerSocket.h"

class ServerStub{
    int ser_sock;
public:
    ServerStub();
    void Init(int socket_fd);
    ~ServerStub();
    std::unique_ptr<Order> ReceiveOrder();
    void ShipRobot(RobotInfo robot_info);
    void sendErrorMsg(char* msg);
    void disconnect();
};

#endif /* ServerStub_h */
