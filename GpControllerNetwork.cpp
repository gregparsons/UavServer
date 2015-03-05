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

#define GP_READ_VECTOR_BYTES_MAX 300


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
		std::cout << "[" << __func__ << "] " << "Error: getaddrinfo" << std::endl;
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
		std::cout << "[" << __func__ << "] "  << "Error: connect 2" << strerror(errno) << std::endl;
		return false;
	}
	freeaddrinfo(resSave);
	

	// Still going? Got a good connection.
	
	
	
	
	
	
	
	
	return true;
}


/**
 *  startListenerThread
 *
 *  Listens on the client socket for messages from the server (like authentication, relay from asset, etc)
 *
 *  @param <#parameter#>
 *  @returns bool success
 */
bool GpControllerNetwork::startListenerThread(){
	
	// Start Listener Thread
	
	std::cout << "[" << __func__ << "] "  << "Starting listen thread" << std::endl;
	std::thread listenThread(&GpControllerNetwork::listenThread, this);
	listenThread.detach(); //??
	std::cout << "[" << __func__ << "] "  << "Listen thread started" << std::endl;
	
	return true;
}





bool GpControllerNetwork::sendAuthenticationRequest(std::string username, std::string key2048){
	
	//encrypt something with the symmetric key I share with the server
	//server decrypts to know I'm who I am.
	
	std::cout << "[" << __func__ << "] "  << "NOT IMPLEMENTED YET!" << std::endl;
	
	GpMessage_Login loginMessage(username, key2048);
	GpMessage message(loginMessage);
	
	uint16_t msgSize = GP_MSG_LOGIN_LEN+GP_MSG_HEADER_LEN;
	uint8_t msg[msgSize];
	bzero(msg, msgSize);
	uint8_t *msgPtr = msg;
	
	message.serialize(msgPtr, msgSize);
	
	sendRawTCP(msgPtr, msgSize);
	
	return true;
}




/**
 *  Send a mavlink-formatted message over TCP to current socket.
 *
 */
/*
 ssize_t GpControllerNetwork::sendRawTCP(mavlink_message_t & message)
 {
	// sendto
	
	ssize_t numBytes = 0;
	numBytes = sendto(_control_fd, &message, message.len, 0, _res->ai_addr, _res->ai_addrlen);
	if(numBytes == -1){
 std::cout << "[" << __func__ << "] "  << "Error: send()" << std::endl;
	}
	else
 std::cout << "[" << __func__ << "] "  << numBytes << " bytes sent" << std::endl;
	
 
	return numBytes;
	
	
	
 }
 */

ssize_t GpControllerNetwork::sendGpMessage(GpMessage &message){
	
	
	
	
	uint16_t msgSize = message.size(); //  message._payloadSize + GP_MSG_HEADER_LEN;

	uint8_t msg[GP_MSG_MAX_LEN];
	bzero(msg, GP_MSG_MAX_LEN);
	uint8_t *msgPtr = msg;
	
	message.serialize(msgPtr, msgSize);
	
	return sendRawTCP(msgPtr, msgSize);

	
}




ssize_t GpControllerNetwork::sendRawTCP(uint8_t *&appPacket, uint16_t &pktSize){
	
	size_t numBytes = 0;
	size_t size = (size_t)pktSize;
	
	numBytes = sendto(_control_fd, appPacket, size, 0, _res->ai_addr, _res->ai_addrlen);
	if(numBytes == -1){
		std::cout << "[" << __func__ << "] "  << "sendto() error: " << strerror(errno) <<  std::endl;
	}
	else
		std::cout << "[" << __func__ << "] "  << numBytes << " bytes sent" << std::endl;
	
	return numBytes;
	
	
	
}









void GpControllerNetwork::listenThread(){
/*
	uint8_t *buffer = _buffer;
	size_t numBytes = 0;
	for(;;){
		numBytes = recv(_control_fd, buffer, sizeof(_buffer), 0);
		if(numBytes >0)
			std::cout << "[" << __func__ << "] "  <<  "Received " << numBytes << " bytes" << std::endl;
		
		parseReceivedBytes();
		
		
	}
*/
	receiveDataAndParseMessage();
	
}




