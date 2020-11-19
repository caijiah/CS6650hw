#ifndef __CLIENT_STUB_H__
#define __CLIENT_STUB_H__

#include <string>

#include "TMClientSocket.h"
#include "Messages.h"

class ClientStub {
private:
	ClientSocket socket;
public:
	ClientStub();
	int Init(std::string ip, int port);
	RobotInfo OrderRobot(RobotOrder order);
	ReadResponse SendRead(tx_read read_req);
	int SendCommitAbort(int final_decision);
	int SendTX(tx transcation);
};


#endif // end of #ifndef __CLIENT_STUB_H__
