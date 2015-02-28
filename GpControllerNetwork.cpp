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

// #include "GpMavlink.h"
#include "GpIpAddress.h"
// #include "GpControllerNetwork.h"


#include "GpControllerNetwork.h"

int GpControllerNetwork::sendTCP(mavlink_message_t & message, int size)
{
	
	
	
	int control_fd = 0;
	struct addrinfo hints, *res = nullptr, *resSave = nullptr;
	int result = 0;
	ssize_t numBytes = 0;
	
	// memset(&hints, 0, sizeof(hints));
	bzero(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;		//AF_INET = tcp, udp
	hints.ai_socktype = SOCK_STREAM;
	
	result = getaddrinfo(GP_FLY_IP_ADDRESS, GP_CONTROL_PORT, &hints, &res);
	if(result == -1){
		std::cout << "Error: getaddrinfo" << std::endl;
	}
	

	
	
	resSave = res;
	
	do {
		
		
		
		
		// Socket
		
		
		
		
		control_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if(control_fd < 0){
			continue;
			
		}
		
		
		
		
		// Connect
		
		
		
		result = connect(control_fd, res->ai_addr, res->ai_addrlen);
		if(result == 0){

			break;

		}
		
		
		close(control_fd);
		res = res->ai_next;
	} while (res != nullptr);
	
	
	
	
	if(res == nullptr){
		
		std::cout << "Error: connect 2" << std::endl;
		return 2;
	}
	
	freeaddrinfo(resSave);
	

	
	
	
	
	
	for(int i=0; i< 999999 ; i++)
	{
		
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
		
		
		
		// numBytes = send(control_fd, &mavMessage, (ssize_t)(MAVLINK_MSG_ID_RC_CHANNELS_OVERRIDE_LEN),0);
		

		numBytes = sendto(control_fd, &mavMessage, (ssize_t)(MAVLINK_MSG_ID_RC_CHANNELS_OVERRIDE_LEN), 0, res->ai_addr, res->ai_addrlen);
		
		
		if(numBytes == -1){
			std::cout << "Error: send()" << std::endl;
			exit(1);
		}
		else
			std::cout << numBytes << " bytes sent" << std::endl;
		
		
		//Do it at 100hz, 10000us = 1/100 sec
		
		numBytes = 0;
		usleep(10000);
		
		
		
	}
	
	close(control_fd);
	return 0;
	
	
	
}

