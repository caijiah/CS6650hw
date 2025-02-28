#ifndef __CLIENT_THREAD_H__
#define __CLIENT_THREAD_H__

#include <string>
#include <vector>
#include <memory>

#include "2PCClientStub.h"
#include "2PCClientTimer.h"
#include "ResourceManager.h"

struct RMAndConnection{
	RM rm;
	ClientStub rm_stub;
	int rm_s;
	int rm_end;
};

class ClientThreadClass {
	int customer_id;
	int num_reqs;
	int robot_type;
	int range_start;
	int range_end;
	ClientStub stub;
	std::vector<std::unique_ptr<RMAndConnection>> rms_connections;

	ClientTimer timer;
public:
	ClientThreadClass();
	void ThreadWriteBody(std::string ip, int port, int id, int rs,
					int re, int reqs, int type, std::vector<RM> given_rms);
	ClientTimer GetTimer();
	std::array<int, 3> generate3DinstinctRand();
};


#endif // end of #ifndef __CLIENT_THREAD_H__
