#include "ClientThread.h"
#include "Messages.h"

#include <iostream>

ClientThreadClass::ClientThreadClass() {}

void ClientThreadClass::ThreadBody(std::string ip, int port, int id, int orders, int type) {
	customer_id = id;
	num_orders = orders;
	req_type = type;
	if (!stub.Init(ip, port)) {
		std::cout << "Thread " << customer_id << " failed to connect" << std::endl;
		return;
	}
	switch (req_type)
	{
	case 1: // as before
		for (int i = 1; i <= num_orders; i++) {
		CustomerRequest crq;
		RobotInfo robot;
		crq.SetRequest(customer_id, i, req_type);
		IdentifyMessage identify_message;
		identify_message.SetIdentifyFlag(CLIENT);

		timer.Start();
		if (stub.SendIdentifyMessage(identify_message) == CLIENT) {
			robot = stub.Order(crq);
		} else {
			// either

			// if we want to make sure that the customer make the order
			// instead of break, we can do:
			// (This kind of failure only happens very large orders
			// for example 1000 make 100 orders. I think it is just too
			// many orders, since we want to test the time > 10 secs but
			// not more than 1 mins.)
			// I can use this way to ensure the order was placed, but
			// it hurts the performance! We pick either.
			timer.EndAndMerge();
			i = i - 1;
			continue;

			// or

			// std::cout << "customer: " << customer_id;
			// std::cout << " failed to make the order." << std::endl;
			// break;
		}
		timer.EndAndMerge();

		if (!robot.IsValid()) {
			std::cout << "Invalid robot " << customer_id << std::endl;
			break;
			}
		}
		break;
	case 2:
		for (int i = 1; i <= num_orders; i++) {
		CustomerRequest crq;
		CustomerRecord record;
		crq.SetRequest(customer_id, i, req_type);

		IdentifyMessage identify_message;
		identify_message.SetIdentifyFlag(CLIENT);
		timer.Start();
		stub.SendIdentifyMessage(identify_message);
		record = stub.ReadRecord(crq);
		timer.EndAndMerge();

		if (!record.IsValid()) {
			std::cout << "Invalid record " << customer_id << std::endl;
			break;
			} else {
			std::cout << "Found record: c_id ";
			std::cout << record.GetCustomerId() << ", last order :";
			std::cout << record.GetLastOrder() << std::endl;
			}
		}
		break;
	case 3:
		for (int i = 1; i <= num_orders; i++) {
			CustomerRequest crq;
			CustomerRecord record;
			crq.SetRequest(i, -1, 2);

			IdentifyMessage identify_message;
			identify_message.SetIdentifyFlag(CLIENT);
			timer.Start();
			stub.SendIdentifyMessage(identify_message);
			record = stub.ReadRecord(crq);
			timer.EndAndMerge();
			if (record.IsValid()) {
				std::cout << record.GetCustomerId() << "\t";
				std::cout << record.GetLastOrder() << std::endl;
				}
		}
		break;
	default:
		break;
	}

}

ClientTimer ClientThreadClass::GetTimer() {
	return timer;
}
