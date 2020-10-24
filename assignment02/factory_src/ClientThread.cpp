#include "ClientThread.h"
#include "Messages.h"

#include <iostream>

ClientThreadClass::ClientThreadClass() {}

void ClientThreadClass::ThreadBody(std::string ip, int port, int id, int orders, int type) {
	customer_id = id;
	num_orders = orders;
	robot_type = type;
	if (!stub.Init(ip, port)) {
		std::cout << "Thread " << customer_id << " failed to connect" << std::endl;
		return;
	}
	for (int i = 0; i < num_orders; i++) {
		CustomerRequest crq;
		RobotInfo robot;
		crq.SetRequest(customer_id, i, robot_type);

		timer.Start();
		robot = stub.Order(crq);
		timer.EndAndMerge();

		if (!robot.IsValid()) {
			std::cout << "Invalid robot " << customer_id << std::endl;
			break;	
		} 
	}
}

ClientTimer ClientThreadClass::GetTimer() {
	return timer;	
}

