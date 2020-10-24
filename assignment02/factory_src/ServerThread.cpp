#include <iostream>
#include <memory>

#include "ServerThread.h"
#include "ServerStub.h"


RobotInfo RobotFactory::CreateRobotAndAdminRequest(CustomerRequest crq, int engineer_id) {
	RobotInfo robot;
	robot.CopyRequest(crq);
	robot.SetEngineerId(engineer_id);

	std::promise<RobotInfo> prom;
	std::future<RobotInfo> fut = prom.get_future();

	std::unique_ptr<AdminRequest> req = std::unique_ptr<AdminRequest>(new AdminRequest);
	req->robot = robot;
	req->prom = std::move(prom);

	arq_lock.lock();
	arq.push(std::move(req));
	arq_cv.notify_one();
	arq_lock.unlock();

	robot = fut.get();
	return robot;
}

void RobotFactory::EngineerThread(std::unique_ptr<ServerSocket> socket, int id) {
	int engineer_id = id;
	int request_type;
	CustomerRequest crq;
	RobotInfo robot;
	CustomerRecord crd;

	ServerStub stub;

	stub.Init(std::move(socket));

	while (true) {
		crq = stub.ReceiveRequest();
		if (!crq.IsValid()) {
			break;	
		}
		request_type = crq.GetRequestType();
		switch (request_type) {
			case 1:
				robot = CreateRobotAndAdminRequest(crq, engineer_id);
				stub.ShipRobot(robot);
				break;
			case 2:
				int cid = crq.GetCustomerId();
				// defaul
				crd.SetCustomerId(cid);
				crd.SetLastOrder(-1);
				crd_lock.lock();
				// if find
				if (customer_record.find(cid) != customer_record.end()) {
					crd.SetLastOrder(customer_record[cid]);
				}
				stub.ReturnRecord(crd);
				break;
			default:
				std::cout << "Undefined request type: "
					<< request_type << std::endl;
				
		}
	}
}

void RobotFactory::AdminThread(int id) {
	std::unique_lock<std::mutex> ul(arq_lock, std::defer_lock);
	while (true) {
		ul.lock();

		if (arq.empty()) {
			arq_cv.wait(ul, [this]{ return !arq.empty(); });
		}

		auto req = std::move(arq.front());
		arq.pop();

		ul.unlock();

		std::this_thread::sleep_for(std::chrono::microseconds(100));
		RobotInfo robot = req->robot;
		int cid = robot.GetCustomerId();
		int order_num = robot.GetOrderNumber();
		crd_lock.lock();
		if (customer_record.find(cid) == customer_record.end()) {
			customer_record.insert(cid, order_num);
		} else {
			customer_record[cid] = order_num;
		}
		crd_lock.unlock();

		MapOp op;
		op.arg1 = 1;
		op.arg1 = cid;
		op.arg2 = order_num;
		smr_log_lock.lock();
		smr_log.push_back(op);
		smr_log_lock.unlock();

		req->robot.SetAdminId(id);
		req->prom.set_value(req->robot);	
	}
}
