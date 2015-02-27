//
//  GpNetworkTransmitter.cpp

#include "GpNetworkTransmitter.h"
#include "GpMavlink.h"

// De/Constructors

// Receive constructor
GpNetworkTransmitter::GpNetworkTransmitter(){
	
	memset(&_hintInfo, 0, sizeof(_hintInfo));
	_hintInfo.ai_family = AF_UNSPEC;			//force IPV4??
	_hintInfo.ai_socktype = SOCK_DGRAM;			//UDP
	_hintInfo.ai_flags = AI_PASSIVE;			//use this IP
	int ok = getaddrinfo(nullptr, SENDPORT.c_str(), &_hintInfo, &_senderInfo);		//should be _receiverInfo
	if(ok != 0)	{ return; }			//fail
	
	// Socket
	
	for(_addrInfo = _senderInfo; _addrInfo!=nullptr; _addrInfo = _addrInfo->ai_next){
		if(-1 == (_socketFd = socket(_addrInfo->ai_family, _addrInfo->ai_socktype, _addrInfo->ai_protocol))){ //domain, type, protocol
			//failed, not this one
			continue;
		}
		
		// Bind
		
		ok = ::bind(_socketFd, _addrInfo->ai_addr, _addrInfo->ai_addrlen);
		if(ok == -1){
			close(_socketFd);
			continue;
		}
		break;	//either found a good one or done looking
	}
}


// Send constructor
GpNetworkTransmitter::GpNetworkTransmitter(std::string destAddress):_destIp(destAddress){
	
	memset(&_hintInfo, 0, sizeof(_hintInfo));	//zeroize
	_hintInfo.ai_family = AF_UNSPEC;
	_hintInfo.ai_socktype = SOCK_DGRAM;			//UDP
	
	int ok = getaddrinfo(destAddress.c_str(), SENDPORT.c_str(), &_hintInfo, &_senderInfo);
	if(0 != ok){
		std::cout << "Bad IP address" << std::endl;
		exit(-1);
	}
	
	
	for(_addrInfo = _senderInfo; _addrInfo!=nullptr; _addrInfo = _addrInfo->ai_next){
		if(-1 == (_socketFd = socket(_addrInfo->ai_family, _addrInfo->ai_socktype, _addrInfo->ai_protocol))){ //domain, type, protocol
			//failed, not this one
			continue;
		}
		break;	//either found a good one or done looking
	}
	
	
	if(_addrInfo == nullptr){
		std::cout << "Couldn't create socket" << std::endl;
		_socketFd = 0;
		return;
	}
}

GpNetworkTransmitter::~GpNetworkTransmitter(){
	freeaddrinfo(_senderInfo);
	close(_socketFd);
	return;
}



// Transmit

void GpNetworkTransmitter::transmitEvent(mavlink_message_t & mesg){
	
	int size = MAVLINK_MSG_ID_RC_CHANNELS_OVERRIDE_LEN;
	
	if(_addrInfo != nullptr){
	
		
		// sendto()
		
		ssize_t byteCount = 0;
		byteCount = sendto(_socketFd, &mesg, (size_t)size, 0, _addrInfo->ai_addr, _addrInfo->ai_addrlen);
		if(byteCount == -1){
			std::cout << "sendto() failed" << std::endl;
			exit(1);
		}
	}
}



// Receive

void GpNetworkTransmitter::listen(){
	
	struct sockaddr_storage socketStorage;
	socklen_t addressLen = sizeof(socketStorage);
	ssize_t byteCount = 0;
	uint8_t *buffer = new uint8_t[MAVLINK_MSG_ID_RC_CHANNELS_OVERRIDE_LEN];
	//std::string ipv6Address[INET6_ADDRSTRLEN];
	
	
	GpMavlink gpMavlink;
	for(;;)
	{
		
		// recv()
		
		byteCount = recv(_socketFd, buffer, MAVLINK_MSG_ID_RC_CHANNELS_OVERRIDE_LEN, 0); //recvfrom(_socketFd, buffer, MAVLINK_MSG_ID_RC_CHANNELS_OVERRIDE_LEN, 0, (struct sockaddr *)&socketStorage, &addressLen);

		if(byteCount > 0){
			
			mavlink_message_t message;
			memcpy(&message, buffer, MAVLINK_MSG_ID_RC_CHANNELS_OVERRIDE_LEN);
			gpMavlink.receiveTestMessage(message);
			
		}
	}
	free(buffer);
	
}
