#ifndef __MESSAGES_H__
#define __MESSAGES_H__

#include <string>
#include <array>

#define TX_READ_IDENTIFY 1
#define TX_IDENTIFY 2
#define TM_RM_IDENTIFY 3
#define COMPLETE_SIG 4

class RobotOrder {
private:
	int customer_id;
	int order_number;
	int robot_type;

public:
	RobotOrder();
	void operator = (const RobotOrder &order) {
		customer_id = order.customer_id;
		order_number = order.order_number;
		robot_type = order.robot_type;
	}
	void SetOrder(int cid, int order_num, int type);
	int GetCustomerId();
	int GetOrderNumber();
	int GetRobotType();

	int Size();

	void Marshal(char *buffer);
	void Unmarshal(char *buffer);

	bool IsValid();

	void Print();
};

class RobotInfo {
private:
	int customer_id;
	int order_number;
	int robot_type;
	int engineer_id;
	int expert_id;

public:
	RobotInfo();
	void operator = (const RobotInfo &info) {
		customer_id = info.customer_id;
		order_number = info.order_number;
		robot_type = info.robot_type;
		engineer_id = info.engineer_id;
		expert_id = info.expert_id;
	}
	void SetInfo(int cid, int order_num, int type, int engid, int expid);
	void CopyOrder(RobotOrder order);
	void SetEngineerId(int id);
	void SetExpertId(int id);

	int GetCustomerId();
	int GetOrderNumber();
	int GetRobotType();
	int GetEngineerId();
	int GetExpertId();

	int Size();

	void Marshal(char *buffer);
	void Unmarshal(char *buffer);

	bool IsValid();

	void Print();
};

class tx_read {
private:
	int robot_id;
	int version;
public:
	tx_read();
	void operator = (const tx_read &read_tx) {
		robot_id = read_tx.robot_id;
		version = read_tx.version;
	}
	void SetInfo(int rid, int ver);
	void CopyTxRead(tx_read rd);
	void SetRobortId(int rid);
	void SetVersion(int ver);

	int GetRobotId();
	int GetVersionNumber();

	int Size();

	void Marshal(char *buffer);
	void Unmarshal(char *buffer);

	bool IsValid();

	void Print();
};

class tx_write {
private:
	int robot_id;
	int bid;
	int customer_id;	
public:
	tx_write();
	void operator = (const tx_write &write_tx) {
		robot_id = write_tx.robot_id;
		bid = write_tx.bid;
		customer_id = write_tx.customer_id;
	}
	void SetInfo(int rid, int bd, int cid);
	void CopyTxWrite(tx_write wr);
	void SetRobortId(int rid);
	void SetBid(int bd);
	void SetCustomerId(int cid);

	int GetRobotId();
	int GetBid();
	int GetCustomerId();

	int Size();

	void Marshal(char *buffer);
	void Unmarshal(char *buffer);

	bool IsValid();

	void Print();
};

class tx {
private:
	int version;
	std::array<tx_read, 3> tx_reads;
	std::array<tx_write, 3> tx_writes;
public:
	tx();
	void operator = (const tx &trans) {
		version = trans.version;
		tx_reads = trans.tx_reads;
		tx_writes = trans.tx_writes;
	}
	void SetInfo(int ver, std::array<tx_read, 3> reads, std::array<tx_write, 3> writes);
	void CopyTx(tx trans);
	void SetVersionNumber(int ver);
	void SetTxReads(std::array<tx_read, 3> reads);
	void SetTxWrites(std::array<tx_write, 3> writes);

	int GetVersionNumber();
	std::array<tx_read, 3> GetTxReads();
	std::array<tx_write, 3> GetTxWrites();

	int Size();

	void Marshal(char *buffer);
	void Unmarshal(char *buffer);

	bool IsValid();

	void Print();
};

class ReadResponse {
private:
	int bid;
	int customer_id;
	int version; 
public:
	ReadResponse();
	void operator = (const ReadResponse &read_res) {
		bid = read_res.bid;
		customer_id = read_res.customer_id;
		version = read_res.version;
	}
	void SetInfo(int bd, int cid, int ver);
	void CopyReadRes(ReadResponse read_res);
	void SetBid(int bd);
	void SetCustomerId(int cid);
	void SetVersion(int ver);

	int GetBid();
	int GetCustomerId();
	int GetVersionNumber();

	int Size();

	void Marshal(char *buffer);
	void Unmarshal(char *buffer);

	bool IsValid();

	void Print();
};


#endif // #ifndef __MESSAGES_H__
