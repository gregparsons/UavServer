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
#include "GpIpAddress.h"


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
 *  @returns bool success
 */
bool GpControllerNetwork::startListenerThread(){
	
	// Start Listener Thread
	std::cout << "[" << __func__ << "] "  <<  "connect() fail: " << strerror(errno) << std::endl;

	
	std::thread listenThread(&GpControllerNetwork::listenThread, this);
	listenThread.detach(); //??
	
	return true;
}





bool GpControllerNetwork::sendAuthenticationRequest(std::string username, std::string key2048){
	
	//encrypt something with the symmetric key I share with the server
	//server decrypts to know I'm who I am.
	
	
	GpMessage_Login loginMessage(username, key2048);
	GpMessage message(loginMessage);
	std::cout << "[" << __func__ << "] "  << "username/pwd: " << loginMessage.username() << "/" << loginMessage.key() << std::endl;
	
	// uint16_t msgSize = GP_MSG_LOGIN_LEN+GP_MSG_HEADER_LEN;
	// uint8_t msg[msgSize];
	// bzero(msg, msgSize);


	// Set GpMessage payload to serialized Login Message
	// std::vector<uint8_t> serializedLoginMsgVect;
	// loginMessage.serialize(serializedLoginMsgVect);
	// message.setPayload(serializedLoginMsgVect);
	
	
	
	std::vector<uint8_t> serializedResultVect;
	serializedResultVect.reserve(GP_MSG_LOGIN_LEN + GP_MSG_HEADER_LEN);
	message.serialize(serializedResultVect);
	
	if(sendRawTCP(serializedResultVect) > 0){
		return true;
	}
	else{
		std::cout << "[" << __func__ << "] "  << "sendRawTCP failed" << std::endl;
		;
	}
	
	

	return false;
}




ssize_t GpControllerNetwork::sendGpMessage(GpMessage &message){
	
	
	
	/*
	uint16_t msgSize = message.size(); //  message._payloadSize + GP_MSG_HEADER_LEN;

	uint8_t msg[GP_MSG_MAX_LEN];
	bzero(msg, GP_MSG_MAX_LEN);
	uint8_t *msgPtr = msg;
	
	message.serialize(msgPtr, msgSize);
	*/


	
	
	std::cout << "[" << __func__ << "] "  << "Sending msg type: " << message._message_type  << ", payload size: " << message._payloadSize << std::endl;

	
	std::vector<uint8_t> serializedMsgVect;
	serializedMsgVect.reserve(message.size());
	message.serialize(serializedMsgVect);

	return sendRawTCP(serializedMsgVect);
	
	// return sendRawTCP(msgPtr, msgSize);

	
}




long GpControllerNetwork::sendRawTCP(std::vector<uint8_t> & rawVect){
	std::cout << "[" << __func__ << "] "  << "" << std::endl;

	const void * sendBytes = rawVect.data();
	size_t sendSize = rawVect.size();
	long numBytes = sendto(_control_fd, sendBytes, sendSize, 0, _res->ai_addr, _res->ai_addrlen);
	if(numBytes == -1){
		std::cout << "[" << __func__ << "] "  << "sendto() error: " << errno << ":" << strerror(errno) <<  std::endl;
	}
	else
		std::cout << "[" << __func__ << "] "  << numBytes << " bytes sent" << std::endl;
	
	return numBytes;
	
	
	
}
/*

ssize_t GpControllerNetwork::sendRawTCP(uint8_t *&appPacket, long pktSize){
	
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
*/








void GpControllerNetwork::listenThread(){
/*
	uint8_t *buffer = _buffer;
	size_t numBytes = 0;
	for(;;){
		numBytes = recv(_control_fd, buffer, sizeof(_buffer), 0);
		if(numBytes >0)
			std::cout << "[" << __func__ << "] "  <<  "		" << numBytes << " bytes" << std::endl;
 
		parseReceivedBytes();
		
		
	}
*/
	
	std::cout << "[" << __func__ << "] "  <<  "" << strerror(errno) << std::endl;

	receiveDataAndParseMessage();
	
}




