// ********************************************************************************
//
//  GpControllerNetwork.cpp
//  UavServer
//  2/27/15
//
// ********************************************************************************

#include <mavlink/c_library/common/mavlink.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>				//INET6_ADDRSTRLEN
#include <netdb.h>
#include <arpa/inet.h>
#include <signal.h>
#include <iostream>
#include <unistd.h>					//usleep

#include "GpControllerNetwork.h"
#include "GpMavlink.h"



bool GpControllerNetwork::gpConnect(const std::string & ip, const std::string & port){
	
	//int control_fd = 0;
	struct addrinfo hints,  *resSave = nullptr; /* *res = nullptr */
	int result = 0;

	
	// memset(&hints, 0, sizeof(hints));
	bzero(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;		//AF_INET = tcp, udp
	hints.ai_socktype = SOCK_STREAM;
	
	result = getaddrinfo(ip.c_str(), port.c_str(), &hints, &_res);
	if(result == -1){
		std::cout << "Error: getaddrinfo" << std::endl;
	}
	
	
	
	
	resSave = _res;
	
	do {
		
		
		
		
		// Socket
		
		
		
		
		_control_fd = socket(_res->ai_family, _res->ai_socktype, _res->ai_protocol);
		if(_control_fd < 0){
			continue;
			
		}
		
		
		
		
		// Connect
		
		
		
		result = connect(_control_fd, _res->ai_addr, _res->ai_addrlen);
		if(result == 0){
			
			break;
			
		}
		
		
		close(_control_fd);
		_res = _res->ai_next;
	} while (_res != nullptr);
	
	
	
	
	if(_res == nullptr){
		
		std::cout << "Error: connect 2" << std::endl;
		return false;
	}
	
	freeaddrinfo(resSave);
	
	
	return true;
}

int GpControllerNetwork::sendTCP(mavlink_message_t & message, int size)
{
	
	
	
	ssize_t numBytes = 0;
	

	
	
	
	
	
//	for(int i=0; i< 999999 ; i++)
//	{
		
		// Mavlink
		
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
		mavlink_message_t mavMessage;
		mavMessage.sysid = 5;
		mavMessage.compid = 7;
		mavlink_msg_rc_channels_override_encode(5, 7, &mavMessage, &channels);
		
		GpMavlink::printMavMessage(mavMessage);

		
		// numBytes = send(_control_fd, &mavMessage, (ssize_t)(MAVLINK_MSG_ID_RC_CHANNELS_OVERRIDE_LEN),0);
		

		numBytes = sendto(_control_fd, &mavMessage, (ssize_t)(MAVLINK_MSG_ID_RC_CHANNELS_OVERRIDE_LEN), 0, _res->ai_addr, _res->ai_addrlen);
		
		
		if(numBytes == -1){
			std::cout << "Error: send()" << std::endl;
			exit(1);
		}
		else
			std::cout << numBytes << " bytes sent" << std::endl;
		
		
		//Do it at 100hz, 10000us = 1/100 sec
		
		numBytes = 0;
//		usleep(10000);
		
		
		
//	}
	
//	close(_control_fd);
	return 0;
	
	
	
}

