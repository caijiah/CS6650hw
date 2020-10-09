//
//  Order.hpp
//  cs6650_assignment1
//
//

#ifndef Order_h
#define Order_h

#include <stdio.h>
#include <memory>
#include <iostream>


class Order{
    int customer_id; // customer id
    int order_number; // # of orders issued by this customer so far
    int robot_type; //either 0-regular or 1-special
    
public:
    Order(unsigned int customer_id,
          unsigned int order_number,
          unsigned int robot_type);
    // a copy constructor
    Order(const Order &other);
    // help function for marshaling
    char* marshal();
    // help function for unmarshaling
    static std::unique_ptr<Order> unmarshal(char* byte_stream);
    bool is_expert_required();
};

#endif /* Order_h */
