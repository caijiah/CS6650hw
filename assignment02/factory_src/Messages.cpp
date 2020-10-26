#include <cstring>
#include <iostream>

#include <arpa/inet.h>
#include "Messages.h"

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

// ------------------Customer Request----------------------
CustomerRequest::CustomerRequest() {
	customer_id = -1;
	order_number = -1;
	request_type = -1;
}

void CustomerRequest::SetRequest(int id, int number, int req_type) {
	customer_id = id;
	order_number = number;
	request_type = req_type;
}

int CustomerRequest::GetCustomerId() { return customer_id; }
int CustomerRequest::GetOrderNumber() { return order_number; }
int CustomerRequest::GetRequestType() { return request_type; }

int CustomerRequest::Size() {
	return sizeof(customer_id) + sizeof(order_number) + sizeof(request_type);
}

void CustomerRequest::Marshal(char *buffer) {
	int net_customer_id = htonl(customer_id);
	int net_order_number = htonl(order_number);
	int net_request_type = htonl(request_type);
	int offset = 0;
	memcpy(buffer + offset, &net_customer_id, sizeof(net_customer_id));
	offset += sizeof(net_customer_id);
	memcpy(buffer + offset, &net_order_number, sizeof(net_order_number));
	offset += sizeof(net_order_number);
	memcpy(buffer + offset, &net_request_type, sizeof(net_request_type));
}

void CustomerRequest::Unmarshal(char *buffer) {
	int net_customer_id;
	int net_order_number;
	int net_request_type;
	int offset = 0;
	memcpy(&net_customer_id, buffer + offset, sizeof(net_customer_id));
	offset += sizeof(net_customer_id);
	memcpy(&net_order_number, buffer + offset, sizeof(net_order_number));
	offset += sizeof(net_request_type);
	memcpy(&net_request_type, buffer + offset, sizeof(net_request_type));

	customer_id = ntohl(net_customer_id);
	order_number = ntohl(net_order_number);
	request_type = ntohl(net_request_type);
}

bool CustomerRequest::IsValid() {
	return (customer_id != -1);
}

void CustomerRequest::Print() {
	std::cout << "id " << customer_id << " ";
	std::cout << "num " << order_number << " ";
	std::cout << "req_type " << request_type << std::endl;
}
// --------------------------------------------------------

RobotInfo::RobotInfo() {
	customer_id = -1;
	order_number = -1;
	request_type = -1;
	engineer_id = -1;
	admin_id = -1;
}

void RobotInfo::SetInfo(int id, int number, int req_type, int engid, int admid) {
	customer_id = id;
	order_number = number;
	request_type = req_type;
	engineer_id = engid;
	admin_id = admid;
}

void RobotInfo::CopyRequest(CustomerRequest request) {
	customer_id = request.GetCustomerId();
	order_number = request.GetOrderNumber();
	request_type = request.GetRequestType();
}
void RobotInfo::SetEngineerId(int id) { engineer_id = id; }
void RobotInfo::SetAdminId(int id) { admin_id = id; }

int RobotInfo::GetCustomerId() { return customer_id; }
int RobotInfo::GetOrderNumber() { return order_number; }
int RobotInfo::GetRequestType() { return request_type; }
int RobotInfo::GetEngineerId() { return engineer_id; }
int RobotInfo::GetAdminId() { return admin_id; }

int RobotInfo::Size() {
	return sizeof(customer_id) + sizeof(order_number) + sizeof(request_type)
		+ sizeof(engineer_id) + sizeof(admin_id);
}

void RobotInfo::Marshal(char *buffer) {
	int net_customer_id = htonl(customer_id);
	int net_order_number = htonl(order_number);
	int net_request_type = htonl(request_type);
	int net_engineer_id = htonl(engineer_id);
	int net_admin_id = htonl(admin_id);
	int offset = 0;

	memcpy(buffer + offset, &net_customer_id, sizeof(net_customer_id));
	offset += sizeof(net_customer_id);
	memcpy(buffer + offset, &net_order_number, sizeof(net_order_number));
	offset += sizeof(net_order_number);
	memcpy(buffer + offset, &net_request_type, sizeof(net_request_type));
	offset += sizeof(net_request_type);
	memcpy(buffer + offset, &net_engineer_id, sizeof(net_engineer_id));
	offset += sizeof(net_engineer_id);
	memcpy(buffer + offset, &net_admin_id, sizeof(net_admin_id));

}

