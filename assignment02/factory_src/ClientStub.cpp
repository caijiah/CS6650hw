#include "ClientStub.h"
#include <arpa/inet.h>
#include <cstring>

ClientStub::ClientStub() {}

int ClientStub::Init(std::string ip, int port) {
	return socket.Init(ip, port);
}

RobotInfo ClientStub::Order(CustomerRequest crq) {
	RobotInfo info;
	char buffer[32];
	int size;
	crq.Marshal(buffer);
	size = crq.Size();
	if (socket.Send(buffer, size, 0)) {
		size = info.Size();
		if (socket.Recv(buffer, size, 0)) {
			info.Unmarshal(buffer);
		}
	}
	return info;
}

CustomerRecord ClientStub::ReadRecord(CustomerRequest crq) {
	CustomerRecord cus_rec;
	char buffer[32];
	int size;
	crq.Marshal(buffer);
	size = crq.Size();
	if (socket.Send(buffer, size, 0)) {
		size = cus_rec.Size();
		if (socket.Recv(buffer, size, 0)) {
			cus_rec.Unmarshal(buffer);
		}
	}
	return cus_rec;
}

int ClientStub::SendIdentifyMessage(IdentifyMessage identify_message) {
	int buff_size = sizeof(int);
	char buffer[buff_size];
	int net_response;
	identify_message.Marshal(buffer);
	int size = identify_message.Size();
	if (socket.Send(buffer, size, 0)) {
		size = buff_size;
		if (socket.Recv(buffer, size, 0)) {
			memcpy(&net_response, buffer, sizeof(net_response));
		}
	}
	return ntohl(net_response);
}
