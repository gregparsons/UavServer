// ********************************************************************************
//
//  GpMavlink.h
//  UavServer
//  2/25/15
//
//  gp
//    Copyright (c) 2015 swimr. All rights reserved.
//
// ********************************************************************************


#ifndef __UavServer__GpMavlink__
#define __UavServer__GpMavlink__

#include <mavlink/c_library/common/mavlink.h>
// #include "GpNetworkTransmitter.h"

class GpMavlink{
	
public:
	
	void send();
	
	static void printMavMessage(const mavlink_message_t & msg);
	static void receiveBytes(uint8_t* & bytes, int byteCount);

	
	
	void printMavChannelsOverride(const mavlink_rc_channels_override_t & ch);
	
	void sendTestMessage();
	void receiveTestMessage(mavlink_message_t & mesg);
	
//	static GpNetworkTransmitter net;
	
};


#endif /* defined(__UavServer__GpMavlink__) */
