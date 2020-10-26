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
#include "FactoryStub.h"
#include "Peer.h"

struct AdminRequest {
	CustomerRecord customer_record;
	std::promise<int> admin_id_pro;
};

struct AdminConfig {
	// the last index of the smr_log that has data
	int last_index = -1;
	// the last index of the smr_log where the
	// MapOp of the log entry is committed and
	// applied to the customer_record
	int committed_index = -1;
	// the production factory id (server id).
	// initially set to -1.
	int primary_id = - 1;
	// the id of the factory. This is assigned via 
	// the command line arguments.
	int factory_id;

	int unique_id;

	std::map<int, Peer> peers;
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

	AdminConfig admin_config;
	std::map<int, FactoryStub> peer_connections;
	bool peer_connected = false;

	RobotInfo CreateRobotAndAdminRequest(CustomerRequest crq, int engineer_id);
	// RobotInfo CreateSpecialRobot(RobotOrder order, int engineer_id);
public:
	void updateCusRecord();
	void SetAdminConfig(AdminConfig ad_config);
	void EngineerThread(std::unique_ptr<ServerSocket> socket, int id);
	void AdminThread(int id);
};

#endif // end of #ifndef __SERVERTHREAD_H__

