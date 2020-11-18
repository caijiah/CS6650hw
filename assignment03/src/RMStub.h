#ifndef __SERVER_STUB_H__
#define __SERVER_STUB_H__

#include <memory>

#include "RMSocket.h"
#include "Messages.h"

class ServerStub {
private:
	std::unique_ptr<ServerSocket> socket;
public:
	ServerStub();
	void Init(std::unique_ptr<ServerSocket> socket);
	tx_read ReceiveTxRead();
	tx ReceiveTX();
    int ReceiveTMReq();
	int ReadIdentify();
	int SendDecision(int d);
    int SendCompleteSig(int sig);
	int SendReadResponse(ReadResponse r_res);
};

#endif // end of #ifndef __SERVER_STUB_H__
