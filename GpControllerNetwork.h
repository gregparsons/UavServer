// ********************************************************************************
//
//  GpControllerNetwork.h
//  UavServer
//  2/27/15
//
//  gp
//    Copyright (c) 2015 swimr. All rights reserved.
//
// ********************************************************************************


#ifndef __UavServer__GpControllerNetwork__
#define __UavServer__GpControllerNetwork__

#define GP_CONTROLLER_NET_MAX_BUFFER_LEN 512

#include <mavlink/c_library/common/mavlink.h>
#include <string>

class GpMessage;

class GpControllerNetwork{
public:
	bool gpConnect(const std::string & ip, const std::string & port);

	bool startListenerThread();
	void listenThread();	//private if thread
	
	bool sendAuthenticationRequest(std::string username, std::string key2048);
	

	
	
	
	
	// ssize_t sendTCP(mavlink_message_t & message);
	ssize_t sendRawTCP(uint8_t *&appPacket, uint16_t & pktSize);
	ssize_t sendGpMessage(GpMessage &message);
	
	
	
	
	
	
	bool _shouldSendControllerOutput = false;
	
	
	
	
	
	

private:
	int _control_fd = 0;
	struct addrinfo *_res = nullptr;
	
	uint8_t _buffer[GP_CONTROLLER_NET_MAX_BUFFER_LEN];

	
	
	// These three are part of the package directly copied from GpUavServer.cpp. Need to make universal. Going to have to do it for asset also.
	// Just a simple recv and parse. Problem action to take for each kind of message is different for controller/server/asset.
	void receiveDataAndParseMessage();
	void putHeaderInMessage(uint8_t *&buffer, long size, GpMessage & message);
	void processMessage(GpMessage & msg);
};


#endif /* defined(__UavServer__GpControllerNetwork__) */
