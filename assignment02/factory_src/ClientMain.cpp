#include <array>
#include <iostream>
#include <iomanip>
#include <thread>
#include <vector>
#include <algorithm> // for std::find
#include <iterator> // for std::begin, std::end

#include "ClientSocket.h"
#include "ClientThread.h"
#include "ClientTimer.h"

int main(int argc, char *argv[]) {
	std::string ip;
	int port;
	int num_customers;
	int num_orders;
	int request_type; // 1, 2, 3
	ClientTimer timer;

	std::vector<std::shared_ptr<ClientThreadClass>> client_vector;
	std::vector<std::thread> thread_vector;

	if (argc < 6) {
		std::cout << "not enough arguments" << std::endl;
		std::cout << argv[0] << "[ip] [port #] [# customers] ";
		std::cout << "[# orders] [robot type 0 or 1]" << std::endl;
		return 0;
	}

	ip = argv[1];
	port = atoi(argv[2]);
	num_customers = atoi(argv[3]);
	num_orders = atoi(argv[4]);
	int valid_rq[] = {1, 2, 3};
	request_type = atoi(argv[5]);

	auto valid_rq_begin = std::begin(valid_rq);
	auto valid_rq_end = std::end(valid_rq);

	if (std::find(valid_rq_begin, valid_rq_end, request_type) == valid_rq_end) {
		std::cout << "Invalid request type as " << request_type << std::endl;
		std::cout << "Please choose from : ";
		for (auto it = valid_rq; it != valid_rq_end; ++it) {
			std::cout << *it << " ";
		}
		return 0;
	}

	if (request_type == 3) {
		auto client_cls = std::shared_ptr<ClientThreadClass>(new ClientThreadClass());
		std::thread client_thread(&ClientThreadClass::ThreadBody, client_cls,
				ip, port, 0, num_orders, request_type);
		client_thread.join();
		return 1;
	}


	timer.Start();
	for (int i = 1; i <= num_customers; i++) {
		auto client_cls = std::shared_ptr<ClientThreadClass>(new ClientThreadClass());
		std::thread client_thread(&ClientThreadClass::ThreadBody, client_cls,
				ip, port, i, num_orders, request_type);

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
	timer.PrintStats();

	return 1;
}
