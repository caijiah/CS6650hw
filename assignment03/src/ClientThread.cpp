#include "ClientThread.h"
#include "Messages.h"

#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <array>
#include <algorithm>


ClientThreadClass::ClientThreadClass() {}

std::array<int, 3> ClientThreadClass::generate3DinstinctRand() {
	std::array<int, 3> result;
	for (int i = 0; i < 3; i++) {
		int r = rand() % range_end + range_start;
		if (std::find(std::begin(result), std::end(result), r) != std::end(result)) {
			i = i - 1;
			continue;
		} else {
			result[i] = r;
		}
	}
	// for (int j = 0; j < 3; j++) {
    	// cout << result[j] << endl;
  	// }
	return result;
}

void ClientThreadClass::ThreadBody(std::string ip, int port, int id, int rs,
								   int re, int reqs, int type) {
	customer_id = id;
	num_reqs = reqs;
	robot_type = type;
	range_start = rs;
	range_end = re;
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

				std::array<int, 3> rands = generate3DinstinctRand();
				for (int j = 0; j < 3 ; j++) {
					// rid
					int robot_id = rands[j];
					// sent a read request
					tx_read read_req;
					read_req.SetRobortId(robot_id);
					//read_req.Print();
					ReadResponse res;
					// receive read response
					res = stub.SendRead(read_req);
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
				// for (int )
				transcation.SetTxWrites(tx_writes);
				transcation.Print();
				int tx_result = stub.SendTX(transcation);
				std::cout << "RM decision " <<  tx_result << std::endl;
			}
			break;
		case 3:
			for (int i = 0; i < num_reqs; i++) {
				tx_read read_req;
				read_req.SetRobortId(range_start + i);
				ReadResponse res;
				// receive read response
				res = stub.SendRead(read_req);
				std::cout << range_start + i << "\t";
				std::cout << res.GetBid() << "\t";
				std::cout << res.GetCustomerId() << "\t";
				std::cout << res.GetVersionNumber() << std::endl;
		}
		break;
		default:
			break;
	}
}

ClientTimer ClientThreadClass::GetTimer() {
	return timer;
}
