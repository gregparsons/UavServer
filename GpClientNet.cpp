// ********************************************************************************
//
//  GpClientNet.cpp
//  UavServer
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
#include <chrono>
#include <bitset>

#include <sys/stat.h>	// mkdir
#include <sstream>
#include <fstream>
#include <iomanip>
#include <cstdlib>
#include <signal.h>


#include "GpClientNet.h"
#include "GpMessage.h"
#include "GpMessage_Login.h"
#include "GpIpAddress.h"


//Statics
std::ofstream GpClientNet::_metricsFile;

void GpClientNet::_interruptHandler(int s){
	std::cout << "Caught signal: " << s << std::endl;

	_closeLogging();
	exit(1);
	
}

void GpClientNet::_catchSignals(){

	
	void (*func)(int);
	func = &GpClientNet::_interruptHandler;
	
	
	
	struct sigaction sigIntHandler;
	
	sigIntHandler.sa_handler = func;
	sigemptyset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;
	
	sigaction(SIGINT, &sigIntHandler, NULL);
	
}


void GpClientNet::_closeLogging(){
	
	if(GpClientNet::_metricsFile.is_open()){
		GpClientNet::_metricsFile.close();
		
	}

	
}


GpClientNet::GpClientNet(){

	_catchSignals();
	
	if(GP_INSTRUMENTATION_ON)
		turnOnLogging(true);
}

GpClientNet::GpClientNet(gp_message_handler message_handler){
	
	

	_catchSignals();
	
	
	
	_message_handler = message_handler;
	if(GP_INSTRUMENTATION_ON)
		turnOnLogging(true);
}

GpClientNet::~GpClientNet(){
	
	_closeLogging();
}


void GpClientNet::turnOnLogging(bool shouldTurnOnLogging){
	
	if(shouldTurnOnLogging){

		std::string dir = "log";
		int mkdirResult = mkdir(dir.c_str(), 0700);
		if(mkdirResult != EEXIST)
			std::cout << "[" << __func__ << "] " << "Logfile mkdir error: " << mkdirResult <<  ": "<< strerror(errno) << std::endl;
		
		//drop the upper four bytes. Don't need years, etc.
		uint32_t now =(uint32_t) std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		
		//create filename from current time
		std::string logFilename = (boost::format("%s/%u.csv") % dir % now ).str();
		
		
		std::cout << "[" << __func__ << "] " << "Opening logfile: " << logFilename <<std::endl;
		
		GpClientNet::_metricsFile.open(logFilename);
		if(!(GpClientNet::_metricsFile.is_open())){
			std::cout << "[" << __func__ << "] " << "Can't create metrics.csv" << std::endl;
		}else{
			std::cout << "[" << __func__ << "] " << "Logging to " << logFilename << std::endl;
			GpClientNet::_metricsFile << "DTG,RTT" << std::endl;
		}
	}
}




bool
GpClientNet::connectToServer(std::string ip, std::string port){
	
	
	for(;;){
	
	

		struct addrinfo hints, *server_info, *server_info_backup;
		memset(&hints, 0, sizeof(addrinfo));
		hints.ai_family = AF_UNSPEC;		//ipv4 or ipv6
		hints.ai_socktype = SOCK_STREAM;	//defaults to udp otherwise?

		if(::getaddrinfo(ip.c_str(), port.c_str(), &hints, &server_info) == -1){
			perror((boost::format("%s") % __func__ ).str().c_str() );
			goto GP_WAIT_AND_TRY_AGAIN_IN_A_BIT;
			//return false;
		}
		
		
		
		
		//ref: Stevens/Fenner/Rudoff
		server_info_backup = server_info;
		do {
			
			// Socket
			
			_fd = ::socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);
			if(_fd < 0){
				continue;
			}
			
	#ifdef GP_OSX	// Only works on mac. on linux the option is set in send()
			
			int yes = 1;
			if((setsockopt(_fd, SOL_SOCKET, SO_NOSIGPIPE, &yes, sizeof(int))) == -1){
				std::cout << "Socket options error SO_NOSIGPIPE" << std::endl;
				goto GP_WAIT_AND_TRY_AGAIN_IN_A_BIT;

				//exit(1);
			}
	#endif
			
			
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
			std::cout << "[" << __func__ << "] "  << "" << strerror(errno) << std::endl;
			goto GP_WAIT_AND_TRY_AGAIN_IN_A_BIT;
			//return false;
		}
		::freeaddrinfo(server_info_backup);
		
		_isConnected = true;
			
		return true;	//if you get this far it worked. Otherwise you skipped passed, slept, then tried again.
			
		GP_WAIT_AND_TRY_AGAIN_IN_A_BIT:
		_isConnected = false;
		usleep(5000000);		//deprecated for nanosleep...
		
	} // for(;;)
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
	
	return _sendMessage(message);
	
}






