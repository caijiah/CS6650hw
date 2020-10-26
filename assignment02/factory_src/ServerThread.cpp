#include <iostream>
#include <memory>
#include <map>


#include "ServerThread.h"
#include "ServerStub.h"
#include "FactoryStub.h"
#include "Message.h"

void RobotFactory::SetAdminConfig(AdminConfig ad_config) {
	admin_config = ad_config;
}


RobotInfo RobotFactory::CreateRobotAndAdminRequest(CustomerRequest crq, int engineer_id) {
	RobotInfo robot;
	robot.CopyRequest(crq);
	robot.SetEngineerId(engineer_id);
	int admin_id = -1;
	CustomerRecord cus_record;
	cus_record.SetRecord(robot.GetCustomerId(), robot.GetOrderNumber());

	std::promise<int> admin_id_pro;
	std::future<int> fut = admin_id_pro.get_future();


	std::unique_ptr<AdminRequest> req = std::unique_ptr<AdminRequest>(new AdminRequest);
	req->customer_record = cus_record;
	req->admin_id_pro = std::move(admin_id_pro);

	arq_lock.lock();
	arq.push(std::move(req));
	arq_cv.notify_one();
	arq_lock.unlock();

	admin_id = fut.get();
	robot.SetAdminId(admin_id);
	return robot;
}

void RobotFactory::EngineerThread(std::unique_ptr<ServerSocket> socket, int id) {
	int engineer_id = id;
	int request_type;
	int cid;
	CustomerRequest crq;
	RobotInfo robot;
	CustomerRecord crd;
	IdentifyMessage identify_message;

	ServerStub server_stub;

	server_stub.Init(std::move(socket));

	while (true) {
		identify_message = server_stub.ReceiveIdentifyMessage();
		indentify_flag = identify_message.GetIdentifyFlag();
		switch (indentify_flag) {
			case CLIENT:
				crq = server_stub.ReceiveRequest();
				if (!crq.IsValid()) {
					break;
				}
				request_type = crq.GetRequestType();
				switch (request_type) {
					case 1:
						robot = CreateRobotAndAdminRequest(crq, engineer_id);
						server_stub.ShipRobot(robot);
						break;
					case 2:
						cid = crq.GetCustomerId();
						// defaul
						crd.SetCustomerId(cid);
						crd.SetLastOrder(-1);
						crd_lock.lock();
						// if find
						if (customer_record.find(cid) != customer_record.end()) {
							crd.SetLastOrder(customer_record[cid]);
						}
						crd_lock.unlock();
						server_stub.ReturnRecord(crd);
						break;
					default:
						std::cout << "Undefined request type: "
							<< request_type << std::endl;
				}
				break;
			case PFA:
				ReplicationRequest replica_req;
				int primary_factory = replica_req.GetFactoryId();
				if (admin_config.primary_id != primary_factory) {
					admin_config.primary_id = primary_factory;
				}
				// write log
				int replica_last_index = replica_req.GetLastIndex();				
				MapOp replica_mop = replica_req.GetMapOp();
				smr_log_lock.lock();
				smr_log.at(replica_last_index) = replica_mop;
				smr_log_lock.unlock();
				admin_config.last_index = replica_last_index;
				// Applies MapOp in the req.committed index of smr log to the customer record and
				// update self.committed index; and
				int replica_commited_index = replica_commited_index;
				MapOp op_replica_commited = smr_log[replica_commited_index];
				int commited_op_cid = last_op.GetArg1();
				int commited_op_order_num = last_op.GetArg2();
				crd_lock.lock();
				break;
			default:
				std::cout << "Undefined identity type: " << indentify_flag << std::endl;
		}
	}
}

void updateCusRecord() {
	MapOp last_op = MapOp[admin_config.last_index];
	int last_op_cid = last_op.GetArg1();
	int last_op_order_num = last_op.GetArg2();
	crd_lock.lock();
	if (customer_record.find(last_op_cid) == customer_record.end()) {
		customer_record.insert({last_op_cid, last_op_order_num});
	} else {
		customer_record[last_op_cid] = last_op_order_num;
	}
	crd_lock.unlock();
	admin_config.committed_index = admin_config.last_index;
}

void RobotFactory::AdminThread(int id) {
	std::unique_lock<std::mutex> ul(arq_lock, std::defer_lock);
	while (true) {
		ul.lock();

		if (arq.empty()) {
			arq_cv.wait(ul, [this]{ return !arq.empty(); });
		}

		// received customer record update request
		if (admin_config.primary_id != admin_config.unique_id) {
			updateCusRecord();
			}	
			admin_config.primary_id = admin_config.unique_id;
		}

		if (!peer_connected) {
			// make connections...
			std::map<int, Peer>::iterator it;
			for ( it = admin_config.peers.begin(); it != admin_config.peers.end(); it++ ) {
				Peer peer = it->second;
				FactoryStub factory_stub;
				factory_stub.Init(peer.peer_ip, peer.peer_port);
				peer_connections.insert({peer.peer_id, factory_stub});
			  }
			peer_connected = true;
		}

		auto req = std::move(arq.front());
		arq.pop();

		ul.unlock();

		std::this_thread::sleep_for(std::chrono::microseconds(100));
		CustomerRecord cus_record = req->customer_record;
		int cid = cus_record.GetCustomerId();
		int order_num = cus_record.GetLastOrder();
		MapOp op;
		op.SetMapOp(1, cid, order_num);
		smr_log_lock.lock();
		smr_log.push_back(op);
		smr_log_lock.unlock();

		admin_config.last_index += 1;

		IdentifyMessage identify_message;
		identify_message.SetIdentifyFlag(PFA);
		ReplicationRequest replica_req;
		replica_req.SetRequest(admin_config.factory_id, admin_config.unique_id, admin_config.last_index, op);
		// send idetnfity message
		std::map<int, FactoryStub>::iterator it;
		for (it = peer_connections.begin(); it != peer_connections.end(); it++ ) {
			FactoryStub each_stub = it->second;
			each_stub.SendIdentifyMessage(identify_message);
			std::this_thread::sleep_for(std::chrono::microseconds(100));
			int res = each_stub.SendReplicationRequest(replica_req);
			if (res != admin_config.last_index) {
				// handle error
				perror("ERROR: failed to create a socket");
				return;
			}
		}
		
		updateCusRecord();
		// finish
		req->admin_id_pro.set_value(id);
	}
}
