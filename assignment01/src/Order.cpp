//
//  Order.cpp
//  cs6650_assignment1
//
//

#include "Order.h"
#include "string.h"

Order::Order(unsigned int c_id, unsigned int order_n, unsigned int robot_tp) {
    customer_id = c_id;
    order_number = order_n;
    robot_type = robot_tp;
}

// a copy constructor
Order::Order(const Order &other) {
    customer_id = other.customer_id;
    order_number = other.customer_id;
    robot_type = other.robot_type;
}

char* Order::marshal() {
    std::size_t int_size = sizeof(int);
    char* buffer = new char[int_size * 3];
    unsigned int net_customer_id, net_order_number, net_robot_type;
    net_customer_id = htonl(customer_id);
    net_order_number = htonl(order_number);
    net_robot_type = htonl(robot_type);
    memcpy(buffer, &net_customer_id, int_size);
    memcpy(buffer + int_size, &net_order_number, int_size);
    memcpy(buffer + int_size * 2, &net_robot_type, int_size);
    std::cout << "Order is marshalled as: " << customer_id << ", " << order_number << ", " << robot_type << std::endl;
    return buffer;
}

std::unique_ptr<Order> Order::unmarshal(char* receive) {
    unsigned int r_customer_id, r_order_number, r_robot_type;
    unsigned int h_customer_id, h_order_number, h_robot_type;
    std::size_t int_size = sizeof(int);
    memcpy(&r_customer_id, receive, int_size);
    memcpy(&r_order_number, receive + int_size, int_size);
    memcpy(&r_robot_type, receive + int_size * 2, int_size);
    h_customer_id = ntohl(r_customer_id);
    h_order_number = ntohl(r_order_number);
    h_robot_type = ntohl(r_robot_type);
    std::cout << "Order is unmarshalled as: " << h_customer_id << ", " << h_order_number << ", " << h_robot_type << std::endl;
    return std::unique_ptr<Order>(new Order(h_customer_id, h_order_number, h_robot_type));
}


bool Order::is_expert_required() {
    return robot_type == 1;
}
