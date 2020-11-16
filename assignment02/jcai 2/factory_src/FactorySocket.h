#ifndef __FACTORYSOCKET_H__
#define __FACTORYSOCKET_H__

#include <string>

#include "Socket.h"


class FactorySocket: public Socket {
public:
	FactorySocket() {}
	~FactorySocket() {}

	int Init(std::string ip, int port);
};


#endif // end of #ifndef __FACTORYSOCKET_H__
