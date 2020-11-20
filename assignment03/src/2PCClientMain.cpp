#include <array>
#include <iostream>
#include <iomanip>
#include <thread>
#include <vector>
#include <cmath>

#include "2PCClientSocket.h"
#include "2PCClientThread.h"
#include "2PCClientTimer.h"
#include "ResourceManager.h"

int main(int argc, char *argv[]) {
	std::string TM_ip;
	int TM_port;
	int range_start;
	int range_end;
	int num_customers;
	int num_RMs;
	int num_reqs;
	int req_type;
	ClientTimer timer;
	std::vector<RM> rms;
	int min_rm_start = INFINITY;
	int max_rm_end = -INFINITY;

	std::vector<std::shared_ptr<ClientThreadClass>> client_vector;
	std::vector<std::thread> thread_vector;

	if (argc < 4) {
		std::cout << "not enough arguments" << std::endl;
		std::cout << argv[0] << "[TM ip] [TM port] ";
        std::cout << "[# RMs] (repeat [ip] [port] [# kv pairs] [base key]) ";
		std::cout << "[key range start] [key range end] [# customer] [# tx] [request type 1 or 3] "<< std::endl;
		return 0;
	}

	TM_ip = argv[1];
	TM_port = atoi(argv[2]);
	num_RMs = atoi(argv[3]);

	// parse RMs
	if (argc < 4 + 4 * num_RMs) {
		std::cout << "not enough arguments" << std::endl;
		std::cout << argv[0] << "[TM ip] [TM port] ";
        std::cout << "[# RMs] (repeat [ip] [port] [# kv pairs] [base key]) ";
		std::cout << "[key range start] [key range end] [# customer] [# tx] [request type 1 or 3] "<< std::endl;
		return 0;
	}

	for (int i = 0; i < num_RMs; i++) {
		RM new_rm;
		new_rm.SetRMIP(argv[4 + i*4]);
		new_rm.SetRMPort(atoi(argv[4 + i*4 + 1]));
		new_rm.SetNumKvPairs(atoi(argv[4 + i*4 + 2]));
		new_rm.SetBaseKey(atoi(argv[4 + i*4 + 3]));
		int rm_start = new_rm.GetBaseKey();
		int rm_end = rm_start + new_rm.GetNumKvPairs() - 1;
		if (rm_start < min_rm_start) {
			min_rm_start = rm_start;
		}
		if (rm_end > max_rm_end) {
			max_rm_end = rm_end;
		}
		rms.push_back(new_rm);
	}

	int after_parse = 4 + 4 * num_RMs;
	range_start = atoi(argv[after_parse]);
	range_end = atoi(argv[after_parse + 1]);
	num_customers = atoi(argv[after_parse + 2]);
	num_reqs = atoi(argv[after_parse + 3]);
	req_type = atoi(argv[after_parse + 4]);

	if (!(range_start >= min_rm_start && range_end <= max_rm_end)) {
			std::cout << "out of RMs range" << std::endl;
			return 0;
	}

	timer.Start();
	// as usual
	for (int i = 0; i < num_customers; i++) {
		auto client_cls = std::shared_ptr<ClientThreadClass>(new ClientThreadClass());
		std::thread client_thread(&ClientThreadClass::ThreadWriteBody, client_cls,
				TM_ip, TM_port, i, range_start, range_end, num_reqs, req_type, rms);

		client_vector.push_back(std::move(client_cls));
		thread_vector.push_back(std::move(client_thread));
	}
	for (auto& th : thread_vector) {
		th.join();
	}
	timer.End();

	for (auto& cls : client_vector) {
		timer.Merge(cls->GetTimer());
	}
	if (req_type == 1) {
		timer.PrintStats();
	}
	return 1;

}
