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

class GpControllerEvent;
class GpMessage;

class GpMavlink{
	
public:
	
	
	static void encodeControllerEventAsMavlink(int left_x, int left_y, int right_x, int right_y, int timestamp, mavlink_message_t & mavlinkMessage, uint16_t & msgLen);
	
	//void sendTestMessage();
	//void receiveTestMessage(mavlink_message_t & mesg);
	
	static void printMavFromGpMessage(GpMessage & msg);
	static void printMavlinkMessage(mavlink_message_t & msg);

	static void deserializeGpMessagePayloadToMavlink(GpMessage & message, mavlink_message_t & mavlink);
	static void deserializeBytesToMavlink(uint8_t* & bytes, size_t byteCount, mavlink_message_t & mavlinkMsg);
	static void deserializePayloadToChannelOverride(mavlink_message_t & sourceMavlink, mavlink_rc_channels_override_t & chOverride);

	
	
private:
	
	static void printMavChannelsOverride(mavlink_rc_channels_override_t & ch);
	
	
};


#endif /* defined(__UavServer__GpMavlink__) */