bool
GpClientNet::_sendMessage(GpMessage &message){
	

	std::lock_guard<std::mutex> sendGuard(_send_mutex);		// when guard class is destroyed at end of function the lock is released
	
	
	
	if(_fd == 0){
		std::cout << "[" << __func__ << "] "  << "Can't send. _fd closed." << std::endl;
		return false;
	}
	
	
	
	
	// Don't change a pong's timestamp
	if(message._message_type != GP_MSG_TYPE_PONG)
		message.setTimestampToNow();
	//else
	//	std::cout << "Sending Pong" << std::endl;
	
	
	// std::cout << "[" << __func__ << "] "  << "Sending message type: " << int(message._message_type) << " on socket: " << _fd << " , time: " << uint32_t(message._timestamp) << std::endl;
	
	
	
	
	
	
	byte_vector bytes;
	bytes.reserve(message.size());
	message.serialize(bytes);
	
	
	
	
	// Need mac/linux #ifdefs here for SIGPIPE
	
	
	
	
	
	ssize_t result = send(_fd, bytes.data(), bytes.size(), MSG_DONTWAIT);	//MSG_DONTWAIT -- If set send returns EAGAIN if outbond traffic is clogged. A one-time "skip it."
																			//MSG_NOSIGNAL
	if(result == -1){
		perror((boost::format("[%s] Error %d:")  % __func__ % errno).str().c_str() );

		if(EAGAIN == errno){

			std::cout << "[" << __func__ << "] "  << "EAGAIN" << std::endl;
			// message was skipped. Try again? Or disgregard? Might depend on message type.

		}
		else if(errno == EPIPE){
			
			// Server's gone. Need to quit this thread and reconnect.
			
			std::cout << "[" << __func__ << "] "  << "Server disconnected (EPIPE)." << std::endl;
			
			//user.logout();
			
			//exit(EPIPE);
			
			
		}
		return false;
	}
	else if (result == 0){
		perror((boost::format("%s: connection closed") % __func__ ).str().c_str() );
		return false;
	}
	return true;
}




void GpClientNet::startListenerAsThread(GpClientNet::gp_message_handler message_handler, std::thread * listenThread){
	
	if(message_handler != nullptr){
		
		_message_handler = message_handler;
		
		std::thread listenThread(&GpClientNet::_listen_for_TCP_messages, this);
		listenThread.detach();
		
	}
	
}

void GpClientNet::startListenerAsThread(GpClientNet::gp_message_handler message_handler){

	if(message_handler != nullptr){
	
		_message_handler = message_handler;

		std::thread listenThread(&GpClientNet::_listen_for_TCP_messages, this);
		listenThread.detach();
	
	}
	
}








/**
 *  Listen for GpMessages from server
 *
 */
bool
GpClientNet::_listen_for_TCP_messages(){
	// std::cout << "[" << __func__ << "] "  <<  "" << std::endl;


	if(_message_handler != nullptr && _fd > 0){

		_receiveDataAndParseMessage();		//blocks in recv(). When a message is complete message_handler() is called on the GpMessage.

	}
	else{
	
		//std::cout << "[" << __func__ << "] "  <<  "_message_handler or _fd weren't set" << std::endl;
		return false;
		
	}

	
	
	return true; //started okay
}
























