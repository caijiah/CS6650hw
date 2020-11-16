#include "Messages.h"

#include <arpa/inet.h>
#include <cstring>
#include <iostream>

RobotOrder::RobotOrder() {
	customer_id = -1;
	order_number = -1;
	robot_type = -1;
}

void RobotOrder::SetOrder(int id, int number, int type) {
	customer_id = id;
	order_number = number;
	robot_type = type;
}

int RobotOrder::GetCustomerId() { return customer_id; }
int RobotOrder::GetOrderNumber() { return order_number; }
int RobotOrder::GetRobotType() { return robot_type; }

int RobotOrder::Size() {
	return sizeof(customer_id) + sizeof(order_number) + sizeof(robot_type);
}

void RobotOrder::Marshal(char *buffer) {
	int net_customer_id = htonl(customer_id);
	int net_order_number = htonl(order_number);
	int net_robot_type = htonl(robot_type);
	int offset = 0;
	memcpy(buffer + offset, &net_customer_id, sizeof(net_customer_id));
	offset += sizeof(net_customer_id);
	memcpy(buffer + offset, &net_order_number, sizeof(net_order_number));
	offset += sizeof(net_order_number);
	memcpy(buffer + offset, &net_robot_type, sizeof(net_robot_type));
}

void RobotOrder::Unmarshal(char *buffer) {
	int net_customer_id;
	int net_order_number;
	int net_robot_type;
	int offset = 0;
	memcpy(&net_customer_id, buffer + offset, sizeof(net_customer_id));
	offset += sizeof(net_customer_id);
	memcpy(&net_order_number, buffer + offset, sizeof(net_order_number));
	offset += sizeof(net_order_number);
	memcpy(&net_robot_type, buffer + offset, sizeof(net_robot_type));

	customer_id = ntohl(net_customer_id);
	order_number = ntohl(net_order_number);
	robot_type = ntohl(net_robot_type);
}

bool RobotOrder::IsValid() {
	return (customer_id != -1);
}

void RobotOrder::Print() {
	std::cout << "id " << customer_id << " ";
	std::cout << "num " << order_number << " ";
	std::cout << "type " << robot_type << std::endl;
}

RobotInfo::RobotInfo() {
	customer_id = -1;
	order_number = -1;
	robot_type = -1;
	engineer_id = -1;
	expert_id = -1;
}

void RobotInfo::SetInfo(int id, int number, int type, int engid, int expid) {
	customer_id = id;
	order_number = number;
	robot_type = type;
	engineer_id = engid;
	expert_id = expid;
}

void RobotInfo::CopyOrder(RobotOrder order) {
	customer_id = order.GetCustomerId();
	order_number = order.GetOrderNumber();
	robot_type = order.GetRobotType();
}
void RobotInfo::SetEngineerId(int id) { engineer_id = id; }
void RobotInfo::SetExpertId(int id) { expert_id = id; }

int RobotInfo::GetCustomerId() { return customer_id; }
int RobotInfo::GetOrderNumber() { return order_number; }
int RobotInfo::GetRobotType() { return robot_type; }
int RobotInfo::GetEngineerId() { return engineer_id; }
int RobotInfo::GetExpertId() { return expert_id; }

int RobotInfo::Size() {
	return sizeof(customer_id) + sizeof(order_number) + sizeof(robot_type)
		+ sizeof(engineer_id) + sizeof(expert_id);
}

void RobotInfo::Marshal(char *buffer) {
	int net_customer_id = htonl(customer_id);
	int net_order_number = htonl(order_number);
	int net_robot_type = htonl(robot_type);
	int net_engineer_id = htonl(engineer_id);
	int net_expert_id = htonl(expert_id);
	int offset = 0;

	memcpy(buffer + offset, &net_customer_id, sizeof(net_customer_id));
	offset += sizeof(net_customer_id);
	memcpy(buffer + offset, &net_order_number, sizeof(net_order_number));
	offset += sizeof(net_order_number);
	memcpy(buffer + offset, &net_robot_type, sizeof(net_robot_type));
	offset += sizeof(net_robot_type);
	memcpy(buffer + offset, &net_engineer_id, sizeof(net_engineer_id));
	offset += sizeof(net_engineer_id);
	memcpy(buffer + offset, &net_expert_id, sizeof(net_expert_id));

}

void RobotInfo::Unmarshal(char *buffer) {
	int net_customer_id;
	int net_order_number;
	int net_robot_type;
	int net_engineer_id;
	int net_expert_id;
	int offset = 0;

	memcpy(&net_customer_id, buffer + offset, sizeof(net_customer_id));
	offset += sizeof(net_customer_id);
	memcpy(&net_order_number, buffer + offset, sizeof(net_order_number));
	offset += sizeof(net_order_number);
	memcpy(&net_robot_type, buffer + offset, sizeof(net_robot_type));
	offset += sizeof(net_robot_type);
	memcpy(&net_engineer_id, buffer + offset, sizeof(net_engineer_id));
	offset += sizeof(net_engineer_id);
	memcpy(&net_expert_id, buffer + offset, sizeof(net_expert_id));

	customer_id = ntohl(net_customer_id);
	order_number = ntohl(net_order_number);
	robot_type = ntohl(net_robot_type);
	engineer_id = ntohl(net_engineer_id);
	expert_id = ntohl(net_expert_id);
}

