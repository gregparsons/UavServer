// ********************************************************************************
//
//  GpUavAsset.h
//  UavServer
//  2/25/15
//
// ********************************************************************************




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
#include <boost/format.hpp>



#include <thread>

#include "GpUavServer.h"
#include "GpIpAddress.h"
#include "GpMavlink.h"
#include "GpMessage.h"
#include "GpMessage_Login.h"
#include "GpDatabase.h"
#include "GpUser.h"








void GpUavServer::sendHeartbeat(GpUser user){
	
	for(;;){
		
		// FAKE MESSAGE
		//std::cout << "[" << __func__ << "] "  << "Sending fake heartbeat to user: " << user._username << " on socket: " << user._fd << std::endl;
		
		
		// Send login complete message to client
		uint8_t *payload = nullptr;
		GpMessage msgLoginComplete(GP_MSG_TYPE_HEARTBEAT, 0, payload);
		
		if(false == sendMessageToController(msgLoginComplete, user)){
			std::cout << "[" << __func__ << "] "  << "Error sending heartbeat" << std::endl;
			
			return;
		}
		
		usleep(3000000);
	}
	
}



/**
 *  signalHandler(): Clean up zombie processes.
 *
 */
/*
 void
signalHandler(int signal)
{
	while(waitpid(-1, NULL, WNOHANG) > 0);
	cout << "signalHandler()" << std::endl;
}
*/




/**
 *  start()
 *
 *  Calls startNetwork() which blocks infinitely to accept connections. So this will never return.
 *
 */
bool GpUavServer::start(){
	
	std::cout << "[" << __func__ << "] "  << "Server starting..." <<std::endl;
	
	return startNetwork();
	
}





