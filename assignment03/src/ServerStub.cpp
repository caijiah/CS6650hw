#include "ServerStub.h"

#include <cstring>
#include <iostream>
#include <arpa/inet.h>

ServerStub::ServerStub() {}

void ServerStub::Init(std::unique_ptr<ServerSocket> socket) {
	this->socket = std::move(socket);
}

int ServerStub::ReadIdentify() {
	char buffer[4];
	int net_identify;
	std::cout << "read" << std::endl;
	if (socket->Recv(buffer, sizeof(net_identify), 0)) {
		memcpy(&net_identify, buffer, sizeof(net_identify));
		return ntohl(net_identify);
	} else {
		return -1;
	}
}

tx_read ServerStub::ReceiveTxRead() {
	char buffer[16];
	tx_read tx_r;
	if (socket->Recv(buffer, tx_r.Size(), 0)) {
		tx_r.Unmarshal(buffer);
	}
	return tx_r;
}

tx ServerStub::ReceiveTX() {
	char buffer[64];
	tx transcation;
	if (socket->Recv(buffer, transcation.Size(), 0)) {
		transcation.Unmarshal(buffer);
	}
	return transcation;
}

int ServerStub::SendDecision(int d) {
	char buffer[4];
	int net_d = htonl(d);
	memcpy(buffer + 0, &net_d, sizeof(d));
	return socket->Send(buffer, sizeof(d), 0);
}

int ServerStub::SendReadResponse(ReadResponse r_res) {
	char buffer[12];
	r_res.Marshal(buffer);
	return socket->Send(buffer, r_res.Size(), 0);
}
