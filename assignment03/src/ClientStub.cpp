#include "ClientStub.h"

#include <arpa/inet.h>
#include <cstring>
#include <iostream>


ClientStub::ClientStub() {}

int ClientStub::Init(std::string ip, int port) {
	return socket.Init(ip, port);
}

RobotInfo ClientStub::OrderRobot(RobotOrder order) {
	RobotInfo info;
	char buffer[32];
	int size;
	order.Marshal(buffer);
	size = order.Size();
	if (socket.Send(buffer, size, 0)) {
		size = info.Size();
		if (socket.Recv(buffer, size, 0)) {
			info.Unmarshal(buffer);
		}
	}
	return info;
}

ReadResponse ClientStub::SendRead(tx_read read_req) {
	ReadResponse res;
	char tx_read_buffer[16];
	read_req.Marshal(tx_read_buffer);

	char buffer[20];
	int net_identify = htonl(TX_READ_IDENTIFY);
	int offset = 0;
	int size;
	memcpy(buffer + offset, &net_identify, sizeof(net_identify));
	offset += sizeof(net_identify);
	memcpy(buffer + offset, tx_read_buffer, read_req.Size());
	size = read_req.Size() + sizeof(net_identify);

	if (int sd = socket.Send(buffer, size, 0)) {
		size = res.Size();
		std::cout << "sd " << sd << " ";
		std::cout << "size " << size << std::endl;
		if (socket.Recv(buffer, size, 0)) {
			res.Unmarshal(buffer);
		}
	}
	return res;
}

int ClientStub::SendTX(tx transcation) {
	int res;
	char tx_buffer[64];
	transcation.Marshal(tx_buffer);

	char buffer[68];
	int net_identify = htonl(TX_IDENTIFY);
	int offset = 0;
	int size;
	memcpy(buffer + offset, &net_identify, sizeof(net_identify));
	offset += sizeof(net_identify);
	memcpy(buffer + offset, tx_buffer, transcation.Size());
	size = transcation.Size() + sizeof(net_identify);

	if (socket.Send(buffer, size, 0)) {
		size = sizeof(int);
		if (socket.Recv(buffer, size, 0)) {
			int net_res;
			memcpy(&net_res, buffer, sizeof(net_res));
			res = ntohl(net_res);
		}
	}
	return res;
}