bool RobotInfo::IsValid() {
	return (customer_id != -1);
}

void RobotInfo::Print() {
	std::cout << "id " << customer_id << " ";
	std::cout << "num " << order_number << " ";
	std::cout << "type " << robot_type << " ";
	std::cout << "engid " << engineer_id << " ";
	std::cout << "expid " << expert_id << std::endl;
}

tx_read::tx_read() {
	robot_id = -1;
	version = -1;
}
void tx_read::SetInfo(int rid, int ver) {
	robot_id = rid;
	version = ver;
}

void tx_read::CopyTxRead(tx_read rd) {
	robot_id = rd.GetRobotId();
	version = rd.GetVersionNumber();
}

void tx_read::SetRobortId(int rid) {
	robot_id = rid;
}

void tx_read::SetVersion(int ver) {
	version = ver;
}

int tx_read::GetRobotId() {
	return robot_id;
}

int tx_read::GetVersionNumber() {
	return version;
}

int tx_read::Size() {
	return sizeof(robot_id) + sizeof(version);
}

void tx_read::Marshal(char *buffer) {
	int net_robot_id = htonl(robot_id);
	int net_version = htonl(version);
	int offset = 0;

	memcpy(buffer + offset, &net_robot_id, sizeof(net_robot_id));
	offset += sizeof(net_robot_id);
	memcpy(buffer + offset, &net_version, sizeof(net_version));
}

void tx_read::Unmarshal(char *buffer){
	int net_robot_id;
	int net_version;
	int offset = 0;

	memcpy(&net_robot_id, buffer + offset, sizeof(net_robot_id));
	offset += sizeof(net_robot_id);
	memcpy(&net_version, buffer + offset, sizeof(net_version));

	robot_id = ntohl(net_robot_id);
	version = ntohl(net_version);
}

bool tx_read::IsValid() {
	return (robot_id != -1);
}

void tx_read::Print(){
	std::cout << "rid " << robot_id << " ";
	std::cout << "ver " << version << std::endl;
}

tx_write::tx_write() {
	robot_id = -1;
	bid = -1;
	customer_id = -1;
}

void tx_write::SetInfo(int rid, int bd, int cid) {
	robot_id = rid;
	bid = bd;
	customer_id = cid;
}

void tx_write::CopyTxWrite(tx_write wr) {
	robot_id = wr.GetRobotId();
	bid = wr.GetBid();
	customer_id = wr.GetCustomerId();
}

void tx_write::SetRobortId(int rid) {
	robot_id = rid;
}

void tx_write::SetBid(int bd) {
	bid = bd;
}

void tx_write::SetCustomerId(int cid) {
	customer_id = cid;
}

int tx_write::GetRobotId() {
	return robot_id;
}

int tx_write::GetBid() {
	return bid;
}

int tx_write::GetCustomerId() {
	return customer_id;
}

int tx_write::Size() {
	return sizeof(robot_id) + sizeof(bid) + sizeof(customer_id);
}

void tx_write::Marshal(char *buffer) {
	int net_robot_id = htonl(robot_id);
	int net_bid = htonl(bid);
	int net_customer_id = htonl(customer_id);
	int offset = 0;

	memcpy(buffer + offset, &net_robot_id, sizeof(net_robot_id));
	offset += sizeof(net_robot_id);
	memcpy(buffer + offset, &net_bid, sizeof(net_bid));
	offset += sizeof(net_bid);
	memcpy(buffer + offset, &net_customer_id, sizeof(net_customer_id));
}

void tx_write::Unmarshal(char *buffer) {
	int net_robot_id;
	int net_bid;
	int net_customer_id;
	int offset = 0;

	memcpy(&net_robot_id, buffer + offset, sizeof(net_robot_id));
	offset += sizeof(net_robot_id);
	memcpy(&net_bid, buffer + offset, sizeof(net_bid));
	offset += sizeof(net_customer_id);
	memcpy(&net_customer_id, buffer + offset, sizeof(net_customer_id));

	robot_id = ntohl(net_robot_id);
	bid = ntohl(net_bid);
	customer_id = ntohl(net_customer_id);
}

bool tx_write::IsValid() {
	return (robot_id != -1);
}

void tx_write::Print() {
	std::cout << "rid " << robot_id << " ";
	std::cout << "bid " << bid << " ";
	std::cout << "cid " << customer_id << std::endl;
}

tx::tx() {
	version = -1;
}

void tx::SetInfo(int ver, std::array<tx_read, 3> reads, std::array<tx_write, 3> writes) {
	version = ver;
	tx_reads = reads;
	tx_writes = writes;
}

void tx::CopyTx(tx trans) {
	version = trans.GetVersionNumber();
	tx_reads = trans.GetTxReads();
	tx_writes = trans.GetTxWrites();
}

