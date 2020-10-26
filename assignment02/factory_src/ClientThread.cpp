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
		stub.SendIdentifyMessage(identify_message);
		robot = stub.Order(crq);
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
		record = stub.ReadRecord(crq);
		timer.EndAndMerge();

		if (!record.IsValid()) {
			std::cout << "Invalid record " << customer_id << std::endl;
			break;
			} else {
			std::cout << "Found record: c_id " << record.GetCustomerId() << ", last order :";
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
