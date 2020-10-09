//
//  RobotInfo.cpp
//  cs6650_assignment1
//
//

#include "RobotInfo.h"
#include <string.h>
#include <iostream>
#include <memory>

RobotInfo::RobotInfo(Order order, unsigned int engi_id, int exper_id) : order(order) {
    this->order = order;
    engineer_id = engi_id;
    expert_id = exper_id;
}

char* RobotInfo::marshal() {
    std::size_t size_of_int = sizeof(int);
    char* buffer = new char[size_of_int * 5];
    // get marshal directly from order
    char* marshalled_order = order.marshal();
    memcpy(buffer, marshalled_order, size_of_int * 3);
    delete marshalled_order;
    // marshal fields in RobotInfo
    unsigned int net_engineer_id;
    int net_expert_id;
    net_engineer_id = htonl(engineer_id);
    net_expert_id = htonl(expert_id);
    memcpy(buffer + size_of_int * 3, &net_engineer_id, size_of_int);
    memcpy(buffer + size_of_int * 4, &net_expert_id, size_of_int);
    return buffer;
}

std::unique_ptr<RobotInfo> RobotInfo::unmarshal(char* receive) {
    std::size_t int_size = sizeof(int);
    // unmarshal order
    std::unique_ptr<Order> order_p;
    char marshalled_order[int_size * 3];
    memcpy(marshalled_order, receive, int_size * 3);
    order_p = Order::unmarshal(marshalled_order);
    unsigned int r_engineer_id, h_engineer_id;
    int r_expert_id, h_expert_id;
    memcpy(&r_engineer_id, receive + int_size * 3, int_size);
    memcpy(&r_expert_id, receive + int_size * 4, int_size);
    h_engineer_id = ntohl(r_engineer_id);
    h_expert_id = ntohl(r_expert_id);
    std::cout << "RobotInfo is unmarshalled as: " << h_engineer_id << ", " << h_expert_id << std::endl;
    return std::unique_ptr<RobotInfo>(new RobotInfo(*order_p, r_engineer_id, r_expert_id));
}