void GpControllerNetwork::receiveDataAndParseMessage()
{
	
	
	std::cout << "[" << __func__ << "] "  <<  "" << strerror(errno) << std::endl;
	
	
	// 2 BUFFERS: receive, message
	
	// RECVBUFFER
	
	size_t length =  READ_VECTOR_BYTES_MAX;  // MAVLINK_MSG_ID_RC_CHANNELS_OVERRIDE_LEN;
	uint8_t recvBuffer[READ_VECTOR_BYTES_MAX];
	bzero(recvBuffer, READ_VECTOR_BYTES_MAX);
	
	uint8_t *recvInPtr = recvBuffer;
	uint8_t *recvOutPtr = recvBuffer;
	uint8_t *recvHead = recvBuffer;
	long bytesInRecvBuffer = 0;
	
	
	
	// MESSAGEBUFFER
	
	uint8_t messageBuffer[READ_VECTOR_BYTES_MAX];
	bzero(&messageBuffer, READ_VECTOR_BYTES_MAX);
	uint8_t *msgHead = messageBuffer;	//set message max depending on size of incoming message
	long messageLenMax = 0;
	long messageLenCurrent = 0;
	bool messageStarted = false;
	long msgBytesStillNeeded = READ_VECTOR_BYTES_MAX;
	
	
	// RECEIVE LOOP
	long bytesToTransfer = 0;
	for(;;){
		
		
		// RECEIVE
		
		bytesInRecvBuffer = recv(_control_fd, recvInPtr, length, 0);		//blocks if no data, returns zero if no data and shutdown occurred
		if(bytesInRecvBuffer == -1){
			std::cout << "[" << __func__ << "] "  << "recv() error" << std::endl;
			return;
		}
		else if(bytesInRecvBuffer == 0){
			//shutdown, if zero bytes are read recv blocks, doesn't return 0 except if connection closed.
			return;
		}
		
		
		std::cout << "[" << __func__ << "] "  << "recv() read: " << bytesInRecvBuffer << " bytes" << std::endl;
		
		
		
		while(bytesInRecvBuffer > 0){
			
			GpMessage newMessage;		// dynamically allocate? or overwrite each time?
			
			if(messageStarted != true){
				
				// Start a message
				newMessage.clear();			// re-use
				
				
				if(bytesInRecvBuffer >= GP_MSG_HEADER_LEN)
					putHeaderInMessage(recvOutPtr, bytesInRecvBuffer, newMessage);		//this is the same as deserialize
				else{
					break;	// Don't have enough bytes for a message header. Nothing to do but get more.
				}
				
				
				
				
				messageLenMax = newMessage._payloadSize + GP_MSG_HEADER_LEN;
				msgBytesStillNeeded = messageLenMax;
				bytesToTransfer = std::min(bytesInRecvBuffer, msgBytesStillNeeded);		//first time, copy as many as possible from packet buffer
				
				
				
				
				memcpy(&messageBuffer, recvOutPtr, bytesToTransfer);
				
				
				
				
				
				
				
			}
			else // if(messageStarted)
			{
				
				//copy the min of available and needed from PACKET to MESSAGE buffer
				msgBytesStillNeeded = messageLenMax - messageLenCurrent;
				bytesToTransfer = std::min(bytesInRecvBuffer, msgBytesStillNeeded);
				memcpy(&messageBuffer, recvOutPtr, bytesToTransfer);
				
			}
			messageLenCurrent = bytesToTransfer;
			bytesInRecvBuffer -= bytesToTransfer;		//are there bytes leftover in recvBuffer?
			
			
			// MESSAGE COMPLETE. PROCESS.
			if(messageLenCurrent == messageLenMax)
			{
				
				//HEADER loaded above
				
				
				// PAYLOAD
				
				uint8_t *tempPayloadPtr = msgHead + GP_MSG_HEADER_LEN;
				newMessage.setPayload(tempPayloadPtr, newMessage._payloadSize);		//payload to vector
				
				// MESSAGE COMPLETE
				
				
				std::cout << "[" << __func__ << "] "  << "Received message with type: " << int(newMessage._message_type) << " and payload size: " << newMessage._payloadSize << std::endl;
				
				processMessage(newMessage);
				
				recvOutPtr+= bytesToTransfer;
				
				// Reset the message buffer.
				messageStarted = false;
				messageLenCurrent = 0;
				messageLenMax = 0;
				
				newMessage.clear();
				
			}
			
		}	// while(bytesInRecvBuffer > 0)
		recvInPtr = recvHead;			// reset both buffer pointers to head if it's empty
		recvOutPtr = recvHead;
		
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
			std::cout << "[" << __func__ << "] Client received GP_MSG_TYPE_AUTHENTICATED_BY_SERVER: setting _shouldSendControllerOutput = true" << std::endl;
			
			
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
		case GP_MSG_TYPE_NONE_ZERO:
		case GP_MSG_TYPE_COMMAND:
		case GP_MSG_TYPE_LOGIN:
		case GP_MSG_TYPE_LOGOUT:
		default:
			break;
	}
}







