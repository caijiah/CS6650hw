#include "ServerSocket.h"
#include "ServerStub.h"
#include <iostream>
#include <list>
#include <thread>

using namespace std;

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

void process_orders(
	std::unique_ptr<ServerStub> ser_stub,
	unsigned int engineer_id
) {
	while (true) {
		std::unique_ptr<Order> order;

		try {
			order = ser_stub->ReceiveOrder();
		} catch (const char* msg) {
			break;
		}
        
		ser_stub->ShipRobot(RobotInfo(*order, engineer_id, -1));
	}
}

int main(int argc, char *argv[]) {
	if (argc < 1) {
        std::cerr << "Please type these 3 arguments: [port #]" << std::endl;
		exit(1);
	}

	int port_num = string_to_int(argv[1]);
	ServerSocket sock = ServerSocket(port_num);

    int counter = 0;
    int engineer_id = 0;

    list<thread> engineers;
	
	while (true) {
		int connected_new_fd = sock.ser_sock_accept();
		counter++;
		unique_ptr<ServerStub> ser_stub;
		ser_stub->Init(connected_new_fd);
		engineers.push_back(thread(process_orders, move(ser_stub), engineer_id));
		engineer_id++;
	}

	return 0;
}

