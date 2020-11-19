#include <iostream>
#include <memory>

#include "RMThread.h"
#include "RMStub.h"
#include "Messages.h"

void RobotFactory::SetUpKvTable(int kv_size, int base) {
	kv_tbl_size = kv_size;
	kv_base = base;
	// worry about the memory leak
	kv_table = new kv_value[kv_tbl_size];
}

int RobotFactory::SendToTXThread(int ident, int tm_req, tx trans) {
	int result;
	std::unique_ptr<TXRequest> tx_req = std::unique_ptr<TXRequest>(new TXRequest);
	std::promise<int> RM_decision;
	std::future<int> fut = RM_decision.get_future();
	std::cout << "preparing tx_req" << std::endl;
	switch (ident) {
		case PREPARE:
			// std::cout << "in PREPARE" << std::endl;
			tx_req->identify = ident;
			// std::cout << "identify done" << std::endl;

			tx_req->transaction = trans;
			// std::cout << "trans done" << std::endl;

			tx_req->decision = std::move(RM_decision);
			// std::cout << "decision done" << std::endl;
			// std::cout << "PREPARE done" << std::endl;
			break;
		case COMMIT_ABORT:
			// std::cout << "in COMMIT_ABORT" << std::endl;
			tx_req->identify = ident;
			tx_req->TM_Req = tm_req;
			tx_req->decision = std::move(RM_decision);
			std::cout << "COMMIT_ABORT done" << std::endl;
			break;
		default:
			break;
	}

	txrq_lock.lock();
	txrq.push(std::move(tx_req));
	std::cout << "pushed" << std::endl;
	if (!txrq.empty()) {
		std::cout << "it is not empty" << std::endl;
	}
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
  int final_decision;
    // std::unique_ptr<TXRequest> tx_req;


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
				// transaction.Print();
                // tx_req = std::unique_ptr<TXRequest>(new TXRequest);
        // tx_req->identify = PREPARE;
        // tx_req->transaction = transaction;
				result = SendToTXThread(PREPARE, -1, transaction);
				// std::cout << "decision ?" << result << std::endl;
				stub.SendDecision(result);
				// std::cout << "finsihed ?" << std::endl;
				break;
      case TM_RM_IDENTIFY:
					std::cout << "asked to commit or abort" << std::endl;
          final_decision = stub.ReceiveTMReq();
          // tx_req->TM_Req = final_decision;
          result = SendToTXThread(COMMIT_ABORT, final_decision, transaction);
          stub.SendCompleteSig(COMPLETE_SIG);
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
	std::unique_lock<std::mutex> ul(txrq_lock, std::defer_lock);
	while (true) {
		std::cout << "tx thread living" << std::endl;
		ul.lock();

		if (txrq.empty()) {
			std::cout << "empty? still" << std::endl;
			txrq_cv.wait(ul, [this]{ return !txrq.empty(); });
		}

        auto req = std::move(txrq.front());
        txrq.pop();
        ul.unlock();

        std::this_thread::sleep_for(std::chrono::microseconds(100));
        int identity = req->identify;
				bool check_reads;
				int final_decision;
				std::array<tx_read, 3> tx_reads;
				std::array<tx_write, 3> tx_writes;

        switch (identity)
        {
        case PREPARE:
            local_tx = req->transaction;
          	tx_reads = local_tx.GetTxReads();
						check_reads = true;
            for (int i = 0; i < 3; i++) {
                tx_read each_read = tx_reads[i];
                each_read.Print();
                int read_rid = each_read.GetRobotId();
								read_rid = read_rid % kv_tbl_size;
                if (read_rid >= kv_base && read_rid <= (kv_base + kv_tbl_size - 1)) {
                    int read_ver = each_read.GetVersionNumber();
                    kv_table_lock.lock();
                    // copy
                    kv_value kv_pair = kv_table[read_rid];
                    kv_table_lock.unlock();
                    std::cout << "kv_version " << kv_pair.version << std::endl;
                    std::cout << "read_ver " << read_ver << std::endl;
                    if (read_ver < kv_pair.version) {
                        check_reads = false;
                    }
                }
            }
            if (check_reads) {
                req->decision.set_value(1);
            } else {
                req->decision.set_value(-1);
            }
            break;
        case COMMIT_ABORT:
            final_decision = req->TM_Req;
            if (final_decision == 1) {
								std::cout << "RM commit" << std::endl;
                tx_writes = local_tx.GetTxWrites();
                for (int j = 0; j < 3; j++) {
                    tx_write each_write = tx_writes[j];
										each_write.Print();
                    int write_rid = each_write.GetRobotId();
										write_rid = write_rid % kv_tbl_size;
                    if ( write_rid >= kv_base && write_rid <= (kv_base + kv_tbl_size - 1)) {
                        int new_bid = each_write.GetBid();
                        int write_cid = each_write.GetCustomerId();
                        kv_value new_entry;
												std::cout << "check local tx: ";
												local_tx.Print();
                        new_entry.bid = new_bid;
                        new_entry.customer_id = write_cid;
                        new_entry.version = local_tx.GetVersionNumber();
                        kv_table_lock.lock();
                        kv_table[write_rid] = new_entry;
												std::cout << "after write: " ;
                        std::cout << kv_table[write_rid].bid;
                        std::cout << kv_table[write_rid].customer_id;
                        std::cout << kv_table[write_rid].version << std::endl;
                        kv_table_lock.unlock();
                    }
			    		}
            }
            req->decision.set_value(COMPLETE_SIG);
            break;
        default:
            break;
        }
	}
	std::cout << "quit" << std::endl;
}
