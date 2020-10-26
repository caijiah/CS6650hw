 #include "Peer.h"

 Peer::Peer() {
    peer_id = -1;
    peer_ip = "";
    peer_port = -1;
 }

Peer::Peer(int p_id, std::string p_ip, int p_port) {
    peer_id = p_id;
    peer_ip = p_id;
    peer_port = p_port;
}

void Peer::SetPeerID(int p_id) {
    peer_id = p_id;
}
void Peer::SetPeerIP(std::string p_ip) {
    peer_ip = p_ip;
}
void Peer::SetPeerPort(int p_port) {
    peer_port = p_port;
}



int Peer::GetPeerID() {
    return peer_id;
}
std::string Peer::GetPeerIP() {
    return peer_ip;
}
int Peer::GetPeerPort() {
    return peer_port;
}