#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

#include "TMSocket.h"
#include "TMThread.h"
#include "ResourceManager.h"

int main(int argc, char *argv[]) {
	int port;
	int worker_cnt = 0;
	int num_RMs;
	ServerSocket socket;
	RobotFactory factory;
	std::unique_ptr<ServerSocket> new_socket;
	std::vector<std::thread> thread_vector;
	std::vector<RM> rms;

	if (argc < 3) {
		std::cout << "not enough arguments" << std::endl;
		std::cout << argv[0] << "[port] [# RMs] (repeat [ip] [port] [# kv pairs] [base key])" << std::endl;
		return 0;
	}
	port = atoi(argv[1]);
	num_RMs = atoi(argv[2]);

	if (argc < 3 + 4 * num_RMs) {
		std::cout << "not enough arguments" << std::endl;
		std::cout << argv[0] << "[port] [# RMs] ";
		std::cout << "(repeat [ip] [port] [# kv pairs] [base key])" << std::endl;
		return 0;
	}

	for (int i = 0; i < num_RMs; i++) {
		RM new_rm;
		new_rm.SetRMIP(argv[3 + i*4]);
		new_rm.SetRMPort(atoi(argv[3 + i*4 + 1]));
		new_rm.SetNumKvPairs(atoi(argv[3 + i*4 + 2]));
		new_rm.SetBaseKey(atoi(argv[3 + i*4 + 3]));
		rms.push_back(new_rm);
	}

	factory.SetUpKvTable(num_RMs, rms);

	std::thread tx_thread(&RobotFactory::TXThread, &factory, worker_cnt++);
	thread_vector.push_back(std::move(tx_thread));

	if (!socket.Init(port)) {
		std::cout << "Socket initialization failed" << std::endl;
		return 0;
	}

	while ((new_socket = socket.Accept())) {
		std::thread worker_thread(&RobotFactory::WokerThread, &factory,
				std::move(new_socket), worker_cnt++);
		thread_vector.push_back(std::move(worker_thread));
	}
	return 0;
}
