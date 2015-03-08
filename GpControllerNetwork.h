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
#include <vector>

#include "GpClientNet.h"

class GpMessage;

class GpControllerNetwork{
public:
	bool gpConnect(const std::string & ip, const std::string & port);

	bool startListenerThread();
	void listenThread();	//private if thread
	
	bool sendAuthenticationRequest(std::string username, std::string key2048);
	

	
	
	
	
	ssize_t sendGpMessage(GpMessage &message);
	
	
	
	
	
	
	// static bool _shouldSendControllerOutput;
	
	
	
	
	
	

private:
	int _control_fd = 0;
	struct addrinfo *_res = nullptr;
	
	uint8_t _buffer[GP_CONTROLLER_NET_MAX_BUFFER_LEN];


	
	GpClientNet _net;

	
	
	
	
	long sendRawTCP(std::vector<uint8_t> & rawVect);

	
	
	// These three are part of the package directly copied from GpUavServer.cpp. Need to make universal. Going to have to do it for asset also.
	// Just a simple recv and parse. Problem action to take for each kind of message is different for controller/server/asset.
	void receiveDataAndParseMessage();
	void putHeaderInMessage(uint8_t *&buffer, long size, GpMessage & message);
	static bool _handle_messages(GpMessage & msg);
//	void _handle_messages(GpMessage & msg);
	
	
	
	
};


#endif /* defined(__UavServer__GpControllerNetwork__) */
