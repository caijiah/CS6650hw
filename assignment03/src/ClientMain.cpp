#include <array>
#include <iostream>
#include <iomanip>
#include <thread>
#include <vector>

#include "ClientSocket.h"
#include "ClientThread.h"
#include "ClientTimer.h"

int main(int argc, char *argv[]) {
	std::string ip;
	int port;
	int range_start;
	int range_end;
	int num_customers;
	int num_reqs;
	int robot_type;
	ClientTimer timer;

	std::vector<std::shared_ptr<ClientThreadClass>> client_vector;
	std::vector<std::thread> thread_vector;

	if (argc < 8) {
		std::cout << "not enough arguments" << std::endl;
		std::cout << argv[0] << "[RM ip] [RM port #] [range start] [range end] ";
		std::cout << "[# customers] [# req] [robot type 1 or 3]" << std::endl;
		return 0;
	}

	ip = argv[1];
	port = atoi(argv[2]);
	range_start = atoi(argv[3]);
	range_end = atoi(argv[4]);
	num_customers = atoi(argv[5]);
	num_reqs = atoi(argv[6]);
	if (num_reqs > range_end + 1) {
		std::cout << "out of RM range" << std::endl;
		return 0;
	}
	robot_type = atoi(argv[7]);


	timer.Start();
	for (int i = 0; i < num_customers; i++) {
		auto client_cls = std::shared_ptr<ClientThreadClass>(new ClientThreadClass());
		std::thread client_thread(&ClientThreadClass::ThreadBody, client_cls,
				ip, port, i, range_start, range_end, num_reqs, robot_type);

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

	if (robot_type == 1) {
		timer.PrintStats();
	}

	return 1;
}