// JUNK FROM OLD CONTROLLER CLIENT, CLEAN UP AND PUT IN _list_for_TCP_messages()



// Only required by receiveDataAndParseMessage (DELETE EVENTUALLY)
void putHeaderInMessage(uint8_t *&buffer, long size, GpMessage & message){
	
	//uint8_t *ptr = buffer;
	
	// Message_Type
	message._message_type = *buffer; //GP_MSG_TYPE_CONTROLLER_LOGIN;
	buffer+=1;
	
	// Payload Size
	uint8_t *sizePtr = buffer;
	uint16_t pSize = 0;
	GpMessage::byteUnpack16(sizePtr, pSize);
	message._payloadSize = pSize;			//GP_MSG_LOGIN_LEN;
	buffer+= (sizeof(uint16_t));
	
	
	
	sizePtr = buffer;
	uint32_t timestamp = 0;
	GpMessage::byteUnpack32(sizePtr, timestamp);
	message._timestamp = timestamp;
	buffer+= (sizeof(uint32_t));
	
	
	
	
	
	
	return;
}



void GpClientNet::_receiveDataAndParseMessage()
{
	
	
	
	
	
	// std::cout << "[" << __func__ << "] "  <<  "" << std::endl;
	
	
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
	GpMessage newMessage;		// dynamically allocate? or overwrite each time?

	for(;;){
		
		
		// RECEIVE
		
		bytesInRecvBuffer = recv(_fd, recvInPtr, length, 0);		//blocks if no data, returns zero if no data and shutdown occurred
		if(bytesInRecvBuffer == -1){
			// std::cout << "[" << __func__ << "] "  << "recv() error" << std::endl;
			return;
		}
		else if(bytesInRecvBuffer == 0){
			//shutdown, if zero bytes are read recv blocks, doesn't return 0 except if connection closed.
			return;
		}
		
		
		// std::cout << "[" << __func__ << "] "  << "recv() read: " << bytesInRecvBuffer << " bytes" << std::endl;
		
		
		
		while(bytesInRecvBuffer > 0){
			
			
			if(messageStarted != true){
				
				// Start a message
				newMessage.clear();
				
				
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
				
				std::cout << "[" << __func__ << "] "  << "Received type: " << int(newMessage._message_type)
						<< " , payload size: " << newMessage._payloadSize
						<< " , timestamp: " << uint32_t(newMessage._timestamp)
						<< std::endl;

				
				// Clean up (before getting guard on message_handler)
				recvOutPtr+= bytesToTransfer;
				// Reset the message buffer.
				messageStarted = false;
				messageLenCurrent = 0;
				messageLenMax = 0;
				
				
/*
				
				// HANDLE_MESSAGE
				
				// Get a lock in case message_handler isn't thread safe.
				std::lock_guard<std::mutex> sendGuard(_message_handler_mutex);		// when guard class is destroyed at end of scope the lock is released
*/
				

				if(newMessage._message_type == GP_MSG_TYPE_PING){
					
					//bounce it back to sender with a pong and new message type. time stamp stays the same.

					newMessage._message_type = GP_MSG_TYPE_PONG;

					sendMessage(newMessage);
				
				}
				else if(newMessage._message_type == GP_MSG_TYPE_PONG){
					
					// analyze round trip time
					
					compareRoundTripTime(newMessage);
					
				}
				else{

					// Get a lock in case message_handler isn't thread safe.
					//std::lock_guard<std::mutex> sendGuard(_message_handler_mutex);		// when guard class is destroyed at end of scope the lock is released
					//_message_handler(newMessage, *this);		// Do the code given by the calling entity.
					
					callMessageHandler(newMessage);
				}
				
				
				
				
				
				
				
				newMessage.clear();
			}
			
		}	// while(bytesInRecvBuffer > 0)
		recvInPtr = recvHead;			// reset both buffer pointers to head if it's empty
		recvOutPtr = recvHead;
	}
}

