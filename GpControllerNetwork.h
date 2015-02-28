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

class GpControllerNetwork{
public:
	static int sendTCP(mavlink_message_t & message, int size);
	

};


#endif /* defined(__UavServer__GpControllerNetwork__) */
