#include <iostream>
#include <memory>
#include <map>


#include "ServerThread.h"
#include "ServerStub.h"
#include "FactoryStub.h"
#include "Messages.h"

void RobotFactory::SetAdminConfig(AdminConfig ad_config) {
	admin_config = ad_config;
}


RobotInfo RobotFactory::CreateRobotAndAdminRequest(CustomerRequest crq, int engineer_id) {
	RobotInfo robot;
	robot.CopyRequest(crq);
	robot.SetEngineerId(engineer_id);
	// std::cout << "customer_id" << robot.GetCustomerId() << std::endl;
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

		identify_message = server_stub.ReceiveIdentifyMessage();
		int indentify_flag;
		indentify_flag = identify_message.GetIdentifyFlag();
		std::cout << "receive a indentify_flag" << indentify_flag << std::endl;
		// std::cout << "receive a indentify_flag" << std::endl;
		switch (indentify_flag) {
			case CLIENT:
			{
				while (true) {
					crq = server_stub.ReceiveRequest();
					if (!crq.IsValid()) {
						break;
					}
					request_type = crq.GetRequestType();
					switch (request_type) {
						case 1:
						{
							std::cout << "before create" << std::endl;
							robot = CreateRobotAndAdminRequest(crq, engineer_id);
							std::cout << "Got a robot" << robot.GetCustomerId() << std::endl;
							server_stub.ShipRobot(robot);
							break;
						}
						case 2:
						{
							cid = crq.GetCustomerId();
							// defaul
							std::cout << "Got read request" << cid << std::endl;

							crd.SetCustomerId(cid);
							crd.SetLastOrder(-1);
							crd_lock.lock();
							// if find
							if (customer_record.find(cid) != customer_record.end()) {
								crd.SetLastOrder(customer_record[cid]);
							}
							crd_lock.unlock();
							std::cout << "record: " << cid << " : " << crd.GetLastOrder() << std::endl;

							server_stub.ReturnRecord(crd);
							break;
						}
						default:
							std::cout << "Undefined request type: "
								<< request_type << std::endl;
					}
				}
				break;
			}
			case PFA:
			{
				while (true) {
					ReplicationRequest replica_req;
					replica_req = server_stub.ReceiveReplicationRequest();
					if (!replica_req.IsValid()) {
						std::cout << "did I break?" << std::endl;
						break;
					}
					int primary_factory = replica_req.GetFactoryId();
					if (admin_config.primary_id != primary_factory) {
						admin_config.primary_id = primary_factory;
					}
					// write log
					int replica_last_index = replica_req.GetLastIndex();
					MapOp replica_mop = replica_req.GetMapOp();
					std::cout << replica_mop.GetOpcode() << std::endl;
					std::cout << replica_mop.GetArg1() << std::endl;
					std::cout << replica_mop.GetArg2() << std::endl;
					std::cout << "commited order number " <<  replica_mop.GetArg2() << std::endl;

					smr_log_lock.lock();
					std::cout << "last index" << replica_last_index << std::endl;
					if ((replica_last_index - admin_config.last_index) > 1) {
						int temp_last = admin_config.last_index;
						for (int i = temp_last; i < replica_last_index; i++) {
							MapOp dummy_mapop = MapOp();
							smr_log.insert((smr_log.begin() + i), dummy_mapop);
							std::cout << "fix" << i << std::endl;
							admin_config.last_index += 1;
						}
						std::cout << "fixed" << admin_config.last_index << std::endl;
					}
					smr_log.insert((smr_log.begin() + replica_last_index), replica_mop);
					std::cout << "check order number " << smr_log[replica_last_index].GetArg2() << std::endl;
					std::cout << "after set smr_log" << std::endl;
					smr_log_lock.unlock();
					admin_config.last_index = replica_last_index;
					// Applies MapOp in the req.committed index of smr log to the customer record and
					// update self.committed index; and
					int replica_commited_index = replica_req.GetCommittedIndex();
					std::cout << "commited index" << replica_commited_index << std::endl;
					if (replica_commited_index != -1) {
						smr_log_lock.lock();
						MapOp op_replica_commited = smr_log[replica_commited_index];
						smr_log_lock.unlock();
						int commited_op_cid = op_replica_commited.GetArg1();
						int commited_op_order_num = op_replica_commited.GetArg2();
						std::cout << "commited order number " <<  commited_op_order_num << std::endl;
						crd_lock.lock();
						if (customer_record.find(commited_op_cid) == customer_record.end()) {
							customer_record.insert({commited_op_cid, commited_op_order_num});
						} else {
							customer_record[commited_op_cid] = commited_op_order_num;
						}
						crd_lock.unlock();
						admin_config.committed_index = replica_commited_index;
					}
					server_stub.ReturnReplicaResponse(admin_config.last_index);
				}
				break;
			}
			default:
				std::cout << "Undefined identity type: " << indentify_flag << std::endl;
				// break;
		}
}