void GpClientNet::callMessageHandler(GpMessage & msg){
	// Get a lock in case message_handler isn't thread safe.
	std::lock_guard<std::mutex> sendGuard(_message_handler_mutex);		// when guard class is destroyed at end of scope the lock is released
	_message_handler(msg, *this);		// Do the code given by the calling entity.
	
	
}



void GpClientNet::sendAuthenticationRequest(std::string username, std::string key, int gp_msg_source_type){
	
	//encrypt something with the symmetric key I share with the server
	//server decrypts to know I'm who I am.
	
	
	GpMessage_Login loginMessage(username, key);
	GpMessage message(loginMessage, gp_msg_source_type); // GP_MSG_TYPE_ASSET_LOGIN or GP_MSG_TYPE_CONTROLLER_LOGIN
	// std::cout << "[" << __func__ << "] "  << "username/pwd: " << loginMessage.username() << "/" << loginMessage.key() << std::endl;
	
	
	sendMessage(message);
	
	
}






//void GpClientNet::sendHeartbeat(){
void GpClientNet::sendHeartbeat(){
	
	for(;;){
		
//		 std::cout << "[" << __func__ << "] "  << "Sending heartbeat to server on socket: " << _fd << std::endl;
		
		 uint8_t *payload = nullptr;
		 GpMessage heartbeat(GP_MSG_TYPE_HEARTBEAT, 0, payload);
		
		 if(sendMessage(heartbeat)==false)
			 return;
		 
		
		
		usleep(3000000);
	}
	
}

void GpClientNet::startBackgroundHeartbeat(){
	
	if(GP_SHOULD_SEND_HEARTBEAT_TO_SERVER_FROM_ASSET){
		//std::cout << "[" << __func__ << "] "  << "Starting heartbeat thread" << std::endl;
		
		std::thread serverHeartbeat(&GpClientNet::sendHeartbeat,this);
		serverHeartbeat.detach();
		
	}
}




void GpClientNet::startBackgroundPing(){
	
	if(GP_SHOULD_SEND_HEARTBEAT_TO_SERVER_FROM_ASSET){
		// std::cout << "[" << __func__ << "] "  << "Starting ping thread" << std::endl;
		
		std::thread serverHeartbeat(&GpClientNet::_sendPing,this);
		serverHeartbeat.detach();
		
	}
}

void GpClientNet::_sendPing(){
	
	for(;;){
		
		// std::cout << "[" << __func__ << "] "  << "Sending ping: " << _fd << std::endl;
		
		uint8_t *payload = nullptr;
		GpMessage heartbeat(GP_MSG_TYPE_PING, 0, payload);
		
		if(sendMessage(heartbeat)==false)
			return;
		
		
		
		usleep(250000);		// 0.25 seconds  = 4 pings per second
	}
	
}




void GpClientNet::compareRoundTripTime(GpMessage & msg){
	
	
	//drop the upper four bytes. Don't need years, etc.
	uint32_t now =(uint32_t) std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	
	uint32_t rtt = (now - msg._timestamp);
	
	std::cout
			//<< "\n\n[" << __func__ << "] " << "Now: " << uint32_t(now)
			//<< "\nSent " << msg._timestamp
			<< "\nRTT (ms)" << rtt << "\n\n"
			<< std::endl;
	
	
	
	if(GP_INSTRUMENTATION_ON && GpClientNet::_metricsFile.is_open()){
	
		
		//create filename from current time
		std::string logFilename = (boost::format("%u.csv") % now ).str();
		
		uint64_t completeTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		GpClientNet::_metricsFile << (boost::format("%u,%u") % completeTime % rtt ).str() << std::endl;
	

		
		
	}
	//std::cout << "[" << __func__ << "] " << "Timenow: " << uint32_t(now) << std::endl;
	
	
	
	
	
	
	
	
}