GpUavServer::~GpUavServer(){
	
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
	std::cout << "[" << __func__ << "] "  << "" <<std::endl;

	struct addrinfo hintAddrInfo, *resSave = nullptr, *res = nullptr;
	struct sockaddr_storage inboundAddress;
	socklen_t addrLen = 0;
	//struct sigaction signalAction;
	int result = 0;
	int yes = 1;
	
	int client_fd = 0;
	
	// Handle SIGPIPE, etc processes
	
	signal(SIGPIPE, SIG_IGN);
	
	/*
	
	signalAction.sa_handler = signalHandler;
	sigemptyset(&signalAction.sa_mask);
	signalAction.sa_flags = SA_RESTART;
	result = sigaction(SIGCHLD, &signalAction, nullptr);
	if(result == -1){
		cout << "Error: sigaction" << std::endl;
		exit(1);
	}
	*/
	
	// Network Begin
	
	memset(&hintAddrInfo, 0, sizeof(addrinfo));
	hintAddrInfo.ai_family = AF_UNSPEC;		//any protocol family
	hintAddrInfo.ai_socktype = SOCK_STREAM;	//stream = TCP
	hintAddrInfo.ai_flags = AI_PASSIVE;		//
	

	// 0.0.0.0
	result = getaddrinfo("0.0.0.0", GP_CONTROLLER_SERVER_PORT.c_str(), &hintAddrInfo, &resSave);
	if(result < 0){ //error
		std::cout << "getaddrinfo() error" << std::endl;
		return 1;
	}
	
	
	
	// Socket
	
	for(res = resSave; res != nullptr; res = res->ai_next){
		
		_listen_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if(_listen_fd == -1){ //error, else file descriptor
			std::cout << "Socket error" << std::endl;
			continue;
		}
		result = setsockopt(_listen_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
		if(result == -1){
			std::cout << "socket options error" << std::endl;
			exit(1);
		}

		// Bind
		if(::bind(_listen_fd, res->ai_addr, res->ai_addrlen) == -1){
			std::cout << "Bind Error: " << strerror(errno) << std::endl;
			exit(1);
		}
		break;
		
	}
	if(res == nullptr){
		std::cout << "Failed to bind" << std::endl;
		return 2;
	}
	freeaddrinfo(resSave);	//done with this
	
	
	// Listen
	if(listen(_listen_fd, MAX_CONNECTION_BACKLOG) == -1){
		std::cout << "Listen error" << std::endl;
		exit(1);
	}
	
	
	// Accept Loop (accept blocks waiting for connections)
	std::cout << "[" << __func__ << "] "  << "Server: waiting for connections" <<std::endl;

	for(;;){
		
		// Accept
		
		addrLen = sizeof(inboundAddress);		//address len for accept should be size of address struct, but on return it gets set to bytes of the actual string (or struct?) address
		client_fd = accept(_listen_fd, (struct sockaddr *)&inboundAddress, &addrLen);
		
		if(client_fd == -1){
			std::cout << "Error: accept(): " << strerror(errno) << std::endl;
			switch (errno) {
				case EBADF:		//errno.h
					break;
				default:
					break;
			}
			continue;
		}
		

		
		
		// CLIENT THREAD -- START
		
		std::thread clientThread (&GpUavServer::threadClientRecv, *this, client_fd);
		clientThread.detach();
		usleep(10000);


	}
}










void GpUavServer::threadClientRecv(int fd)
{
	

	std::cout << "[" << __func__ << "] "  << "Entering recv() thread " << std::this_thread::get_id() << std::endl;
	
	GpUser *user = nullptr;
	
	
	
	
	
	
	
	
	
	
	
	
	

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
		
		
		bytesInRecvBuffer = recv(fd, recvInPtr, length, 0);		//blocks if no data, returns zero if no data and shutdown occurred
		if(bytesInRecvBuffer == -1){
			std::cout << "Error: recv()" << std::endl;
			//exit(1);
			return;
		}
		else if(bytesInRecvBuffer == 0){
			//shutdown, if zero bytes are read recv blocks, doesn't return 0 except if connection closed.
			//exit(0);
			return;
		}

		std::cout << "[" << __func__ << "] "  << "recv(): " << bytesInRecvBuffer << " bytes" <<std::endl;
		
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
				
				
				
				// Create User Object; Parse Message

				// Dynamic allocate new user depending on type of login message received...
				// ...if thread sigpipes in this infinite loop, will this memory get released?
				if(user == nullptr){
					if(newMessage._message_type == GP_MSG_TYPE_ASSET_LOGIN){
						user = new GpAssetUser();
						
					}
					else if(newMessage._message_type == GP_MSG_TYPE_CONTROLLER_LOGIN){
						user = new GpControllerUser();
						
					}
					else{
						goto SKIP_THIS_MESSAGE_UNTIL_LOGGED_IN;
					}

					
					user->_fd = fd;	//assign socket file descriptor received in function parameter

					
					// **** HEARTBEAT move this to handler; run once logged in this is dumb that this is even here. *****
					
					if(GP_SHOULD_SEND_HEARTBEAT_SERVER_TO_CONTROLLER){
						std::cout << "[" << __func__ << "] "  << "TEST: Starting heartbeat" << std::endl;
						
						std::thread serverHeartbeat(&GpUavServer::sendHeartbeat, *this, *user);
						serverHeartbeat.detach();
						
					}

					
					
					
					
				}

				
			SKIP_THIS_MESSAGE_UNTIL_LOGGED_IN:
				if(user!= nullptr)
					processMessage(newMessage, *user);
				
				
				
				
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
	
	
	//Never reached:
	if(user!=nullptr)
		delete user;
	
	
}





void GpUavServer::putHeaderInMessage(uint8_t *&buffer, long size, GpMessage & message){
	
	// Message_Type
	message._message_type = *buffer; //GP_MSG_TYPE_CONTROLLER_LOGIN;
	
	
	
	// Payload Size
	uint8_t *sizePtr = buffer + 1;
	uint16_t pSize = 0;
	GpMessage::bitUnstuff16(sizePtr, pSize);
	message._payloadSize = pSize;			//GP_MSG_LOGIN_LEN;
	
	
	return;
}




void GpUavServer::processMessage(GpMessage & msg, GpUser & user){

	// REMOVE user parameter. Not needed.
	
	
	
	
	
	
	std::cout << "[" << __func__ << "] "  << "Received message with type: " << int(msg._message_type) << " and payload size: " << msg._payloadSize << std::endl;

	
	if(user._isAuthenticated){
		switch (msg._message_type) {
			
			case GP_MSG_TYPE_COMMAND:
			{
				std::cout << "[" << __func__ << "] "  << "GP_MSG_TYPE_COMMAND" << std::endl;

				GpMavlink::printMavFromGpMessage(msg);
				
				/*
				 
				 
				 
				 Here, re-send message to asset
				 
				 
				 
				 */
				
				
				if(typeid(user).name() == typeid(GpControllerUser).name()){
					GpControllerUser &controller = (dynamic_cast<GpControllerUser&>(user));
					
					if(controller._asset._connected){
						

						sendMessageToController(msg, controller._asset);
						
					}
				}
				break;
				
			}
				
			case GP_MSG_TYPE_LOGOUT:
			{
				
				//GpDatabase::logout();
				//user.isAuthenticated set to false in db logout
				std::cout << "[" << __func__ << "] "  << "Processing logout message" << std::endl;
				break;
			}
			case GP_MSG_TYPE_HEARTBEAT:
			{
				
				//GpDatabase::logout();
				//user.isAuthenticated set to false in db logout
				std::cout << "[" << __func__ << "] "  << "GP_MSG_TYPE_HEARTBEAT from " << user._username << " on socket " << user._fd << std::endl;
				
				
				
				
				// Who is asset's owner? Need to send heartbeat to them.
				
				
				break;
			}
				
			default:
			{
				std::cout << "[" << __func__ << "] "  << "Message type: "<< msg._message_type << std::endl;
				break;
			}
		}
	}
	else{

		// If you're not authenticated, the only message you can even send is an authentication request.
		
		
		switch (msg._message_type) {
				
				
			case GP_MSG_TYPE_CONTROLLER_LOGIN:
			{
				
				std::cout << "[" << __func__ << "] "  << "CONTROLLER login message" << std::endl;
				GpMessage_Login loginMsg(msg._payLd_vec);
				
				
				
				// AUTHENTICATE
				
				if(true == (user.authenticate(loginMsg.username(), loginMsg.key())   ))
				{
					
					std::cout << "[" << __func__ << "] "  << "Sending authentication confirmation to: " << user._username << "on socket " << user._fd << std::endl;
					
					
					
					// moved temporarily lower as response to asset connect request (signals controller to start sending game controller outputs)
					//sendLoginConfirmationMessageTo(user);

				
					
					// ********* TEST: insert a fake asset **********
#ifdef GP_SHOULD_INSERT_FAKE_ASSET
					GpAssetUser asset;
					asset._user_id = GP_ASSET_ID_TEST_ONLY;
					asset._username = "testing";
					asset._connected = true;
					GpDatabase::insertAsset(asset);
#endif
					
					// If this is a Controller, try to connect to the Asset.
					
					std::cout << "[" << __func__ << "] User type: "  << typeid(user).name() << std::endl;
					if(typeid(user).name() == typeid(GpControllerUser).name()){
						bool connectedToAsset = (dynamic_cast<GpControllerUser &>(user)).requestConnectionToAsset(GP_ASSET_ID_TEST_ONLY);
					
						if(connectedToAsset){
							
							// send message to tell controller to start sending. putting the login confirm message here for now. Make separate message in the
							sendLoginConfirmationMessageTo(user);
						}
					}
				}
				break;
			}
				
			case GP_MSG_TYPE_ASSET_LOGIN:
			{
				
				std::cout << "[" << __func__ << "] "  << "ASSET login message" << std::endl;
				GpMessage_Login loginMsg(msg._payLd_vec);
				
				
				
				// AUTHENTICATE
				
				if(true == (user.authenticate(loginMsg.username(), loginMsg.key())   ))
				{
					
					
					
					
					
					
					std::cout << "[" << __func__ << "] "  << "Sending authentication confirmation to: " << user._username << "on socket " << user._fd << std::endl;
					sendLoginConfirmationMessageTo(user);
					
				}
				break;
			}
			default:
				break;
		}
	}
	
	
	
	
	

}







/**
 *  Send serialized message to the controller over TCP.
 *
 *  @param GpMessage & msg
 *  @returns Bool
 */
bool GpUavServer::sendMessageToController(GpMessage & msg, GpUser & user){
	
	
	// SERIALIZE
	
	std::vector<uint8_t> byteVect;
	byteVect.reserve(msg.size());
	msg.serialize(byteVect);
	
	
	// SEND TCP
	
	ssize_t retVal = send(user._fd, byteVect.data(), byteVect.size(), 0);	//this will be a new size based on the actual size of the serialized data (not the default above)
	if(retVal == -1){

		perror((boost::format("[%s] Error %d:")  % __func__ % errno).str().c_str() );

		/*
		if(errno == EPIPE){
			
			// client disconnected; clean up. Mark client disconnected. Remove from controller list.

			
		}
		*/
		
		return false;
		
		
		
	}
	return true;
}




void GpUavServer::sendLoginConfirmationMessageTo(GpUser & user){

	
	
	// Send login complete message to client
	uint8_t *payload = nullptr;	// No payload required, just a ref to a pointer needed for constructor.
	GpMessage msgLoginComplete(GP_MSG_TYPE_AUTHENTICATED_BY_SERVER, 0, payload);
	
	if(sendMessageToController(msgLoginComplete, user) == false){
		std::cout << "[" << __func__ << "] "  << "Error sending login confirmation" << std::endl;
	}else{
		std::cout << "[" << __func__ << "] "  << "Authentication confirmation sent." << std::endl;
	}

}