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

#include <mavlink/c_library/common/mavlink.h>
#include <string>

class GpControllerNetwork{
public:
	bool gpConnect(const std::string & ip, const std::string & port);
	
	bool gpAuthenticateUser(std::string username, std::string key2048);
	
	
	ssize_t sendTCP(mavlink_message_t & message);
	ssize_t sendTCP(uint8_t *&appPacket, uint16_t & pktSize);
	

private:
	int _control_fd = 0;
	struct addrinfo *_res = nullptr;
};


#endif /* defined(__UavServer__GpControllerNetwork__) */