void RobotInfo::Unmarshal(char *buffer) {
	int net_customer_id;
	int net_order_number;
	int net_request_type;
	int net_engineer_id;
	int net_admin_id;
	int offset = 0;

	memcpy(&net_customer_id, buffer + offset, sizeof(net_customer_id));
	offset += sizeof(net_customer_id);
	memcpy(&net_order_number, buffer + offset, sizeof(net_order_number));
	offset += sizeof(net_order_number);
	memcpy(&net_request_type, buffer + offset, sizeof(net_request_type));
	offset += sizeof(net_request_type);
	memcpy(&net_engineer_id, buffer + offset, sizeof(net_engineer_id));
	offset += sizeof(net_engineer_id);
	memcpy(&net_admin_id, buffer + offset, sizeof(net_admin_id));

	customer_id = ntohl(net_customer_id);
	order_number = ntohl(net_order_number);
	request_type = ntohl(net_request_type);
	engineer_id = ntohl(net_engineer_id);
	admin_id = ntohl(net_admin_id);
}

bool RobotInfo::IsValid() {
	return (customer_id != -1);
}

void RobotInfo::Print() {
	std::cout << "id " << customer_id << " ";
	std::cout << "num " << order_number << " ";
	std::cout << "req_type " << request_type << " ";
	std::cout << "engid " << engineer_id << " ";
	std::cout << "admid " << admin_id << std::endl;
}

CustomerRecord::CustomerRecord() {
	customer_id = -1;
	last_order = -1;
}

void CustomerRecord::SetRecord(int cid, int lo) {
	customer_id = cid;
	last_order = lo;
}

void CustomerRecord::SetCustomerId(int cid) { customer_id = cid; }
void CustomerRecord::SetLastOrder(int lo) { last_order = lo; }

int CustomerRecord::GetCustomerId() { return customer_id; }

int CustomerRecord::GetLastOrder() { return last_order; }

int CustomerRecord::Size() {
	return sizeof(customer_id) + sizeof(last_order);
}

void CustomerRecord::Marshal(char *buffer) {
	int net_customer_id = htonl(customer_id);
	int net_last_order = htonl(last_order);
	int offset = 0;

	memcpy(buffer + offset, &net_customer_id, sizeof(net_customer_id));
	offset += sizeof(net_customer_id);
	memcpy(buffer + offset, &net_last_order, sizeof(net_last_order));
}

void CustomerRecord::Unmarshal(char *buffer) {
	int net_customer_id;
	int net_last_order;
	int offset = 0;

	memcpy(&net_customer_id, buffer + offset, sizeof(net_customer_id));
	offset += sizeof(net_customer_id);
	memcpy(&net_last_order, buffer + offset, sizeof(net_last_order));

	customer_id = ntohl(net_customer_id);
	last_order = ntohl(net_last_order);
}

bool CustomerRecord::IsValid() {
	return (customer_id != -1);
}

void CustomerRecord::Print() {
	std::cout << "id " << customer_id << " ";
	std::cout << "last order " << last_order << " ";
}

// ------------------- MapOp -------------
MapOp::MapOp() {
	opcode = -1;
	arg1 = -1;
	arg2 = -2;
}

void MapOp::SetMapOp(int oc, int a1, int a2) {
	opcode = oc;
	arg1 = a1;
	arg2 = a2;
}

void MapOp::SetOpcode(int oc) {
	opcode = oc;
}

void MapOp::SetArg1(int a1) {
	arg1 = a1;
}

void MapOp::SetArg2(int a2) {
	arg2 = a2;
}

int MapOp::GetOpcode() {
	return opcode;
}

int MapOp::GetArg1() {
	return arg1;
}

int MapOp::GetArg2() {
	return arg2;
}

int MapOp::Size() {
	return sizeof(opcode) + sizeof(arg1) + sizeof(arg2);
}

void MapOp::Marshal(char * buffer) {
	int net_opcode = htonl(opcode);
	int net_arg1 = htonl(arg1);
	int net_arg2 = htonl(arg2);

	int offset = 0;

	memcpy(buffer + offset, &net_opcode, sizeof(net_opcode));
	offset += sizeof(net_opcode);
	memcpy(buffer + offset, &net_arg1, sizeof(net_arg1));
	offset += sizeof(net_arg1);
	memcpy(buffer + offset, &net_arg2, sizeof(net_arg2));
}

