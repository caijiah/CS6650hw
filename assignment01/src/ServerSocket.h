//
//  ServerSocket.h
//  cs6650_assignment1
//
//

#ifndef ServerSocket_h
#define ServerSocket_h

#define BACKLOG SOMAXCONN

class ServerSocket {
    int sockfd;
    
public:
    ServerSocket(int given_port);
    ~ServerSocket();
    int ser_sock_accept();
};

#endif /* ServerSocket_h */
