#ifndef __SERVERTHREAD_H__
#define __SERVERTHREAD_H__

#include <condition_variable>
#include <future>
#include <mutex>
#include <queue>
#include <map>
#include <thread>

#include "Messages.h"
#include "ServerSocket.h"

struct MapOp {
int opcode; // operation code: 1 - update value 
int arg1; // customer_id to apply the operation 
int arg2; // parameter for the operation
};

struct AdminRequest {
	RobotInfo robot;
	std::promise<RobotInfo> prom;
};

class RobotFactory {
private:
	std::queue<std::unique_ptr<AdminRequest>> arq;
	std::mutex arq_lock;
	std::condition_variable arq_cv;

	// a2
	std::mutex crd_lock;
	std::map<int, int> customer_record;
	std::mutex smr_log_lock;
	std::vector<MapOp> smr_log;

	RobotInfo CreateRobotAndAdminRequest(CustomerRequest crq, int engineer_id);
	// RobotInfo CreateSpecialRobot(RobotOrder order, int engineer_id);
public:
	void EngineerThread(std::unique_ptr<ServerSocket> socket, int id);
	void AdminThread(int id);
};

#endif // end of #ifndef __SERVERTHREAD_H__

