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
#include "GpClientNet.h"


// Static
// bool GpControllerNetwork::_shouldSendControllerOutput = false;



/**
 *  Start TCP connection from client ground controller to server.
 *
 *  @param ip address, port number
 *	@returns true if successful
 */
/*
bool GpControllerNetwork::gpConnect(const std::string & ip, const std::string & port){
	
	
	//GpClientNet net;
	//if(_net.connectToServer(GP_CONTROLLER_SERVER_IP, GP_ASSET_SERVER_PORT) == false){
	
	
	
	// CHANGE TO ASSET PORT!
	
	if(_net.connectToServer(GP_CONTROLLER_SERVER_IP, GP_CONTROLLER_SERVER_PORT) == false){
		
		std::cout << "[" << __func__ << "] "  << "No server at ip/port?" << std::endl;
		
		
		
		return false;
	}
	
	// _net.startListenerAsThread(GpControllerNetwork::_handle_messages);
	
	
	
	
	
	
	
	
	
	return true;
}
*/

/**
 *  startListenerThread
 *
 *  Listens on the client socket for messages from the server (like authentication, relay from asset, etc)
 *  @returns bool success
 */
/*
bool GpControllerNetwork::startListenerThread(){
	
	// Start Listener Thread
	std::cout << "[" << __func__ << "] "  <<  "DOES NOTHING DEPRECATED" << strerror(errno) << std::endl;

	return false;
}


*/

/*
void GpControllerNetwork::sendAuthenticationRequest(std::string username, std::string key2048){
	
	//encrypt something with the symmetric key I share with the server
	//server decrypts to know I'm who I am.
	
	
	GpMessage_Login loginMessage(username, key2048);
	GpMessage message(loginMessage);
	std::cout << "[" << __func__ << "] "  << "username/pwd: " << loginMessage.username() << "/" << loginMessage.key() << std::endl;
	
	// sendGpMessage(message);

	_net.sendMessage(message);
}
*/

/*

ssize_t GpControllerNetwork::sendGpMessage(GpMessage &message){
	

	
	std::cout << "[" << __func__ << "] "  << "DEPRECATE (wrapper for GpClientNet::sendMessage()" << int(message._message_type)  << ", payload size: " << message._payloadSize << std::endl;

	if(_net.sendMessage(message) == true)
		return 1;
	return 0;
	
}

*/
/*

long GpControllerNetwork::sendRawTCP(std::vector<uint8_t> & rawVect){
	std::cout << "[" << __func__ << "] "  << "DEPRECATED SHOULDNT BE CALLED AT ALL" << std::endl;

//	const void * sendBytes = rawVect.data();

	const uint8_t * sendBytes = &rawVect[0];		// rawVect.data();
	size_t sendSize = rawVect.size();

	
	if(sendSize == 0)
	{
		std::cout << "[" << __func__ << "] " << "0 bytes sent (vector size zero)" << std::endl;
		return 0;
		
	}
	
	
	
	
	// SEND
	

	long numBytes = send(_control_fd, sendBytes, sendSize, 0);
	if(numBytes == -1){
		std::cout << "[" << __func__ << "] "  << "sendto() error: " << errno << ":" << strerror(errno) <<  std::endl;
	}
	else
		std::cout << "[" << __func__ << "] "  << numBytes << " bytes sent" << std::endl;
	
	return numBytes;
	
	
	
}

*/


/*
void GpControllerNetwork::listenThread(){

	std::cout << "[" << __func__ << "] "  <<  "DEPRECATE YOU SHOULDN'T SEE THIS AT ALL" << std::endl;

	
	usleep(10000);
	receiveDataAndParseMessage();
	
}

*/




void GpControllerNetwork::receiveDataAndParseMessage()
{
	
	
	std::cout << "[" << __func__ << "] "  <<  "DEPRECATE YOU SHOULDN'T SEE THIS AT ALL" << std::endl;
	
	
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
				
				
				
				
				
				//_handle_messages(newMessage);

				
				
				
				
				
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




//DEPRECATED
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


/*

bool GpControllerNetwork::_handle_messages(GpMessage & msg){
	std::cout << "[" << __func__ << "] "  <<  "GpControllerNetwork called me." << std::endl;

	
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
		case GP_MSG_TYPE_COMMAND:
		{
			std::cout << "[" << __func__ << "] Client received GP_MSG_TYPE_COMMAND" << std::endl;
			
			break;
		}
		//all the rest fall through
		case GP_MSG_TYPE_NONE_ZERO:
		case GP_MSG_TYPE_LOGIN:
		case GP_MSG_TYPE_LOGOUT:
		default:
			break;
	}
	
	return true;
}







*/