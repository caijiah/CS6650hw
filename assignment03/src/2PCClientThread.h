#ifndef __CLIENT_THREAD_H__
#define __CLIENT_THREAD_H__

#include <chrono>
#include <ctime>
#include <string>

#include "2PCClientStub.h"
#include "2PCClientTimer.h"
#include "ResourceManager.h"

class ClientThreadClass {
	int customer_id;
	int num_reqs;
	int robot_type;
	int range_start;
	int range_end;
	ClientStub stub;
	std::vector<RM> rms;

	ClientTimer timer;
public:
	ClientThreadClass();
	void ThreadWriteBody(std::string ip, int port, int id, int rs,
					int re, int reqs, int type);
	void ThreadReadBody(std::vector<RM> given_rms, int id, int rs,
					int re, int reqs, int type);				
	ClientTimer GetTimer();
	std::array<int, 3> generate3DinstinctRand();
};


#endif // end of #ifndef __CLIENT_THREAD_H__
