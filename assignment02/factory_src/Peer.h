#ifndef __PEER_H__
#define __PEER_H__

#include <string>
class Peer {
private:
	int peer_id;
	std::string peer_ip;
	int peer_port;
	bool status;
public:
    Peer();
    Peer(int p_id, std::string p_ip, int p_port);
    void SetPeerID(int p_id);
    void SetPeerIP(std::string p_ip);
    void SetPeerPort(int p_port);
		void SetPeerStatus(bool state);
		bool GetPeerStatus();
    int GetPeerID();
    std::string GetPeerIP();
    int GetPeerPort();
};

#endif // #ifndef __MESSAGES_H__