void RobotFactory::updateCusRecord() {
	crd_lock.lock();
	MapOp last_op = smr_log[admin_config.last_index];
	int last_op_cid = last_op.GetArg1();
	int last_op_order_num = last_op.GetArg2();
	if (customer_record.find(last_op_cid) == customer_record.end()) {
		customer_record.insert({last_op_cid, last_op_order_num});
	} else {
		customer_record[last_op_cid] = last_op_order_num;
	}
	std::cout << "written record"<< last_op_cid << " : " << customer_record[last_op_cid] << std::endl;
	admin_config.committed_index = admin_config.last_index;
	crd_lock.unlock();
}

void RobotFactory::AdminThread(int id) {
	std::unique_lock<std::mutex> ul(arq_lock, std::defer_lock);
	while (true) {
		ul.lock();

		if (arq.empty()) {
			arq_cv.wait(ul, [this]{ return !arq.empty(); });
		}
		std::cout << "receive done?" << std::endl;
		// received customer record update request
		admin_config_lock.lock();
		if (admin_config.primary_id != admin_config.unique_id) {
			if (admin_config.primary_id != -1 ) {
				std::cout << "primary_id"<< admin_config.primary_id << std::endl;
				std::cout << "might enter here???" << std::endl;
				updateCusRecord();
				std::this_thread::sleep_for(std::chrono::microseconds(100));
			}
			admin_config.primary_id = admin_config.unique_id;
		}
		admin_config_lock.unlock();
		std::cout << "set primary id done" << std::endl;

		if (!peer_connected) {
			// make connections...
			std::map<int, Peer>::iterator it;
			for ( it = admin_config.peers.begin(); it != admin_config.peers.end(); it++ ) {
				Peer peer = it->second;
				if (peer.GetPeerStatus()) {
					std::unique_ptr<FactoryStub> factory_stub = std::unique_ptr<FactoryStub>(new FactoryStub);
					std::cout << "FactoryStub init" << std::endl;
					if (!factory_stub->Init(peer.GetPeerIP(), peer.GetPeerPort())) {
						peer.SetPeerStatus(false);
						std::cout << "Peer " << peer.GetPeerID() << " failed to connect" << std::endl;
					} else {
						peer_connections[peer.GetPeerID()] = std::move(factory_stub);
					}
				}
			}
				std::cout << "make connections?" << std::endl;
			peer_connected = true;
		}
		std::cout << "make connections done?" << std::endl;

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
		if ((smr_log.size() - admin_config.last_index) > 1) {
			smr_log[admin_config.last_index] = op;
		} else {
			smr_log.push_back(op);
			admin_config.last_index = admin_config.last_index + 1;
		}
		smr_log_lock.unlock();

		std::cout << admin_config.last_index << std::endl;
		std::cout << "last_index:" << admin_config.last_index << std::endl;


		ReplicationRequest replica_req;
		replica_req.SetRequest(admin_config.factory_id, admin_config.committed_index
			                   , admin_config.last_index, op);
		// send idetnfity message
		std::map<int, std::unique_ptr<FactoryStub>>::iterator it;
		for (it = peer_connections.begin(); it != peer_connections.end(); it++ ) {
			// std::unique_ptr<FactoryStub> each_stub = std::move(it->second);
			if (!it->second->GetStubDown()) {
				std::cout << "send " << std::endl;
				if (!it->second->GetIdentified()) {
					IdentifyMessage identify_message;
					identify_message.SetIdentifyFlag(PFA);
					it->second->SendIdentifyMessage(identify_message);
					it->second->SetIdentified(true);
				}
				int res = it->second->SendReplicationRequest(replica_req);
				if (res != admin_config.last_index) {
					std::cout << "One back up failed: " << it->first << std::endl;
					// handle error
					it->second->ShutStubDown();
					// return;
				}
			}
		}

		updateCusRecord();
		// finish
		req->admin_id_pro.set_value(id);
	}
}
