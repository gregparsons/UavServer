// ********************************************************************************
//
//  GpUavAsset.h
//  UavServer
//  2/25/15
//
// ********************************************************************************

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>				//INET6_ADDRSTRLEN
#include <netdb.h>
#include <arpa/inet.h>
#include <signal.h>
#include <iostream>
#include <unistd.h>					//usleep
#include <mavlink/c_library/common/mavlink.h>

#include "GpUavController.h"
#include "GpMavlink.h"
#include "GpIpAddress.h"
#include "GpNetworkTransmitter.h"

using namespace std;


void sendMavlinkMessage(mavlink_message_t & msg){
	/*
	cout << "GpUavController::sendMavlinkMessage()" << endl;
	
	GpMavlink gpMavlink;

	for(;;){
		gpMavlink.sendTestMessage();
		usleep(500);
	}
	 */
}







bool GpUavController::start(){
	
	cout << "GpUavController::start()" << endl;
	
	// 1. Start up game controller.
 
	// 2. Connect() with server
	
	
	// Loop:
	// 3. Receive events from game controller
	
	// 4. Encapsulate events in Mavlink message format
	
	// 5. Send packet over network.
	
	
	
	
	//mavlink_message_t mesg;
	//sendMavlinkMessage(mesg);
	controllerSend();
	
	
	
	return true;
}



int GpUavController::controllerSend(){
	
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
		cout << "Error: getaddrinfo" << endl;
	}
	

	// Print IP address
	char s[INET6_ADDRSTRLEN];
	inet_ntop(res->ai_family, get_in_addr((struct sockaddr *)&res->ai_addr), s, res->ai_addrlen);
	cout << "Address: " << s << endl;


	
	resSave = res;
	
	do {
		
		
		
		
		// Socket
		
		
		
		
		control_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if(control_fd < 0){
			continue;

		}
		

		
		
		// Connect
		

		
		result = connect(control_fd, res->ai_addr, res->ai_addrlen);
		if(result == 0)
			break;
		
		
		close(control_fd);
		res = res->ai_next;
	} while (res != nullptr);
	
	
	
	// Print IP
	
	
	
	inet_ntop(res->ai_family, get_in_addr((struct sockaddr *)&res->ai_addr), s, res->ai_addrlen);
	cout << "Address: " << s << endl;
	
	if(res == nullptr){
		
		cout << "Error: connect 2" << endl;
		return 2;
	}

	freeaddrinfo(resSave);
	
	/*
	 
	 
	 Send mavlink 18 bytes
	 
	 */
	
	
	
	
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
		
		/**
		 *  (socket, message, msg len, flags, dest address, dest addr len)
		 *
		 */
		numBytes = sendto(control_fd, &mavMessage, (ssize_t)(MAVLINK_MSG_ID_RC_CHANNELS_OVERRIDE_LEN), 0, res->ai_addr, res->ai_addrlen);
		
		
		if(numBytes == -1){
			cout << "Error: send()" << endl;
			exit(1);
		}
		else
			cout << numBytes << " bytes sent" << endl;
		
		numBytes = 0;
		
		
		//Do it at 100hz, 10000us = 1/100 sec
		
		usleep(10000);
		
		
		
	}
	
	close(control_fd);
	return 0;
	
	
	
}