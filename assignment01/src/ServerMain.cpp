#include "ServerStub.h"
#include "RobotInfo.h"
#include "Order.h"
#include <iostream>
#include <memory>
#include <list>
#include <thread>
#include <queue>
#include <mutex>
#include <chrono>
#include <map>
#include <condition_variable>

std::mutex queue_mtx;
std::mutex map_mtx;
std::condition_variable expert_cv;
std::condition_variable engineer_cv;
std::map<unsigned int, int> engineer_expert_pair;
std::queue<unsigned int> expert_reqs_queue;

int str_to_int(char* arg) {
    try {
        return std::stoi(arg);
    } catch (std::invalid_argument& e) {
        std::cerr << "Error: Fail to parse " << arg << " to an interger." << std::endl;
        exit(1);
    }
}

void expert_process_orders(
                           unsigned int expert_id
                           ) {
    while (true) {
        std::unique_lock<std::mutex> ul(queue_mtx);
        if (expert_reqs_queue.size() == 0) {
            // Auto release the lock and wait indefinitely until being signled.
            expert_cv.wait(ul); // Auto hold the lock after wake up.
        } else {
            int engineer_id = expert_reqs_queue.front();
            expert_reqs_queue.pop();
            ul.unlock();
            std::this_thread::sleep_for(std::chrono::microseconds(100));
            map_mtx.lock();
            engineer_expert_pair.insert({engineer_id, expert_id});
            map_mtx.unlock();
            engineer_cv.notify_all();
        }
    }
}

void process_orders(
                    std::unique_ptr<ServerStub> stub,
                    unsigned int engineer_id
                    ) {
    while (true) {
        std::unique_ptr<Order> order;
        
        try {
            order = stub->receive_order();
        } catch (const char* msg) {
            break;
        }
        
        int expert_id = -1;
        
        if (order->is_expert_required()) {
            queue_mtx.lock();
            expert_reqs_queue.push(engineer_id);
            queue_mtx.unlock();
            expert_cv.notify_one();
            
            while (true) {
                std::unique_lock<std::mutex> ml(map_mtx);
                auto pair_itr = engineer_expert_pair.find(engineer_id);
                if (pair_itr == engineer_expert_pair.end()) {
                    engineer_cv.wait(ml);
                } else {
                    expert_id = pair_itr->second;
                    engineer_expert_pair.erase(engineer_id);
                    ml.unlock();
                    break;
                }
            }
        }
        
        stub->ship_robot(RobotInfo(*order, engineer_id, expert_id));
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cerr << "Error: please provide [port #] [# experts]." << std::endl;
        exit(1);
    }
    int portno = str_to_int(argv[1]);
    int num_of_experts = str_to_int(argv[2]);
    
    std::list<std::thread> experts;
    std::list<std::thread> engineers;
    ServerSocket sock(portno);
    int engineer_id = 0;
    
    // Create threads for expert engineers.
    for (int i = 0; i < num_of_experts; i++) {
        experts.push_back(std::thread(expert_process_orders, engineer_id));
        engineer_id++;
    }
    
    int counter = 0;
    while (true) {
        int conn_sock = sock.accept();
        counter++;
        std::unique_ptr<ServerStub> stub = std::unique_ptr<ServerStub>(new ServerStub(conn_sock));
        engineers.push_back(std::thread(process_orders, std::move(stub), engineer_id));
        engineer_id++;
    }
    
    return 0;
}
