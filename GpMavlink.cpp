// ********************************************************************************
//
//  GpMavlink.cpp
//  UavServer
//  2/25/15
//
// ********************************************************************************

#include <iostream>
#include <bitset>
#include <string.h>			//memset, bzero
#include "GpMavlink.h"
#include "GpIpAddress.h"
#include "GpControllerEvent.h"
#include "GpMessage.h"




void GpMavlink::printMavFromGpMessage(GpMessage & msg){
	
	
	if(msg._message_type != GP_MSG_TYPE_COMMAND){
		std::cout << "[" << __func__ << "] "  << "Can't decode. Not a Mavlink message." << std::endl;
		return;
	}
	
	

	
	// GpMessage Payload to Mavlink
	mavlink_message_t mav;
	deserializeGpMessagePayloadToMavlink(msg, mav);
	
	// Print Mavlink Message
	printMavlinkMessage(mav);
	
	
}




void GpMavlink::deserializeGpMessagePayloadToMavlink(GpMessage & message, mavlink_message_t & mavlink){
	memcpy(&mavlink, message._payLd_vec.data(), message._payLd_vec.size());

}


void GpMavlink::deserializePayloadToChannelOverride(mavlink_message_t & mavlink, mavlink_rc_channels_override_t & channels){
	mavlink_msg_rc_channels_override_decode(&mavlink, &channels);
	
}





void GpMavlink::printMavlinkMessage(mavlink_message_t & mav)
{
	
	switch (mav.msgid) {
		case MAVLINK_MSG_ID_RC_CHANNELS_OVERRIDE:
		{
			std::cout << "[" << __func__ << "] "  << "MAVLINK_MSG_ID_RC_CHANNELS_OVERRIDE" << std::endl;
			
			// Print
			
			mavlink_rc_channels_override_t channels;
			deserializePayloadToChannelOverride(mav, channels);
			printMavChannelsOverride(channels);
			
			break;
		}
		default:
			break;
	}

}


void GpMavlink::printMavChannelsOverride(mavlink_rc_channels_override_t & ch)
{
	
	///< RC channel 1 value, in microseconds. A value of UINT16_MAX means to ignore this field.
	std::cout << "Channels Override:\n";
	std::cout << "ch1: " << (int)ch.chan1_raw << "\n";
	std::cout << "ch2: " << (int)ch.chan2_raw << "\n";
	std::cout << "ch3: " << (int)ch.chan3_raw << "\n";
	std::cout << "ch4: " << (int)ch.chan4_raw << "\n";
	std::cout << "target component: " << ch.target_component << "\n";
	std::cout << "target system: " << ch.target_system << "\n";
	std::cout << std::endl;
}





void GpMavlink::sendTestMessage(){
	///< RC channel 1 value scaled, (-100%) -10000, (0%) 0, (100%) 10000, (invalid) INT16_MAX.
	mavlink_rc_channels_override_t channels;
	channels.chan1_raw = 5;
	channels.chan2_raw = 64000;
	channels.chan3_raw = UINT16_MAX;
	channels.chan4_raw = UINT16_MAX;
	channels.chan5_raw = UINT16_MAX;
	channels.chan6_raw = UINT16_MAX;
	channels.chan7_raw = UINT16_MAX;
	channels.chan8_raw = UINT16_MAX;
	channels.target_component = 11;
	channels.target_system = 9;

	// Encode
	
	mavlink_message_t mavMessage;
	mavMessage.sysid = 5;
	mavMessage.compid = 7;
	// mavlink_msg_rc_channels_override_encode(<#uint8_t system_id#>, <#uint8_t component_id#>, <#mavlink_message_t *msg#>, <#const mavlink_rc_channels_override_t *rc_channels_override#>)
	mavlink_msg_rc_channels_override_encode(5, 7, &mavMessage, &channels);
	
	
	
}




void GpMavlink::receiveTestMessage(mavlink_message_t & mesg){
	
	
	// Decode
	// mavlink_msg_rc_channels_override_decode(const mavlink_message_t *msg, <#mavlink_rc_channels_override_t *rc_channels_override#>)

	mavlink_rc_channels_override_t incomingObject;
	mavlink_msg_rc_channels_override_decode(&mesg, &incomingObject);

	std::cout << "Received:" << std::endl;
}








/**
 *  [Static] Convert controller event data to mavlink message of type MAVLINK_MSG_ID_RC_CHANNELS_OVERRIDE
 *
 */
void GpMavlink::encodeControllerEventAsMavlink(int left_x, int left_y, int right_x, int right_y, int timestamp, mavlink_message_t & mavlinkMessage, uint16_t & msgLen) {

	mavlink_rc_channels_override_t rcChannelOverrideMessage;
	memset(&rcChannelOverrideMessage, 0, MAVLINK_MSG_ID_RC_CHANNELS_OVERRIDE_LEN);
	
	///< RC channel 1 value, in microseconds. A value of UINT16_MAX means to ignore this field.	rcChannelOverrideMessage.chan1_raw = left_x;
	rcChannelOverrideMessage.chan1_raw = left_x;
	rcChannelOverrideMessage.chan2_raw = left_y;
	rcChannelOverrideMessage.chan3_raw = right_x;
	rcChannelOverrideMessage.chan4_raw = right_y;
	rcChannelOverrideMessage.chan5_raw = UINT16_MAX;
	rcChannelOverrideMessage.chan6_raw = UINT16_MAX;
	rcChannelOverrideMessage.chan7_raw = UINT16_MAX;
	rcChannelOverrideMessage.chan8_raw = UINT16_MAX;
	rcChannelOverrideMessage.target_component = 1;
	rcChannelOverrideMessage.target_system = 1;
	
	/*
	mavlinkMessage.sysid = 5;
	mavlinkMessage.compid = 7;
	*/
	
	uint8_t systemId = 1;				// HARD CODED SYSTEM ID!!!!
	uint8_t compId = 1;
	
	
	// mavlink_msg_rc_channels_override_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t *msg, const mavlink_rc_channels_override_t *rc_channels_override)

	msgLen = mavlink_msg_rc_channels_override_encode(systemId, compId, &mavlinkMessage, &rcChannelOverrideMessage);
	
	// Now mavlinkMessage has the ready-to-send mavlink message. This function returns with it filled.
	
	printMavChannelsOverride(rcChannelOverrideMessage);
	
	
}





