#include <iostream>
#include <memory>

#include "TMThread.h"
#include "TMStub.h"
#include "TMClientStub.h"
#include "Messages.h"
#include "ResourceManager.h"

void RobotFactory::SetUpKvTable(int n_rms, std::vector<RM> given_rms) {
  num_RMs = n_rms;
  rms = given_rms;
}

int RobotFactory::SendToTXThread(tx transaction) {
	int result;
	std::promise<int> RM_decision;
	std::future<int> fut = RM_decision.get_future();

	std::unique_ptr<TXRequest> TX_req = std::unique_ptr<TXRequest>(new TXRequest);
	TX_req->transaction = transaction;
	TX_req->decision = std::move(RM_decision);

	txrq_lock.lock();
	txrq.push(std::move(TX_req));
	txrq_cv.notify_one();
	txrq_lock.unlock();


	result = fut.get();
	return result;
}

void RobotFactory::WokerThread(std::unique_ptr<ServerSocket> socket, int id) {
	int identity;
	tx_read tx_r;
	tx transaction;
	ReadResponse rd_res;
	kv_value kv_pair;
	int result = -1;
  std::unique_ptr<TXRequest> tx_req;


	ServerStub stub;

	stub.Init(std::move(socket));

	while (true) {
		std::cout << "try to read " << std::endl;
		// IdentifyMessage identity_msg;
		identity = stub.ReadIdentify();
		// identity = identity_msg.();
		std::cout << "identity: " << identity << std::endl;
		if (identity == -1) {
			break;
		}
		switch (identity) {
			case TX_IDENTIFY:
				transaction = stub.ReceiveTX();
				transaction.Print();
				result = SendToTXThread(transaction);
				// std::cout << "decision ?" << result << std::endl;
				stub.SendDecision(result);
				// std::cout << "finsihed ?" << std::endl;
				break;
			default:
				std::cout << "Undefined request type: "
					<< identity << std::endl;
					break;
		}
		// stub.SendRobot(robot);
	}
}

void RobotFactory::TXThread(int id) {
    if (!rms_connection) {
        for (auto & rm : rms) {
            std::unique_ptr<ClientStub> rm_stub = std::unique_ptr<ClientStub>(new ClientStub);
			if (rm_stub->Init(rm.GetRMIP(), rm.GetRMPort())) {
				rm_connections.push_back(std::move(rm_stub));
			}
        }
        rms_connection = true;
    }

	int local_version = 0;
	std::unique_lock<std::mutex> ul(txrq_lock, std::defer_lock);
	while (true) {
		ul.lock();

		if (txrq.empty()) {
			txrq_cv.wait(ul, [this]{ return !txrq.empty(); });
		}
		auto req = std::move(txrq.front());
		txrq.pop();
		local_version += 1;
		ul.unlock();

		std::this_thread::sleep_for(std::chrono::microseconds(100));
		tx transaction = req->transaction;
		transaction.SetVersionNumber(local_version);
    std::cout << "check trans: " << std::endl;
    transaction.Print();

        bool final_decision = true;
        for (auto & rm_connection: rm_connections) {
            int decision = rm_connection->SendTX(transaction);
            if (decision == -1) {
                final_decision = false;
            }
        }

        std::cout << "TM receive decison" << final_decision << std::endl;

        if (final_decision) {
            req->decision.set_value(1);
        } else {
            req->decision.set_value(-1);
        }
        for (auto & rm_connection: rm_connections) {
            rm_connection->SendCommitAbort(final_decision);
        }
	}
	// std::cout << "quit" << std::endl;
}
