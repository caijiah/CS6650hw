#include "ClientStub.h"

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
	CustomerRecod cus_rec;
	char buffer[32]
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