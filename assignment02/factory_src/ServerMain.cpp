#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

#include "ServerSocket.h"
#include "ServerThread.h"
#include "Peer.h"

#define NUM_FACTORY_ADMIN 1

int main(int argc, char *argv[]) {
	int port;
	int engineer_cnt = 0;
	// int num_admin = NUM_FACTORY_ADMIN;
	int num_peers;
	int unique_ID;
	std::map<int, Peer> peers; 

	ServerSocket socket;
	RobotFactory factory;
	std::unique_ptr<ServerSocket> new_socket;
	std::vector<std::thread> thread_vector;

	
	if (argc < 4) {
		std::cout << "not enough arguments" << std::endl;
		std::cout << argv[0] << "[port #] [# nuique ID] [# peers]" << std::endl;
		return 0;
	}

	num_peers = atoi(argv[3]);
	if (argc < 4 + 3 * num_peers) {
		std::cout << "not enough arguments" << std::endl;
		std::cout << argv[0] << "[port #] [# nuique ID] [# peers] ";
		std::cout << "(repeat [ID] [IP] [port #]" << std::endl;
		return 0;
	}

	port = atoi(argv[1]);
	unique_ID = atoi(argv[2]);
	for (int i = 0; i < num_peers; i++) {
		Peer new_peer;
		new_peer.SetPeerID(atoi(argv[4 + i*3]));
		new_peer.SetPeerIP(argv[4 + i*3 + 1]);
		new_peer.SetPeerPort(atoi(argv[4 + i*3 + 2]));
		peers.insert({new_peer.GetPeerID(), new_peer});
	}

	AdminConfig admin_config;
	admin_config.unique_id = unique_ID;
	admin_config.last_index = 0;
	admin_config.committed_index = 0;
	admin_config.peers = peers;

	factory.SetAdminConfig(admin_config);
	std::thread admin_thread(&RobotFactory::AdminThread, &factory, engineer_cnt++);
	thread_vector.push_back(std::move(admin_thread));

	if (!socket.Init(port)) {
		std::cout << "Socket initialization failed" << std::endl;
		return 0;
	}

	while ((new_socket = socket.Accept())) {
		std::thread engineer_thread(&RobotFactory::EngineerThread, &factory, 
				std::move(new_socket), engineer_cnt++);
		thread_vector.push_back(std::move(engineer_thread));
	}
	return 0;
}
