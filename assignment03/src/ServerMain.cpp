#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

#include "ServerSocket.h"
#include "ServerThread.h"

int main(int argc, char *argv[]) {
	int port;
	int kv_pairs_num;
	int base_key;
	int engineer_cnt = 0;
	ServerSocket socket;
	RobotFactory factory;
	std::unique_ptr<ServerSocket> new_socket;
	std::vector<std::thread> thread_vector;
	
	if (argc < 4) {
		std::cout << "not enough arguments" << std::endl;
		std::cout << argv[0] << "[port #] [# key-value pairs] [base key #]" << std::endl;
		return 0;
	}
	port = atoi(argv[1]);
	kv_pairs_num = atoi(argv[2]);
	base_key = atoi(argv[3]);

	factory.SetUpKvTable(kv_pairs_num, base_key);

	std::thread expert_thread(&RobotFactory::TXThread, &factory, engineer_cnt++);
	thread_vector.push_back(std::move(expert_thread));

	if (!socket.Init(port)) {
		std::cout << "Socket initialization failed" << std::endl;
		return 0;
	}

	while ((new_socket = socket.Accept())) {
		std::thread engineer_thread(&RobotFactory::WokerThread, &factory, 
				std::move(new_socket), engineer_cnt++);
		thread_vector.push_back(std::move(engineer_thread));
	}
	return 0;
}
