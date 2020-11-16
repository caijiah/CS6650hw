#include <cstring>
#include <iostream>
#include <arpa/inet.h>

#include "ServerStub.h"

ServerStub::ServerStub() {}

void ServerStub::Init(std::unique_ptr<ServerSocket> socket) {
	this->socket = std::move(socket);
}

CustomerRequest ServerStub::ReceiveRequest() {
	char buffer[32];
	CustomerRequest crq;
	if (socket->Recv(buffer, crq.Size(), 0)) {
		crq.Unmarshal(buffer);
	}
	return crq;
}

int ServerStub::ShipRobot(RobotInfo info) {
	char buffer[32];
	info.Marshal(buffer);
	return socket->Send(buffer, info.Size(), 0);
}

int ServerStub::ReturnRecord(CustomerRecord cus_rec) {
	char buffer[16];
	cus_rec.Marshal(buffer);
	return socket->Send(buffer, cus_rec.Size(), 0);
}

IdentifyMessage ServerStub::ReceiveIdentifyMessage() {
	char buffer[sizeof(int)];
	IdentifyMessage identify_message;
	if (socket->Recv(buffer, identify_message.Size(), 0)) {
		identify_message.Unmarshal(buffer);
	}
	return identify_message;
}

ReplicationRequest ServerStub::ReceiveReplicationRequest() {
	int buff_size = sizeof(int) * 6;
	char buffer[buff_size];
	ReplicationRequest replication_request;
	if (socket->Recv(buffer, replication_request.Size(), 0)) {
		replication_request.Unmarshal(buffer);
	}
	return replication_request;
}

int ServerStub::ReturnReplicaResponse(int last_index) {
	char buffer[sizeof(int)];
	int net_last_index = htonl(last_index);
	memcpy(buffer, &net_last_index, sizeof(net_last_index));
	return socket->Send(buffer, sizeof(int), 0);
}
