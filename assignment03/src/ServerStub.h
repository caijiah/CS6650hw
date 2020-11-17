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
	RobotOrder ReceiveOrder();
	tx_read ReceiveTxRead();
	tx ReceiveTX();
	int ReadIdentify();
	int SendDecision(int d);
	int SendReadResponse(ReadResponse r_res);
	int SendRobot(RobotInfo info);
};

#endif // end of #ifndef __SERVER_STUB_H__
