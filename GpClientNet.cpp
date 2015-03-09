// ********************************************************************************
//
//  GpClientNet.cpp
//  UavServer
//  3/7/15
//
// ********************************************************************************
#include <sys/types.h>
#include <netdb.h>
#include <iostream>
#include <boost/format.hpp>
#include <thread>
#include <arpa/inet.h> //only for inet_ntop()
#include <strings.h>	//bzero
#include <string.h>		//memcpy, memset


#include "GpClientNet.h"
#include "GpMessage.h"
#include "GpMessage_Login.h"
#include "GpIpAddress.h"


//TEMP FORWARD DECLARE!!!!!!!!!!!!!!!! DELETE
void receiveDataAndParseMessage(bool message_handler(GpMessage & mesg), int _fd);




GpClientNet::GpClientNet(){

}

GpClientNet::GpClientNet(gp_message_handler message_handler){
	
	_message_handler = message_handler;
}


bool
GpClientNet::connectToServer(std::string ip, std::string port){
	

	struct addrinfo hints, *server_info, *server_info_backup;
	memset(&hints, 0, sizeof(addrinfo));
	hints.ai_family = AF_UNSPEC;		//ipv4 or ipv6
	hints.ai_socktype = SOCK_STREAM;	//defaults to udp otherwise?

	if(::getaddrinfo(ip.c_str(), port.c_str(), &hints, &server_info) == -1){
		perror((boost::format("%s") % __func__ ).str().c_str() );
		return false;
	}
	
	

	
	//ref: Stevens/Fenner/Rudoff
	server_info_backup = server_info;
	do {
		
		// Socket
		
		_fd = ::socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);
		if(_fd < 0){
			continue;
		}
		

		
		
		// Connect
		
		if(0 == ::connect(_fd, server_info->ai_addr, server_info->ai_addrlen)){
			

			// Print IP address connected to
			
			if(server_info->ai_family == AF_INET){
				char str[INET_ADDRSTRLEN];
				inet_ntop(AF_INET, &(((struct sockaddr_in *)server_info->ai_addr)->sin_addr), str, INET_ADDRSTRLEN);
			
				std::cout << "[" << __func__ << "] " << "Connected to " << str << std::endl;
			}
			


			
			break;			// break if good connect
		}

		
		
		
		close(_fd);			// otherwise close, keep trying
		server_info = server_info->ai_next;
	} while (server_info != nullptr);
	
	if(server_info == nullptr){
		std::cout << "[" << __func__ << "] "  << "Error: connect 2" << strerror(errno) << std::endl;
		return false;
	}
	::freeaddrinfo(server_info_backup);
	
	return true;
}


/**
 *  sendMessage
 *
 *  Thread safe call to _sendMessage
 *
 *  @param GpMessage & message
 *  @returns bool result
 */
bool
GpClientNet::sendMessage(GpMessage &message){
	
	std::lock_guard<std::mutex> sendGuard(_send_mutex);		// when guard class is destroyed at end of function the lock is released
	return _sendMessage(message);
	
}

bool
GpClientNet::_sendMessage(GpMessage &message){
	
	if(_fd == 0){
		std::cout << "[" << __func__ << "] "  << "Can't send. _fd closed." << std::endl;
		return false;
	}
	
	
	
	byte_vector bytes;
	bytes.reserve(message.size());
	message.serialize(bytes);
	
	ssize_t result = send(_fd, bytes.data(), bytes.size(), MSG_DONTWAIT);	//MSG_DONTWAIT -- If set send returns EAGAIN if outbond traffic is clogged. A one-time "skip it."

	if(result == -1){
		if(EAGAIN == errno){

			;
			// message was skipped. Try again? Or disgregard? Might depend on message type.

		}
		perror(__func__);		//prints function name and error
		return false;
	}
	else if (result == 0){
		perror((boost::format("%s: connection closed") % __func__ ).str().c_str() );
		return false;
	}
	return true;
}







void GpClientNet::startListenerAsThread(GpClientNet::gp_message_handler message_handler){

	if(message_handler != nullptr){
	
		_message_handler = message_handler;

		
		//new detached thread
		//_listen_for_TCP_messages();

		
		
		std::thread listenThread(&GpClientNet::_listen_for_TCP_messages, this);
		// listenThread.join();
		listenThread.detach();
	}
}








/**
 *  Listen for GpMessages from server
 *
 */
bool
GpClientNet::_listen_for_TCP_messages(){
	std::cout << "[" << __func__ << "] "  <<  "" << std::endl;


	if(_message_handler != nullptr && _fd > 0){

		_receiveDataAndParseMessage();		//blocks in recv(). When a message is complete message_handler() is called on the GpMessage.

	}
	else{
	
		std::cout << "[" << __func__ << "] "  <<  "_message_handler or _fd weren't set" << std::endl;
		return false;
		
	}

	
	
	return true; //started okay
}
























// JUNK FROM OLD CONTROLLER CLIENT, CLEAN UP AND PUT IN _list_for_TCP_messages()



// Only required by receiveDataAndParseMessage (DELETE EVENTUALLY)
void putHeaderInMessage(uint8_t *&buffer, long size, GpMessage & message){
	
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



void GpClientNet::_receiveDataAndParseMessage()
{
	
	
	std::cout << "[" << __func__ << "] "  <<  "" << std::endl;
	
	
	// 2 BUFFERS: receive, message
	
	// RECVBUFFER ----------------------- CHANGE TO VECTOR ----------------------------------------
	
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
		
		bytesInRecvBuffer = recv(_fd, recvInPtr, length, 0);		//blocks if no data, returns zero if no data and shutdown occurred
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
				
				std::cout << "[" << __func__ << "] "  << "Received message with type: " << int(newMessage._message_type) << " and payload size: " << newMessage._payloadSize << std::endl;

				
				// Clean up (before getting guard on message_handler)
				recvOutPtr+= bytesToTransfer;
				// Reset the message buffer.
				messageStarted = false;
				messageLenCurrent = 0;
				messageLenMax = 0;
				
				

				
				// HANDLE_MESSAGE
				
				// Get a lock in case message_handler isn't thread safe.
				std::lock_guard<std::mutex> sendGuard(_message_handler_mutex);		// when guard class is destroyed at end of scope the lock is released
				_message_handler(newMessage, *this);		// Do the code given by the calling entity.

				
				
				newMessage.clear();
			}
			
		}	// while(bytesInRecvBuffer > 0)
		recvInPtr = recvHead;			// reset both buffer pointers to head if it's empty
		recvOutPtr = recvHead;
	}
}





void GpClientNet::sendAuthenticationRequest(std::string username, std::string key){
	
	//encrypt something with the symmetric key I share with the server
	//server decrypts to know I'm who I am.
	
	
	GpMessage_Login loginMessage(username, key);
	GpMessage message(loginMessage);
	std::cout << "[" << __func__ << "] "  << "username/pwd: " << loginMessage.username() << "/" << loginMessage.key() << std::endl;
	
	
	sendMessage(message);
	
	
}

