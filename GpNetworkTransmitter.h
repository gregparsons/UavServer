/**
 Transmit and receive network packets.
 */


#ifndef __controllerHello__GpNetworkTransmitter__
#define __controllerHello__GpNetworkTransmitter__

#include <cstdint>
#include <string>
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>		//sendto()


#include <mavlink/c_library/common/mavlink.h>

class GpControllerEvent;

class GpNetworkTransmitter{
public:
	//Constructor for recipient
	GpNetworkTransmitter();
	
	//Constructor for sender,
	GpNetworkTransmitter(std::string destAddress);
	
	~GpNetworkTransmitter();
	
	
	// Sender
	void transmitEvent(mavlink_message_t & mesg);

	// Receiver/Listener
	void listenOnNetwork();
	
private:
	std::string _destIp;

	int _socketFd = 0;
	struct addrinfo _hintInfo, *_senderInfo = nullptr, *_addrInfo=nullptr;		//netdb.h
	
};

#endif /* defined(__controllerHello__GpNetworkTransmitter__) */
