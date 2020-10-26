#ifndef __FACTORY_STUB_H__
#define __FACTORY_STUB_H__

#include <string>

#include "ClientSocket.h"
#include "Messages.h"

class FactoryStub {
private:
	ClientSocket socket;
public:
	FactoryStub();
	int Init(std::string ip, int port);
	void SendIdentifyMessage(IdentifyMessage identify_message);
	int SendReplicationRequest(ReplicationRequest replica_req);
};

#endif // end of #ifndef __FACTORY_STUB_H__
