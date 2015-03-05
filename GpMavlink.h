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

class GpControllerEvent;

class GpMavlink{
	
public:
	static void encodeControllerEventAsMavlink(int left_x, int left_y, int right_x, int right_y, int timestamp, mavlink_message_t & mavlinkMessage, uint16_t & msgLen);
	static void decodeMavlinkBytesToControlEvent(uint8_t* & bytes, size_t byteCount);
	static void printMavChannelsOverride(const mavlink_rc_channels_override_t & ch);
	

	
	//junk
	void send();
	static void printMavMessage(const mavlink_message_t & msg);
	void sendTestMessage();
	void receiveTestMessage(mavlink_message_t & mesg);
	
//	static GpNetworkTransmitter net;
	
};


#endif /* defined(__UavServer__GpMavlink__) */
