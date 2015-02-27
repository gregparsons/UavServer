// ********************************************************************************
//
//  GpMavlink.cpp
//  UavServer
//  2/25/15
//
// ********************************************************************************

#include <iostream>
#include "GpMavlink.h"
#include "GpIpAddress.h"

using namespace std;

GpNetworkTransmitter GpMavlink::net(GP_FLY_IP_ADDRESS);

void GpMavlink::send(){
	
	cout << "GpMavlink::send()" << endl;
	
}


void GpMavlink::printMavMessage(const mavlink_message_t & msg)
{
	
	/*
	 
	 typedef struct __mavlink_message {
	 uint16_t checksum; ///< sent at end of packet
	 uint8_t magic;   ///< protocol magic marker
	 uint8_t len;     ///< Length of payload
	 uint8_t seq;     ///< Sequence of packet
	 uint8_t sysid;   ///< ID of message sender system/aircraft
	 uint8_t compid;  ///< ID of the message sender component
	 uint8_t msgid;   ///< ID of message in payload
	 uint64_t payload64[(MAVLINK_MAX_PAYLOAD_LEN+MAVLINK_NUM_CHECKSUM_BYTES+7)/8];
	 }
	 
	 */
	std::cout << "Mav Message\n";
	std::cout << "magic: " << std::bitset<8>(msg.magic) << "\n";
	std::cout << "len: " << std::bitset<8>(msg.len) << "\n";
	std::cout << "seq: " << int(msg.seq) << "\n";
	std::cout << "sender sysid: " << std::bitset<8>(msg.sysid) << "\n";
	std::cout << "sender compid: " << std::bitset<8>(msg.compid) << "\n";
	std::cout << "msgid: " << std::bitset<8>(msg.msgid) << "\n";
	std::cout << "payload64[0]: " <<  std::bitset<64>(msg.payload64[0]) << "\n";
	std::cout << "payload64[1]: " <<  std::bitset<64>(msg.payload64[1]) << "\n";
	std::cout << "payload64[2]: " <<  std::bitset<64>(msg.payload64[2]) << "\n";
	std::cout << "payload64[3]: " <<  std::bitset<64>(msg.payload64[3]) << "\n";
	std::cout << std::endl;
}


void GpMavlink::printMavChannelsOverride(const mavlink_rc_channels_override_t & ch)
{
	std::cout << "Channels Override Struct\n";
	std::cout << "ch1: " << ch.chan1_raw << "\n";
	std::cout << "ch2: " << ch.chan2_raw << "\n";
	std::cout << "ch3: " << ch.chan3_raw << "\n";
	std::cout << "target component: " << ch.target_component << "\n";
	std::cout << "target system: " << ch.target_system << "\n";
	std::cout << std::endl;
}



void GpMavlink::sendTestMessage(){
	
	
	
	
	
	
	
	
	/*
	 
	 
	 #define MAVLINK_MSG_ID_RC_CHANNELS_OVERRIDE 70
	 
	 typedef struct __mavlink_rc_channels_override_t
	 {
	 uint16_t chan1_raw; ///< RC channel 1 value, in microseconds. A value of UINT16_MAX means to ignore this field.
	 uint16_t chan2_raw; ///< RC channel 2 value, in microseconds. A value of UINT16_MAX means to ignore this field.
	 uint16_t chan3_raw; ///< RC channel 3 value, in microseconds. A value of UINT16_MAX means to ignore this field.
	 uint16_t chan4_raw; ///< RC channel 4 value, in microseconds. A value of UINT16_MAX means to ignore this field.
	 uint16_t chan5_raw; ///< RC channel 5 value, in microseconds. A value of UINT16_MAX means to ignore this field.
	 uint16_t chan6_raw; ///< RC channel 6 value, in microseconds. A value of UINT16_MAX means to ignore this field.
	 uint16_t chan7_raw; ///< RC channel 7 value, in microseconds. A value of UINT16_MAX means to ignore this field.
	 uint16_t chan8_raw; ///< RC channel 8 value, in microseconds. A value of UINT16_MAX means to ignore this field.
	 uint8_t target_system; ///< System ID
	 uint8_t target_component; ///< Component ID
	 } mavlink_rc_channels_override_t;
	 
	 
	 */
	
	
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
	
	cout << "Sending: " << endl;
	printMavMessage(mavMessage);
	printMavChannelsOverride(channels);
	
	net.transmitEvent(mavMessage);
	
}


void GpMavlink::receiveTestMessage(mavlink_message_t & mesg){
	
	
	// Decode
	// mavlink_msg_rc_channels_override_decode(<#const mavlink_message_t *msg#>, <#mavlink_rc_channels_override_t *rc_channels_override#>)

	mavlink_rc_channels_override_t incomingObject;
	mavlink_msg_rc_channels_override_decode(&mesg, &incomingObject);

	cout << "Received:" << endl;
	printMavMessage(mesg);
	printMavChannelsOverride(incomingObject);
	

	
	//mavlink_parse_char(uint8_t chan, uint8_t c, mavlink_message_t *r_message, mavlink_status_t *r_mavlink_status)
	
}