void tx::SetVersionNumber(int ver) {
	version = ver;
}

void tx::SetTxReads(std::array<tx_read, 3> reads) {
	tx_reads = reads;
}

void tx::SetTxWrites(std::array<tx_write, 3> writes) {
	tx_writes = writes;
}

int tx::GetVersionNumber() {
	return version;
}
std::array<tx_read, 3> tx::GetTxReads() {
	return tx_reads;
}
std::array<tx_write, 3> tx::GetTxWrites() {
	return tx_writes;
}

int tx::Size() {
	int reads_size = 0;
	int writes_size = 0;
	for(auto i = tx_reads.begin(); i != tx_reads.end(); ++i)
    {
    	reads_size += i->Size();
    }
	for(auto j = tx_writes.begin(); j != tx_writes.end(); ++j)
    {
    	writes_size += j->Size();
    }

	return sizeof(version) + reads_size + writes_size;
}



void tx::Marshal(char *buffer) {
	int net_version = htonl(version);
	int offset = 0;

	memcpy(buffer + offset, &net_version, sizeof(net_version));
	offset += sizeof(net_version);

	for(auto i = tx_reads.begin(); i != tx_reads.end(); ++i)
    {
		char read_buffer[i->Size()];
		i->Marshal(read_buffer);
		memcpy(buffer + offset, &read_buffer, i->Size());
		offset += i->Size();
    }

	for(auto j = tx_writes.begin(); j != tx_writes.end(); ++j)
    {
		char write_buffer[j->Size()];
		j->Marshal(write_buffer);
		memcpy(buffer + offset, &write_buffer, j->Size());
		offset += j->Size();
    }
}

void tx::Unmarshal(char *buffer) {
	int net_version;
	int offset = 0;

	memcpy(&net_version, buffer + offset, sizeof(net_version));
	offset += sizeof(net_version);
	version = ntohl(net_version);

	for(auto i = tx_reads.begin(); i != tx_reads.end(); ++i)
    {
		char read_buffer[i->Size()];
		memcpy(&read_buffer, buffer + offset, i->Size());
		i->Unmarshal(read_buffer);
		offset += i->Size();
    }

	for(auto j = tx_writes.begin(); j != tx_writes.end(); ++j)
    {
		char write_buffer[j->Size()];
		memcpy(&write_buffer, buffer + offset, j->Size());
		j->Unmarshal(write_buffer);
		offset += j->Size();
    }
}

bool tx::IsValid() {
	return (version != -1);
}

void tx::Print() {
	std::cout << "ver " << version << std::endl;
	for(auto i = tx_reads.begin(); i != tx_reads.end(); ++i)
    {
		i->Print();
    }

	for(auto j = tx_writes.begin(); j != tx_writes.end(); ++j)
    {
		j->Print();
    }
}

ReadResponse::ReadResponse() {
	bid = -1;
	customer_id = -1;
	version = -1;
}

void ReadResponse::SetInfo(int bd, int cid, int ver) {
	bid = bd;
	customer_id = cid;
	version = ver;
}

void ReadResponse::CopyReadRes(ReadResponse read_res) {
	bid = read_res.GetBid();
	customer_id = read_res.GetCustomerId();
	version = read_res.GetVersionNumber();
}

void ReadResponse::SetBid(int bd) {
	bid = bd;
}

void ReadResponse::SetCustomerId(int cid) {
	customer_id = cid;
}

void ReadResponse::SetVersion(int ver) {
	version = ver;
}

int ReadResponse::GetBid() {
	return bid;
}

int ReadResponse::GetCustomerId() {
	return customer_id;
}

int ReadResponse::GetVersionNumber() {
	return version;
}

int ReadResponse::Size() {
	return sizeof(bid) + sizeof(customer_id) + sizeof(version);
}

void ReadResponse::Marshal(char *buffer) {
	int net_bid = htonl(bid);
	int net_customer_id = htonl(customer_id);
	int net_version = htonl(version);
	int offset = 0;

	memcpy(buffer + offset, &net_bid, sizeof(net_bid));
	offset += sizeof(net_bid);
	memcpy(buffer + offset, &net_customer_id, sizeof(net_customer_id));
	offset += sizeof(net_customer_id);
	memcpy(buffer + offset, &net_version, sizeof(net_version));
}

void ReadResponse::Unmarshal(char *buffer) {
	int net_bid;
	int net_customer_id;
	int net_version;
	int offset = 0;

	memcpy(&net_bid, buffer + offset, sizeof(net_bid));
	offset += sizeof(net_bid);
	memcpy(&net_customer_id, buffer + offset, sizeof(net_customer_id));
	offset += sizeof(net_customer_id);
	memcpy(&net_version, buffer + offset, sizeof(net_version));

	bid = ntohl(net_bid);
	customer_id = ntohl(net_customer_id);
	version = ntohl(net_version);
}

bool ReadResponse::IsValid() {
	return (customer_id != -1);
}

void ReadResponse::Print() {
	std::cout << "bid " << bid << " ";
	std::cout << "cid " << customer_id << " ";
	std::cout << "ver " << version << std::endl;
}
