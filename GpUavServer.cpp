// ********************************************************************************
//
//  GpUavAsset.h
//  UavServer
//  2/25/15
//
// ********************************************************************************

#define MAX_CONNECTION_BACKLOG 10
#define READ_VECTOR_BYTES_MAX 300


#include <mavlink/c_library/common/mavlink.h>
#include <iostream>
#include <bitset>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>	//
#include <signal.h>
#include <unistd.h>		//fork()
#include <vector>
#include <string.h>			//memset, bzero
#include <sys/wait.h>


#include <thread>

#include "GpUavServer.h"
#include "GpIpAddress.h"
#include "GpMavlink.h"
#include "GpMessage.h"
#include "GpMessage_Login.h"
#include "GpDatabase.h"

using namespace std;





void GpUavServer::sendHeartbeat(){
	
	for(;;){
	
		// FAKE MESSAGE
		std::cout << "Sending fake message as heartbeat" << std::endl;
		
		
		
		
		// Send login complete message to client
		uint8_t *payload = nullptr;
		GpMessage msgLoginComplete(GP_MSG_TYPE_AUTHENTICATED_BY_SERVER, 0, payload);
		
		if(false == sendMessageToController(msgLoginComplete)){
			std::cout << "[GpUavServer::processMessage] Error sending login confirmation" << std::endl;
		}
		
		usleep(3000000);
	}
	
}





/**
 *  start()
 *
 *  Calls startNetwork() which blocks infinitely to accept connections. So this will never return.
 *
 */
bool GpUavServer::start(){
	
	std::cout << "Server starting..." <<std::endl;
	
	
	
	
	
	return startNetwork();
	
}





/**
 *  signalHandler(): Clean up zombie processes.
 *
 */
void
signalHandler(int signal)
{
	while(waitpid(-1, NULL, WNOHANG) > 0);
	cout << "signalHandler()" << endl;
}



/**
 *  startNetwork()
 *
 *  Performs an infinite loop accepting TCP connections. When a connection is accepted, it's forked and 
 *  _client_fd is the active socket. The parent closes that socket and the child closes the listener socket.
 *
 */
bool
GpUavServer::startNetwork(){

	struct addrinfo hintAddrInfo, *resSave = nullptr, *res = nullptr;
	struct sockaddr_storage inboundAddress;
	socklen_t addrLen = 0;
	struct sigaction signalAction;
	int result = 0;
	int yes = 1;
	
	// Handle zombie processes
	
	signalAction.sa_handler = signalHandler;
	sigemptyset(&signalAction.sa_mask);
	signalAction.sa_flags = SA_RESTART;
	result = sigaction(SIGCHLD, &signalAction, nullptr);
	if(result == -1){
		cout << "Error: sigaction" << endl;
		exit(1);
	}
	
	
	// Network Begin
	
	memset(&hintAddrInfo, 0, sizeof(addrinfo));
	hintAddrInfo.ai_family = AF_UNSPEC;		//any protocol family
	hintAddrInfo.ai_socktype = SOCK_STREAM;	//stream = TCP
	hintAddrInfo.ai_flags = AI_PASSIVE;		//
	
	result = getaddrinfo(GP_CONTROLLER_SERVER_IP.c_str(), GP_CONTROLLER_SERVER_PORT.c_str(), &hintAddrInfo, &resSave);
	if(result < 0){ //error
		cout << "getaddrinfo() error" << endl;
		return 1;
	}
	
	
	
	// Socket
	
	for(res = resSave; res != nullptr; res = res->ai_next){
		
		_listen_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if(_listen_fd == -1){ //error, else file descriptor
			cout << "Socket error" << endl;
			continue;
		}
		result = setsockopt(_listen_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
		if(result == -1){
			cout << "socket options error" << endl;
			exit(1);
		}
		
		
		// Bind
		
		result = ::bind(_listen_fd, res->ai_addr, res->ai_addrlen);
		if(result == -1){
			
//			char err[20];
//			perror(err);
			cout << "Bind Error: " << strerror(errno) << endl;
			exit(1);
		}
		break;
		
	}
	if(res == nullptr){
		cout << "Failed to bind" << endl;
		return 2;
	}
	freeaddrinfo(resSave);	//done with this
	
	
	
	// Listen
	
	result = listen
	(_listen_fd, MAX_CONNECTION_BACKLOG);
	if(result == -1){
		cout << "Listen error" << endl;
		exit(1);
	}
	
	
	// Accept Loop (accept blocks waiting for connections)
	
	cout << "Server: waiting for connections" << endl;
	for(;;){
		
		// Accept
		
		addrLen = sizeof(inboundAddress);		//address len for accept should be size of address struct, but on return it gets set to bytes of the actual string (or struct?) address
		_client_fd = accept(_listen_fd, (struct sockaddr *)&inboundAddress, &addrLen);
		
		if(_client_fd == -1){
			cout << "Error: accept(): " << strerror(errno) << endl;
			switch (errno) {
				case EBADF:		//errno.h
					break;
				default:
					break;
			}
			continue;
		}
		
		
		
		
		// TEST SERVER HEARTBEAT
		
		
		std::thread serverHeartbeat(&GpUavServer::sendHeartbeat, this);
		serverHeartbeat.detach();

		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		// RECEIVE THREAD -- START
		
		std::thread clientThread (&GpUavServer::threadClientRecv, this);
		clientThread.detach();
		
		std::cout << "[GpUavServer::startNetwork] after starting thread" << endl;
		
		//clientThread.join();

	}
}

















/**
 *  forkClientRecv
 *
 *  Performs the forked client recv(). Exits the child process upon completion (which should not be 
 *  until client disconnects).
 *
 *  @param int & _client_fd
 *  @returns void
 */
//void GpUavServer::forkClientRecv(int & _client_fd)
void GpUavServer::threadClientRecv()
{
	

	std::cout << "[GpUavServer::threadClientRecv] starting thread: " << std::this_thread::get_id() << std::endl;
	

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
	
	for(;;){
		
		
 		// RECEIVE
		
GP_RECEIVE:
		
		bytesInRecvBuffer = recv(_client_fd, recvInPtr, length, 0);		//blocks if no data, returns zero if no data and shutdown occurred
		recvInPtr += bytesInRecvBuffer;
		if(bytesInRecvBuffer == -1){
			cout << "Error: recv()" << endl;
			//exit(1);
			return;
		}
		else if(bytesInRecvBuffer == 0){
			//shutdown, if zero bytes are read recv blocks, doesn't return 0 except if connection closed.
			//exit(0);
			return;
		}

		
		cout << "\nRead: " << bytesInRecvBuffer << " bytes" << endl;

		
		
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
				long bytesToTransfer = min(bytesInRecvBuffer, msgBytesStillNeeded);		//first time, copy as many as possible from packet buffer
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
				long bytesToTransfer = min(bytesInRecvBuffer, bytesStillNeeded);
				
				memcpy(&messageBuffer, recvOutPtr, bytesToTransfer);
				messageLenCurrent += bytesToTransfer;
				recvOutPtr += bytesToTransfer;
				bytesInRecvBuffer -= bytesToTransfer;
				
			}
			
			
			// MESSAGE COMPLETE. PROCESS.
			if(messageLenCurrent == messageLenMax)
			{
				newMessage._payload = msgHead+3;	//okay, but this is about to get cleared with arrival of next packet, message here is the entire message, not the payload
				std::cout << "Received message with type: " << newMessage._message_type << " and payload size: " << newMessage._payloadSize << std::endl;
				
				
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
		}
		
		
	}
}





