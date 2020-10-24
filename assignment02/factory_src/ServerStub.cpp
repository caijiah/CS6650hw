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