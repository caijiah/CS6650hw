#include <cstring>

#include "FactoryStub.h"

FactoryStub::FactoryStub() {}

int FactoryStub::Init(std::string ip, int port) {
	return socket.Init(ip, port);
}


// RobotInfo ClientStub::Order(CustomerRequest crq) {
// 	RobotInfo info;
// 	char buffer[32];
// 	int size;
// 	crq.Marshal(buffer);
// 	size = crq.Size();
// 	if (socket.Send(buffer, size, 0)) {
// 		size = info.Size();
// 		if (socket.Recv(buffer, size, 0)) {
// 			info.Unmarshal(buffer);
// 		}
// 	}
// 	return info;
// }

void FactoryStub::SendIdentifyMessage(IdentifyMessage identify_message) {
	int buff_size = sizeof(int);
	char buffer[buff_size];
	identify_message.Marshal(buffer);
	int size = identify_message.Size();
	socket.Send(buffer, size, 0);
}

int FactoryStub::SendReplicationRequest(ReplicationRequest replica_req) {
	int buff_size = sizeof(int) * 6;
	char buffer[buff_size];
	replica_req.Marshal(buffer);
	int size = replica_req.Size();
	if (socket.Send(buffer, size, 0)) {
		size = sizeof(int);
		if (socket.Recv(buffer, size, 0)) {
			int net_res;
			memcpy(&net_res, buffer, sizeof(net_res));
			return ntohl(net_res);
		}
	}
}