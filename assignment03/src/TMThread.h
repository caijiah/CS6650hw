#ifndef __SERVERTHREAD_H__
#define __SERVERTHREAD_H__

#include <condition_variable>
#include <future>
#include <mutex>
#include <queue>
#include <thread>

#include "Messages.h"
#include "TMSocket.h"
#include "TMStub.h"
#include "TMClientStub.h"
#include "ResourceManager.h"

#define PREPARE 0
#define COMMIT_ABORT 1

struct ExpertRequest {
	RobotInfo robot;
	std::promise<RobotInfo> prom;
};

struct TXRequest {
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
	std::vector<RM> rms;
	bool rms_connection = false;
	int num_RMs;
	std::vector<std::unique_ptr<ClientStub>> rm_connections;


	std::queue<std::unique_ptr<TXRequest>> txrq;
	std::mutex txrq_lock;
	std::condition_variable txrq_cv;

	int SendToTXThread(tx transaction);
public:
	void SetUpKvTable(int n_rms, std::vector<RM> rms);
	void WokerThread(std::unique_ptr<ServerSocket> socket, int id);
	void TXThread(int id);
};

#endif // end of #ifndef __SERVERTHREAD_H__
