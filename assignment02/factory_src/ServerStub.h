#ifndef __SERVER_STUB_H__
#define __SERVER_STUB_H__

#include <memory>

#include "ServerSocket.h"
#include "Messages.h"

class ServerStub {
private:
	std::unique_ptr<ServerSocket> socket;
public:
	ServerStub();
	void Init(std::unique_ptr<ServerSocket> socket);
	CustomerRequest ReceiveRequest();
	int ShipRobot(RobotInfo info);
	int ReturnRecord(CustomerRecord cus_rec);
	IdentifyMessage ReceiveIdentifyMessage();
	ReplicationRequest ReceiveReplicationRequest();
	int ReturnReplicaResponse(int last_index);
};


#endif // end of #ifndef __SERVER_STUB_H__
