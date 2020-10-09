//
//  ClientSocket.h
//  cs6650_assignment1
//
//

#ifndef ClientSocket_h
#define ClientSocket_h

#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string>

class ClientSocket {
    int sockfd;
    struct sockaddr_in servinfo;
    
public:
    ClientSocket(std::string ip, int port);
    ~ClientSocket();
    void cli_sock_connect();
    void cli_sock_close();
    void send_msg(char* buffer, std::size_t size);
    void recv_msg(char* buffer);
};

#endif /* ClientSocket_h */