void MapOp::Unmarshal(char * buffer) {
	int net_opcode;
	int net_arg1;
	int net_arg2;

	int offset = 0;

	memcpy(&net_opcode, buffer + offset, sizeof(net_opcode));
	offset += sizeof(net_opcode);
	memcpy(&net_arg1, buffer + offset, sizeof(net_arg1));
	offset += sizeof(net_arg1);
	memcpy(&net_arg2, buffer + offset, sizeof(net_arg2));

	opcode = ntohl(net_opcode);
	arg1 = ntohl(net_arg1);
	arg2 = ntohl(net_arg2);
}

bool MapOp::IsValid() {
	return (opcode != -1);
}

void MapOp::Print() {
	std::cout << "opcode " << opcode << " ";
	std::cout << "arg1 " << arg1 << " ";
	std::cout << "arg2 " << arg2 << " ";
}


// ----------------- ReplicationRequest ----------------
ReplicationRequest::ReplicationRequest() {
	factory_id = -1;
	committed_index = -1;
	last_index = -1;
	map_op = MapOp();
}

void ReplicationRequest::SetRequest(int fid, int commit_index, int last_inde, MapOp mo) {
	factory_id = fid;
	committed_index = commit_index;
	last_index = last_inde;
	map_op = mo;
}

void ReplicationRequest::SetFactoryId(int fid) {
	factory_id = fid;
}
void ReplicationRequest::SetCommittedIndex(int commit_index) {
	committed_index = commit_index;
}

void ReplicationRequest::SetLastIndex(int last_inde) {
	last_index = last_inde;
}

void ReplicationRequest::SetMapOp(MapOp mo) {
	map_op = mo;
}

int ReplicationRequest::GetFactoryId() {
	return factory_id;
}
int ReplicationRequest::GetCommittedIndex() {
	return committed_index;
}
int ReplicationRequest::GetLastIndex() {
	return last_index;
}

int ReplicationRequest::Size() {
	return sizeof(factory_id) + sizeof(committed_index) + 
	       sizeof(last_index) + map_op.Size();
}

void ReplicationRequest::Marshal(char *buffer) {
	int net_factory_id = htonl(factory_id);
	int net_committed_index = htonl(committed_index);
	int net_last_index = htonl(last_index);

	int offset = 0;

	memcpy(buffer + offset, &net_factory_id, sizeof(net_factory_id));
	offset += sizeof(net_factory_id);
	memcpy(buffer + offset, &net_committed_index, sizeof(net_committed_index));
	offset += sizeof(net_committed_index);
	memcpy(buffer + offset, &net_last_index, sizeof(net_last_index));
	offset += sizeof(net_last_index);

	char map_buff[map_op.Size()];
	map_op.Marshal(map_buff);
	// check if it works
	memcpy(buffer + offset, &map_buff, map_op.Size());
}

void ReplicationRequest::Unmarshal(char *buffer) {
	int net_factory_id;
	int net_committed_index;
	int net_last_index;

	int offset = 0;

	memcpy(&net_factory_id, buffer + offset, sizeof(net_factory_id));
	offset += sizeof(net_factory_id);
	memcpy(&net_committed_index, buffer + offset, sizeof(net_committed_index));
	offset += sizeof(net_committed_index);
	memcpy(&net_last_index, buffer + offset, sizeof(net_last_index));
	offset += sizeof(net_last_index);

	factory_id = ntohl(net_factory_id);
	committed_index = ntohl(net_committed_index);
	last_index = ntohl(net_last_index);

	char map_buff[map_op.Size()];
	memcpy(&map_buff, buffer + offset, map_op.Size());
	map_op.Unmarshal(map_buff);
}

bool ReplicationRequest::IsValid() {
	return (factory_id != -1);
}

void ReplicationRequest::Print() {
	std::cout << "factory_id " << factory_id << " ";
	std::cout << "committed_index " << committed_index << " ";
	std::cout << "last_index " << last_index << " ";
	map_op.Print();
}


IdentifyMessage::IdentifyMessage() {
	identify = -1;
}

int IdentifyMessage::Size() {
	return sizeof(identify);
}

void IdentifyMessage::SetIdentifyFlag(int ident) {
	identify = ident;
}

int IdentifyMessage::GetIdentifyFlag() {
	return identify;
}

bool IdentifyMessage::IsValid() {
	return (identify != -1);
}


void IdentifyMessage::Marshal(char *buffer) {
	int net_identify = htonl(identify);
	memcpy(buffer, &net_identify, sizeof(net_identify));
}

void IdentifyMessage::Unmarshal(char *buffer) {
	int net_identify;
	memcpy(&net_identify, buffer, sizeof(net_identify));
	identify = ntohl(net_identify);
}