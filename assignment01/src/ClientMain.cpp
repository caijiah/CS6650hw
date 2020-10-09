#include "ClientStub.h"

#include <iostream>
#include <stdlib.h>
#include <thread>
#include <mutex>
#include <chrono>
#include <list>
#include <numeric>
#include <algorithm> 


using namespace std;
mutex mtx;
list<long long> latencies_record;

// help function that simply use stoi but with error handling
int string_to_int(std::string str) {
    try {
        return std::stoi(str);
        } catch (std::invalid_argument& e) {
			std::cerr << "Error: Invalid input: " << str 
			<< ". Please check." << std::endl;
            exit(1);
        }   
}

void make_orders(
	std::unique_ptr<ClientStub> cli_stub,
	unsigned int customer_id, 
	unsigned int num_orders,
	unsigned int robot_type
) {
	// each customer takes # orders
	for (unsigned int i = 0; i < num_orders; i++) {
        unique_ptr<Order> order = unique_ptr<Order>(new Order(customer_id, i, robot_type));
		auto start = std::chrono::high_resolution_clock::now();
		cli_stub->order(std::move(order));
		auto end = std::chrono::high_resolution_clock::now();
		long long elapsed_time = chrono::duration_cast<chrono::microseconds>(end - start).count();
		mtx.lock();
		latencies_record.push_back(elapsed_time);
		mtx.unlock();
	}
	cli_stub->disconnect();
}

int main(int argc, char *argv[]) {
	if (argc < 6) {
        std::cerr << "Please type these 6 arguments: [ip addr] [port #]"
		<< " [# customers] [# orders] [robot type]" << std::endl;
		exit(1);
	}

    // assign given arguments
    std::string ip_addr = argv[1];
	int port_num = string_to_int(argv[2]);
	unsigned int num_customers, num_orders, robot_type;
	num_customers = string_to_int(argv[3]);
	num_orders = string_to_int(argv[4]);
	robot_type = string_to_int(argv[5]);

	// create the customer threads as many as customer number:
	std::thread customer_threads[num_customers];
	
	auto main_start = chrono::high_resolution_clock::now();
	for (unsigned int i = 0; i < num_customers; i++) {
		std::unique_ptr<ClientStub> cli_stub;
		cli_stub->Init(ip_addr, port_num);
		customer_threads[i] = std::thread(make_orders, std::move(cli_stub), i, num_orders, robot_type);
	}

	for (unsigned int i = 0; i < num_customers; i++) {
		customer_threads[i].join();
	}
	auto main_end = chrono::high_resolution_clock::now();
	double overall_elapsed = chrono::duration_cast<chrono::duration<double>>(main_end - main_start).count();

	int total_orders = num_customers * num_orders;
	long long sum_latency = accumulate(latencies_record.begin(), latencies_record.end(), 0);
	// return a pointer, so dereference
	long long min_latency = *min_element(latencies_record.begin(), latencies_record.end());
	long long max_latency = *max_element(latencies_record.begin(), latencies_record.end());
	double avg_latency = sum_latency / total_orders;
	double throughput = total_orders / overall_elapsed;
	// prints as [avg latency] [min latency] [max latency] [throughput]
	std::cout << avg_latency << '\t' << min_latency << '\t' << max_latency << '\t' << throughput << std::endl;

	return 0;
}

