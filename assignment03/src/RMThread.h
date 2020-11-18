#ifndef __SERVERTHREAD_H__
#define __SERVERTHREAD_H__

#include <condition_variable>
#include <future>
#include <mutex>
#include <queue>
#include <thread>

#include "Messages.h"
#include "RMSocket.h"

#define PREPARE 0
#define COMMIT_ABORT 1

struct ExpertRequest {
	RobotInfo robot;
	std::promise<RobotInfo> prom;
};

struct TXRequest {
    int identify;
    int TM_Req;
	tx transaction;
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
	// kv_tbl
	int kv_tbl_size;
	int kv_base;
	struct kv_value *kv_table;
	std::mutex kv_table_lock;

	std::queue<std::unique_ptr<TXRequest>> txrq;
	std::mutex txrq_lock;
	std::condition_variable txrq_cv;

	int SendToTXThread(std::unique_ptr<TXRequest> tx_req);
public:
	void SetUpKvTable(int kv_size, int base);
	void WokerThread(std::unique_ptr<ServerSocket> socket, int id);
	void TXThread(int id);
};

#endif // end of #ifndef __SERVERTHREAD_H__