void GpControllerNetwork::receiveDataAndParseMessage()
{
	
	
	std::cout << "[" << __func__ << "] "  << "starting thread: " << std::this_thread::get_id() << std::endl;
	
	
	// 2 BUFFERS: receive, message
	
	// RECVBUFFER
	
	size_t length =  GP_READ_VECTOR_BYTES_MAX;  // MAVLINK_MSG_ID_RC_CHANNELS_OVERRIDE_LEN;
	uint8_t recvBuffer[GP_READ_VECTOR_BYTES_MAX];
	bzero(recvBuffer, GP_READ_VECTOR_BYTES_MAX);
	
	uint8_t *recvInPtr = recvBuffer;
	uint8_t *recvOutPtr = recvBuffer;
	uint8_t *recvHead = recvBuffer;
	long bytesInRecvBuffer = 0;
	
	
	
	// MESSAGEBUFFER
	
	uint8_t messageBuffer[GP_READ_VECTOR_BYTES_MAX];
	bzero(&messageBuffer, GP_READ_VECTOR_BYTES_MAX);
	uint8_t *msgHead = messageBuffer;	//set message max depending on size of incoming message
	long messageLenMax = 0;
	long messageLenCurrent = 0;
	bool messageStarted = false;
	long msgBytesStillNeeded = GP_READ_VECTOR_BYTES_MAX;
	
	
	
	// RECEIVE LOOP
	
	for(;;){
		
		
		// RECEIVE
		
	GP_RECEIVE:
		
		bytesInRecvBuffer = recv(_control_fd, recvInPtr, length, 0);		//blocks if no data, returns zero if no data and shutdown occurred
		recvInPtr += bytesInRecvBuffer;
		if(bytesInRecvBuffer == -1){
			std::cout << "[" << __func__ << "] "  << "Error: recv()" << std::endl;
			exit(1);
			//continue;
		}
		else if(bytesInRecvBuffer == 0){
			//shutdown, if zero bytes are read recv blocks, doesn't return 0 except if connection closed.
			exit(0);
			//continue;
		}
		
		
		std::cout << "[" << __func__ << "] "  << "Read: " << bytesInRecvBuffer << " bytes" << std::endl;
		
		
		
		GpMessage newMessage;
		while(bytesInRecvBuffer > 0){
			
			if(messageStarted != true){
				if(bytesInRecvBuffer >= GP_MSG_HEADER_LEN)
					putHeaderInMessage(recvOutPtr, bytesInRecvBuffer, newMessage);		//this is the same as deserialize
				else{
					goto GP_RECEIVE;	// Don't have enough bytes for a message header. Nothing to do but get more.
				}
				
				// Start a message
				messageLenMax = newMessage._payloadSize + GP_MSG_HEADER_LEN;
				msgBytesStillNeeded = messageLenMax;
				long bytesToTransfer = std::min(bytesInRecvBuffer, msgBytesStillNeeded);		//first time, copy as many as possible from packet buffer
				memcpy(&messageBuffer, recvOutPtr, bytesToTransfer);
				messageLenCurrent = bytesToTransfer;
				recvOutPtr += bytesToTransfer;					//increment fill line of message buffer
				bytesInRecvBuffer -= bytesToTransfer;		//are there bytes leftover in recvBuffer?
				messageStarted = true;
				
			}
			else // if(messageStarted)
			{
				
				//copy the min of available and needed from PACKET to MESSAGE buffer
				long bytesStillNeeded = messageLenMax - messageLenCurrent;
				long bytesToTransfer = std::min(bytesInRecvBuffer, bytesStillNeeded);
				
				memcpy(&messageBuffer, recvOutPtr, bytesToTransfer);
				messageLenCurrent += bytesToTransfer;
				recvOutPtr += bytesToTransfer;
				bytesInRecvBuffer -= bytesToTransfer;
				
			}
			
			
			// MESSAGE COMPLETE. PROCESS.
			if(messageLenCurrent == messageLenMax)
			{
				newMessage._payload = msgHead+3;	//okay, but this is about to get cleared with arrival of next packet, message here is the entire message, not the payload
				std::cout << "[" << __func__ << "] "  << "Received message with type: " << newMessage._message_type << " and payload size: " << newMessage._payloadSize << std::endl;
				
				
				// Parse and process the message
				uint16_t messageSize = newMessage._payloadSize + GP_MSG_HEADER_LEN;
				newMessage.deserialize(msgHead, messageSize);
				processMessage(newMessage);
				
				
				
				// Reset the message buffer.
				messageStarted = false;
				messageLenCurrent = 0;
				messageLenMax = 0;
			}
			
		}	// while(bytesInRecvBuffer > 0)
		if(bytesInRecvBuffer == 0){
			recvInPtr = recvHead;
			recvOutPtr = recvHead;
		}
		
		
	}
}





void GpControllerNetwork::putHeaderInMessage(uint8_t *&buffer, long size, GpMessage & message){
	
	//uint8_t *ptr = buffer;
	
	// Message_Type
	message._message_type = *buffer; //GP_MSG_TYPE_LOGIN;
	
	
	
	// Payload Size
	uint8_t *sizePtr = buffer + 1;
	uint16_t pSize = 0;
	GpMessage::bitUnstuff16(sizePtr, pSize);
	message._payloadSize = pSize;			//GP_MSG_LOGIN_LEN;
	
	
	return;
}




void GpControllerNetwork::processMessage(GpMessage & msg){
	
	
	switch (msg._message_type) {
		case GP_MSG_TYPE_AUTHENTICATED_BY_SERVER:
		{
			// START SENDING MESSAGES
			std::cout << "[" << __func__ << "] Client received GP_MSG_TYPE_AUTHENTICATED_BY_SERVER: starting asset commands from game controller" << std::endl;
			
			
			// start sending asset commands
			
			
			_shouldSendControllerOutput = true;
			
			
			
			
			break;
		}
		case GP_MSG_TYPE_HEARTBEAT:
		{
			std::cout << "[" << __func__ << "] Client received GP_MSG_TYPE_HEARTBEAT" << std::endl;
			
			break;
		}
			
		//all the rest fall through
		case GP_MSG_TYPE_COMMAND:
		case GP_MSG_TYPE_GENERIC:
		case GP_MSG_TYPE_LOGIN:
		case GP_MSG_TYPE_LOGOUT:
		default:
			break;
	}
}







