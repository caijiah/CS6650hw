#include "ServerSocket.h"
#include "ServerStub.h"
#include <iostream>
#include <list>
#include <thread>
#include <queue>
#include <mutex>
#include <map>
#include <chrono>

using namespace std;

mutex que_mtx;
queue<unsigned int> shared_reqs_queue;

mutex map_mtx;
map<unsigned int, int>engineer_assign_expert;

condition_variable expert_cv;
condition_variable engineer_cv;

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
        
        int expert_id = -1;
        if (order->need_an_expert()) {
            // lock, since we push reqs to the shared queue
            que_mtx.lock();
            shared_reqs_queue.push(engineer_id);
            que_mtx.unlock();
            expert_cv.notify_one();
            
            while (true) {
                unique_lock<mutex> map_lock(map_mtx);
                auto assign_itr = engineer_assign_expert.find(engineer_id);
                if (assign_itr == engineer_assign_expert.end()) {
                    engineer_cv.wait(map_lock);
                } else {
                    expert_id = assign_itr->second;
                    engineer_assign_expert.erase(engineer_id);
                    map_lock.unlock();
                    break;
                }
            }
        }
        ser_stub->ShipRobot(RobotInfo(*order, engineer_id, expert_id));
    }
}

void expert_process_orders(
                           unsigned int expert_id
                           ) {
    while (true) {
        unique_lock<std::mutex> que_lock(que_mtx);
        if (shared_reqs_queue.size() == 0) {
            expert_cv.wait(que_lock);
        } else {
            int engineer_id = shared_reqs_queue.front();
            shared_reqs_queue.pop();
            que_lock.unlock();
            this_thread::sleep_for(chrono::microseconds(100));
            map_mtx.lock();
            engineer_assign_expert.insert({engineer_id, expert_id});
            map_mtx.unlock();
            engineer_cv.notify_all();
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cerr << "Please type these 2 arguments: [port #]"
        << " [# experts]" << std::endl;
        exit(1);
    }
    
    int port_num = string_to_int(argv[1]);
    ServerSocket sock = ServerSocket(port_num);
    int experts_num = string_to_int(argv[2]);
    
    list<thread> engineers;
    list<thread> experts_pool;
    
    int engineer_id = 0;
    
    for (int i = 0; i < experts_num; i++) {
        experts_pool.push_back(std::thread(expert_process_orders, engineer_id));
        engineer_id++;
    }
    
    while (true) {
        int connected_new_fd = sock.ser_sock_accept();
        unique_ptr<ServerStub> ser_stub;
        ser_stub->Init(connected_new_fd);
        engineers.push_back(thread(process_orders, move(ser_stub), engineer_id));
        engineer_id++;
    }
    
    return 0;
}

