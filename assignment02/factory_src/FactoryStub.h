#ifndef __FACTORY_STUB_H__
#define __FACTORY_STUB_H__

#include <string>

#include "FactorySocket.h"
#include "Messages.h"

class FactoryStub {
private:
	FactorySocket socket;
	bool stub_down;
	bool identified;
public:
	FactoryStub();
	bool GetStubDown();
	void ShutStubDown();
	int Init(std::string ip, int port);
	void SendIdentifyMessage(IdentifyMessage identify_message);
	int SendReplicationRequest(ReplicationRequest replica_req);
	void SetIdentified(bool state);
	bool GetIdentified();
};

#endif // end of #ifndef __FACTORY_STUB_H__
