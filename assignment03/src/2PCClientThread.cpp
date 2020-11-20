#include "2PCClientThread.h"
#include "Messages.h"

#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <array>
#include <algorithm>
#include <vector>
// #include <ctime>


ClientThreadClass::ClientThreadClass() {}

void ClientThreadClass::ThreadWriteBody(std::string ip, int port, int id, int rs,
								   int re, int reqs, int type, std::vector<RM> given_rms) {
	customer_id = id;
	num_reqs = reqs;
	robot_type = type;
	range_start = rs;
	range_end = re;
	int current_index;
	srand(time(NULL));
	if (!stub.Init(ip, port)) {
		std::cout << "Thread " << customer_id << " failed to connect" << std::endl;
		return;
	}

	for (auto & g_rm : given_rms) {
		std::unique_ptr<RMAndConnection> rm_c = std::unique_ptr<RMAndConnection>(new RMAndConnection);
		rm_c->rm = g_rm;
		if (!rm_c->rm_stub.Init(g_rm.GetRMIP(), g_rm.GetRMPort())) {
		std::cout << "Thread " << customer_id << " failed to connect" << std::endl;
		return;
		}
		rm_c->rm_s = g_rm.GetBaseKey();
		rm_c->rm_end = g_rm.GetBaseKey() + g_rm.GetNumKvPairs() - 1;
		rms_connections.push_back(std::move(rm_c));
	}

	switch (robot_type) {
		case 1:
			for (int i = 0; i < num_reqs; i++) {
				// tx instance
				tx transcation;
				// tx reads
				std::array<tx_read, 3> tx_reads;
				// tx writes
				std::array<tx_write, 3> tx_writes;
				for (int j = 0; j < 3 ; j++) {
					// rid
					int robot_id = rand() % (range_end - range_start + 1) + range_start;
					// sent a read request
					tx_read read_req;
					read_req.SetRobortId(robot_id);
					//read_req.Print();
					ReadResponse res;
					// receive read response

					for (auto & rm_c : rms_connections) {
						if (robot_id >= rm_c->rm_s && robot_id <= rm_c->rm_end) {
						timer.Start();
						res = rm_c->rm_stub.SendRead(read_req);
						timer.EndAndMerge();
						}
					}
					//res.Print();
					read_req.SetVersion(res.GetVersionNumber());
					// add read info into the TX
					tx_reads[j] = read_req;
					// add write info into the TX
					int read_bd = res.GetBid();
					// std::cout<< customer_id << "thread receive a res: " << std::endl;
					// res.Print();
					tx_write write_req;
					write_req.SetInfo(robot_id, read_bd + 1, customer_id);
					// std::cout<< customer_id << "thread make a write: " << std::endl;
					// write_req.Print();
					tx_writes[j] = write_req;
				}
				transcation.SetTxReads(tx_reads);
				// for (int )
				transcation.SetTxWrites(tx_writes);
				// transcation.Print();
				timer.Start();
				int tx_result = stub.SendTX(transcation);
				timer.EndAndMerge();
				if (tx_result == 1) {
					timer.CommitIncrement();
				} else {
					timer.AbortIncrement();
				}
			}
			break;
		case 3:
			current_index = range_start;
			for (auto & rm_c : rms_connections) {
				int rm_s = rm_c->rm_s;
				int rm_end = rm_c->rm_end;
				while (num_reqs > 0 && current_index <= rm_end && current_index >= rm_s) {
					tx_read read_req;
					read_req.SetRobortId(current_index);
					ReadResponse res;
					// receive read response
					res = rm_c->rm_stub.SendRead(read_req);
					std::cout << current_index << "\t";
					std::cout << res.GetBid() << "\t";
					std::cout << res.GetCustomerId() << "\t";
					std::cout << res.GetVersionNumber() << std::endl;
					current_index++;
					num_reqs--;
				}
			}
			break;
		default:
			break;
	}
}

ClientTimer ClientThreadClass::GetTimer() {
	return timer;
}
