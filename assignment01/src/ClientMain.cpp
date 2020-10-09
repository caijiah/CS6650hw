#include "ClientStub.h"
#include "Order.h"

#include <iostream>
#include <stdlib.h>
#include <thread>
#include <mutex>
#include <chrono>
#include <list>
#include <numeric>
#include <algorithm>
#include <string>


using namespace std;
mutex mtx;
list<long long> latencies_record;

// help function that simply use stoi but with error handling
int string_to_int(string str) {
    try {
        return std::stoi(str);
        } catch (invalid_argument& e) {
			cerr << "Error: Invalid input: " << str 
			<< ". Please check." << endl;
            exit(1);
        }   
}

void make_orders(
	unique_ptr<ClientStub> cli_stub,
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
        cerr << "Please type these 6 arguments: [ip addr] [port #]"
		<< " [# customers] [# orders] [robot type]" << endl;
		exit(1);
	}

    // assign given arguments
        cout << "parse" << endl;
    	string ip_addr = argv[1];
	int port_num = string_to_int(argv[2]);
	unsigned int num_customers, num_orders, robot_type;
	num_customers = string_to_int(argv[3]);
	num_orders = string_to_int(argv[4]);
	robot_type = string_to_int(argv[5]);

	// create the customer threads as many as customer number:
	thread customer_threads[num_customers];
	
	auto main_start = chrono::high_resolution_clock::now();
	for (unsigned int i = 0; i < num_customers; i++) {
		std::unique_ptr<ClientStub> cli_stub = unique_ptr<ClientStub>(new ClientStub(ip_addr, port_num));
                customer_threads[i] = thread(make_orders, move(cli_stub), i, num_orders, robot_type);
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
	cout << avg_latency << '\t' << min_latency << '\t' << max_latency << '\t' << throughput << endl;

	return 0;
}

