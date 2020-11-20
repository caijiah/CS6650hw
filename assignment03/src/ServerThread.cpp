#include <iostream>
#include <memory>

#include "ServerThread.h"
#include "ServerStub.h"
#include "Messages.h"

void RobotFactory::SetUpKvTable(int kv_size, int base) {
	kv_tbl_size = kv_size;
	kv_base = base;
	// worry about the memory leak
	kv_table = new kv_value[kv_tbl_size];
}

int RobotFactory::SendToTXThread(tx transaction) {
	int result;
	std::promise<int> RM_decision;
	std::future<int> fut = RM_decision.get_future();

	std::unique_ptr<TXRequest> TX_req = std::unique_ptr<TXRequest>(new TXRequest);
	TX_req->TX_request = transaction;
	TX_req->decision = std::move(RM_decision);

	txrq_lock.lock();
	txrq.push(std::move(TX_req));
	txrq_cv.notify_one();
	txrq_lock.unlock();


	result = fut.get();
	return result;
}

void RobotFactory::WokerThread(std::unique_ptr<ServerSocket> socket, int id) {
	int robot_id;
	int version;
	int bidding_info;
	int customer_id;
	int identity;
	tx_read tx_r;
	tx transaction;
	ReadResponse rd_res;
	kv_value kv_pair;
	int result = -1;

	ServerStub stub;

	stub.Init(std::move(socket));

	while (true) {
		identity = stub.ReadIdentify();
		if (identity == -1) {
			break;
		}
		switch (identity) {
			case TX_READ_IDENTIFY:
				tx_r = stub.ReceiveTxRead();
				//tx_r.Print();
				robot_id = tx_r.GetRobotId();
				kv_table_lock.lock();
				kv_pair = kv_table[robot_id];
				kv_table_lock.unlock();
				bidding_info = kv_pair.bid;
				version = kv_pair.version;
				customer_id = kv_pair.customer_id;
				rd_res.SetInfo(bidding_info, customer_id, version);
				rd_res.Print();
				stub.SendReadResponse(rd_res);
				break;
			case TX_IDENTIFY:
				transaction = stub.ReceiveTX();
				transaction.Print();
				result = SendToTXThread(transaction);
				stub.SendDecision(result);
				break;
			default:
				std::cout << "Undefined request type: "
					<< identity << std::endl;
					break;
		}
	}
}

void RobotFactory::TXThread(int id) {
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
		tx transaction = req->TX_request;
		transaction.SetVersionNumber(local_version);
		std::array<tx_read, 3> tx_reads = transaction.GetTxReads();
		std::array<tx_write, 3> tx_writes = transaction.GetTxWrites();

		bool check_reads = true;
		for (int i = 0; i < 3; i++) {
			tx_read each_read = tx_reads[i];
			each_read.Print();
			int read_rid = each_read.GetRobotId();
			int read_ver = each_read.GetVersionNumber();
			kv_table_lock.lock();
			// copy
			kv_value kv_pair = kv_table[read_rid];
			kv_table_lock.unlock();
			if (read_ver < kv_pair.version) {
				check_reads = false;
			}
		}

		if (check_reads) {
			// commit
			for (int j = 0; j < 3; j++) {
				tx_write each_write = tx_writes[j];
				each_write.Print();
				int new_bid = each_write.GetBid();
				int write_rid = each_write.GetRobotId();
				int write_cid = each_write.GetCustomerId();
				kv_value new_entry;
				new_entry.bid = new_bid;
				new_entry.customer_id = write_cid;
				new_entry.version = local_version;
				kv_table_lock.lock();
				kv_table[write_rid] = new_entry;
				kv_table_lock.unlock();
			}
			req->decision.set_value(1);
		} else {
			//abort
			req->decision.set_value(-1);
		}
	}
}
