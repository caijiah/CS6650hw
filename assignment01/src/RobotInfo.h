//
//  RobotInfo.hpp
//  cs6650_assignment1
//
//

#ifndef RobotInfo_h
#define RobotInfo_h

#include <stdio.h>
#include "Order.h"

class RobotInfo {
    Order order;
    unsigned int engineer_id; // id of the engineer who created the robot
    int expert_id; // id of the expert who added a special module
                   // -1 indicates that there is no special module
    
public:
    RobotInfo(Order order, unsigned int engi_id, int exper_id);
    char* marshal();
    static std::unique_ptr<RobotInfo> unmarshal(char* receive);
};

#endif /* RobotInfo_h */
