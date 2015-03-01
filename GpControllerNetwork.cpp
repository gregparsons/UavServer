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


/**
 *  Start TCP connection from client ground controller to server.
 *
 *  @param ip address, port number
 *	@returns true if successful
 */
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
	
	//ref: Stevens/Fenner/Rudoff
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









/**
 *  Send a mavlink-formatted message over TCP to current socket.
 *
 */
int GpControllerNetwork::sendTCP(mavlink_message_t & message)
{
	// sendto
	
	ssize_t numBytes = 0;
	numBytes = sendto(_control_fd, &message, message.len, 0, _res->ai_addr, _res->ai_addrlen);
	if(numBytes == -1){
		std::cout << "Error: send()" << std::endl;
		exit(1);
	}
	else
		std::cout << numBytes << " bytes sent" << std::endl;
	

	return 0;
	
	
	
}

