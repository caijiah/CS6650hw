#ifndef __SERVERTHREAD_H__
#define __SERVERTHREAD_H__

#include <condition_variable>
#include <future>
#include <mutex>
#include <queue>
#include <thread>

#include "Messages.h"
#include "ServerSocket.h"

struct ExpertRequest {
	RobotInfo robot;
	std::promise<RobotInfo> prom;
};

struct TXRequest {
	tx TX_request;
	std::promise<int> decision;
};

struct kv_value {
	int bid; // current bid
	int customer_id; // bidding customer’s id
	int version; // version number determined by the system 
	kv_value () { bid = 0; customer_id = -1; version = 0; }
};

class RobotFactory {
private:
	std::queue<std::unique_ptr<ExpertRequest>> erq;
	std::mutex erq_lock;
	std::condition_variable erq_cv;

	// kv_tbl
	int kv_tbl_size;
	int kv_base;
	struct kv_value *kv_table;
	std::mutex kv_table_lock;

	std::queue<std::unique_ptr<TXRequest>> txrq;
	std::mutex txrq_lock;
	std::condition_variable txrq_cv;

	RobotInfo CreateRegularRobot(RobotOrder order, int engineer_id);
	RobotInfo CreateSpecialRobot(RobotOrder order, int engineer_id);
public:
	void SetUpKvTable(int kv_size, int base);
	void WokerThread(std::unique_ptr<ServerSocket> socket, int id);
	void TXThread(int id);
};

#endif // end of #ifndef __SERVERTHREAD_H__

