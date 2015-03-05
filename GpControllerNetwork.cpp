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
#include <thread>
#include <string.h>					//bzero(), could just use memcpy

#include "GpControllerNetwork.h"
#include "GpMavlink.h"
#include "GpMessage.h"
#include "GpMessage_Login.h"


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
			break;			// break if good connect
		}
		close(_control_fd);
		_res = _res->ai_next;
	} while (_res != nullptr);
	
	if(_res == nullptr){
		std::cout << "Error: connect 2" << std::endl;
		return false;
	}
	freeaddrinfo(resSave);
	

	// Still going? Got a good connection.
	
	
	
	// Start Listener
	
	std::cout << "[GpControllerNetwork::gpConnect] Starting listen thread" << std::endl;
	std::thread listenThread(&GpControllerNetwork::listen, this);
	listenThread.detach(); //??
	std::cout << "[GpControllerNetwork::gpConnect] listen thread started" << std::endl;
	
	
	
	
	
	return true;
}







bool GpControllerNetwork::gpAuthenticateUser(std::string username, std::string key2048){
	
	//encrypt something with the symmetric key I share with the server
	//server decrypts to know I'm who I am.

	std::cout << "GpControllerNetwork::gpAuthenticateUser NOT IMPLEMENTED YET!" << std::endl;

	GpMessage_Login loginMessage(username, key2048);
	GpMessage message(loginMessage);
	
	uint16_t msgSize = GP_MSG_LOGIN_LEN+GP_MSG_HEADER_LEN;
	uint8_t msg[msgSize];
	bzero(msg, msgSize);
	uint8_t *msgPtr = msg;
	
	message.serialize(msgPtr, msgSize);
	
	sendTCP(msgPtr, msgSize);

	
	
	
	
	
	
	return true;
}








/**
 *  Send a mavlink-formatted message over TCP to current socket.
 *
 */
ssize_t GpControllerNetwork::sendTCP(mavlink_message_t & message)
{
	// sendto
	
	ssize_t numBytes = 0;
	numBytes = sendto(_control_fd, &message, message.len, 0, _res->ai_addr, _res->ai_addrlen);
	if(numBytes == -1){
		std::cout << "Error: send()" << std::endl;
	}
	else
		std::cout << numBytes << " bytes sent" << std::endl;
	

	return numBytes;
	
	
	
}

ssize_t GpControllerNetwork::sendTCP(uint8_t *&appPacket, uint16_t &pktSize){

	int numBytes = 0;
	size_t size = (size_t)pktSize;

	
	
	// Message is going out okay. Why is it garbled on the other end?
	// GpMessage test_message;
	// test_message.deserialize(appPacket, pktSize);
	
	
	
	
	
	
	
	numBytes = sendto(_control_fd, appPacket, size, 0, _res->ai_addr, _res->ai_addrlen);
	if(numBytes == -1){
		std::cout << "[GpControllerNetwork::sendTCP] sendto() error: " << strerror(errno) <<  std::endl;
	}
	else
		std::cout << numBytes << " bytes sent" << std::endl;
	
	return numBytes;
	
	
	
}





void GpControllerNetwork::listen(){

	uint8_t *buffer = _buffer;
	size_t numBytes = 0;
	for(;;){
		numBytes = recv(_control_fd, buffer, sizeof(_buffer), 0);
		if(numBytes >0)
			std::cout << "[GpControllerNetwork::listen] Received " << numBytes << " bytes" << std::endl;
	}
	
}