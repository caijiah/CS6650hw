#include "ClientThread.h"
#include "Messages.h"

#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <array>
#include <algorithm>


ClientThreadClass::ClientThreadClass() {}

void ClientThreadClass::ThreadBody(std::string ip, int port, int id, int rs,
								   int re, int reqs, int type) {
	customer_id = id;
	num_reqs = reqs;
	robot_type = type;
	range_start = rs;
	range_end = re;
	srand(time(NULL));
	if (!stub.Init(ip, port)) {
		std::cout << "Thread " << customer_id << " failed to connect" << std::endl;
		return;
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
					timer.Start();
					res = stub.SendRead(read_req);
					timer.EndAndMerge();
					//res.Print();
					read_req.SetVersion(res.GetVersionNumber());
					// add read info into the TX
					tx_reads[j] = read_req;
					// add write info into the TX
					int read_bd = res.GetBid();
					tx_write write_req;
					write_req.SetInfo(robot_id, read_bd + 1, customer_id);
					tx_writes[j] = write_req;
				}
				transcation.SetTxReads(tx_reads);
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
			for (int i = 0; i < num_reqs; i++) {
				tx_read read_req;
				read_req.SetRobortId(range_start + i);
				ReadResponse res;
				// receive read response
				if (i <= range_end) {
					res = stub.SendRead(read_req);
					std::cout << range_start + i << "\t";
					std::cout << res.GetBid() << "\t";
					std::cout << res.GetCustomerId() << "\t";
					std::cout << res.GetVersionNumber() << std::endl;
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
