#include <cstring>
#include <arpa/inet.h>

#include "FactoryStub.h"

FactoryStub::FactoryStub() {}

int FactoryStub::Init(std::string ip, int port) {
	return socket.Init(ip, port);
}

void FactoryStub::SendIdentifyMessage(IdentifyMessage identify_message) {
	int buff_size = sizeof(int);
	char buffer[buff_size];
	identify_message.Marshal(buffer);
	int size = identify_message.Size();
	socket.Send(buffer, size, 0);
}

int FactoryStub::SendReplicationRequest(ReplicationRequest replica_req) {
	int net_res;
	int buff_size = sizeof(int) * 6;
	char buffer[buff_size];
	replica_req.Marshal(buffer);
	int size = replica_req.Size();
	if (socket.Send(buffer, size, 0)) {
		size = sizeof(net_res);
		if (socket.Recv(buffer, size, 0)) {
			memcpy(&net_res, buffer, sizeof(net_res));
		}
	}
	return ntohl(net_res);
}
