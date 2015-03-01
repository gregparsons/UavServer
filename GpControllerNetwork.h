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
	
	
	int sendTCP(mavlink_message_t & message, int size);
	

private:
	int _control_fd = 0;
	struct addrinfo *_res = nullptr;
};


#endif /* defined(__UavServer__GpControllerNetwork__) */