void GpUavServer::putHeaderInMessage(uint8_t *&buffer, long size, GpMessage & message){
	
	uint8_t *ptr = buffer;
	
	// Message_Type
	message._message_type = *buffer; //GP_MSG_TYPE_LOGIN;
	
	
	
	// Payload Size
	uint8_t *sizePtr = buffer + 1;
	uint16_t pSize = 0;
	GpMessage::bitUnstuff16(sizePtr, pSize);
	message._payloadSize = pSize;			//GP_MSG_LOGIN_LEN;
	
	
	return;
}




void GpUavServer::processMessage(GpMessage & msg){

	
	switch (msg._message_type) {
		case GP_MSG_TYPE_COMMAND:
		{
			std::cout << "[GpUavServer::processMessage] Processing COMMAND message" << std::endl;

			
			break;

		}
		case GP_MSG_TYPE_LOGIN:
		{
			std::cout << "[GpUavServer::processMessage] Processing login message" << std::endl;

			// Authenticate
			GpMessage_Login loginMsg(msg._payload);
			if(true == (GpDatabase::authenticateUser(loginMsg.username(), loginMsg.key())))
			{
				std::cout << "User: " << loginMsg.username() << " authenticated." << std::endl;
				
				
				
				
				
				// Send login complete message to client
				uint8_t *payload = nullptr;
				GpMessage msgLoginComplete(GP_MSG_TYPE_AUTHENTICATED_BY_SERVER, 0, payload);

				if(false == sendMessageToController(msgLoginComplete)){
					std::cout << "[GpUavServer::processMessage] Error sending login confirmation" << std::endl;
				}
				   
				
				
				
			}
			break;
		}
		case GP_MSG_TYPE_LOGOUT:
		{
			std::cout << "[GpUavServer::processMessage] Processing logout message" << std::endl;
			break;
		}
		case GP_MSG_TYPE_GENERIC:
		{
			std::cout << "[GpUavServer::processMessage] Processing generic message" << std::endl;
			break;
		}
		default:
				break;
	}
	
	
	
}

/**
 *  Send serialized message to the controller over TCP.
 *
 *  @param GpMessage & msg
 *  @returns Bool
 */
bool GpUavServer::sendMessageToController(GpMessage & msg){
	
	uint8_t bytes[GP_MSG_MAX_LEN];		//
	uint8_t *bytePtr = bytes;
	uint16_t size = sizeof(bytes);		//this will return with the actual size of the message
	msg.serialize(bytePtr, size);
	
	
	ssize_t retVal = send(_client_fd, bytePtr, size, 0);	//this will be a new size based on the actual size of the serialized data (not the default above)
	if(retVal == -1)
		return false;
	return true;
}



