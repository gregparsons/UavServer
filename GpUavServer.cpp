// ********************************************************************************
//
//  GpUavAsset.h
//  UavServer
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
#include <typeinfo>	//required by Raspbian for typeid
#include <thread>
#include <mutex>

#include "GpUavServer.h"
#include "GpIpAddress.h"
#include "GpMavlink.h"
#include "GpMessage.h"
#include "GpMessage_Login.h"
#include "GpDatabase.h"
#include "GpUser.h"


GpUavServer::~GpUavServer(){
	
}




/**
 *  start()
 *
 *  Calls startNetwork() which blocks infinitely to accept connections. So this will never return.
 *
 */
bool GpUavServer::start(){
	// std::cout << "[" << __func__ << "] "  << "Server starting..." <<std::endl;
	
	return startNetwork();
	
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
	// std::cout << "[" << __func__ << "] "  << "" <<std::endl;

	struct addrinfo hintAddrInfo, *resSave = nullptr, *res = nullptr;
	struct sockaddr_storage inboundAddress;
	socklen_t addrLen = 0;
	int result = 0;
	int yes = 1;
	int client_fd = 0;

	
	
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
#ifdef GP_OSX	// Only works on mac. on linux the option is set in send()
		result = setsockopt(_listen_fd, SOL_SOCKET, SO_NOSIGPIPE, &yes, sizeof(int));

		if(result == -1){
			std::cout << "socket options error SO_NOSIGPIPE" << std::endl;
			exit(1);
		}
#endif
		result = setsockopt(_listen_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
		if(result == -1){
			std::cout << "socket options error SO_REUSEADDR" << std::endl;
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
		
		// CLIENT LISTEN THREAD -- START
		
		std::thread clientThread (&GpUavServer::client_listener_thread, this, client_fd);
		clientThread.detach();
		usleep(10000);

	}
}




/**
 *  sendHeartbeatTo
 *
 */
void GpUavServer::sendHeartbeatTo(GpUser user){
	
	for(;;){
		std::cout << "[" << __func__ << "] "  << "Sending server heartbeat to : " << user._username << " on socket: " << user._fd << std::endl;
		
		// Send login complete message to client
		uint8_t *payload = nullptr;
		GpMessage heartbeatMessage(GP_MSG_TYPE_HEARTBEAT, 0, payload);
		
		if(false == _send_message(heartbeatMessage, user)){
			std::cout << "[" << __func__ << "] "  << "Error sending heartbeat." << std::endl;
			
			return;
		}
		
		usleep(3000000);
	}
}




/**
 *  client_listener_thread
 *
 *  Function started as a thread when a client connects. This is the incoming message handler.
 *
 *  @param int file_descriptor
 */
void GpUavServer::client_listener_thread(int fd)
{
	std::cout << "[" << __func__ << "] "  << "Spawning client listener thread on socket " << fd << std::endl;
	
	GpUser *user = nullptr;
	
	
	
	
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
	long bytesToTransfer = 0;

	
	
	
	// RECEIVE LOOP

	GpMessage newMessage;
	
	for(;;){
		
		
 		// RECEIVE
		
		
		bytesInRecvBuffer = recv(fd, recvInPtr, length, 0);		//blocks if no data, returns zero if no data and shutdown occurred
		if(bytesInRecvBuffer == -1){
			// std::cout << "Error: recv()" << std::endl;
			break;
		}
		else if(bytesInRecvBuffer == 0){
			break;
		}

		std::cout << "[" << __func__ << "] "  << "recv(): " << bytesInRecvBuffer << " bytes" <<std::endl;
		
		
		// If there are bytes on the receive queue, turn them into a message or return to recv until a message can be formed.
		while(bytesInRecvBuffer > 0){
			
			if(messageStarted != true){

				newMessage.clear();			// re-use

				if(bytesInRecvBuffer >= GP_MSG_HEADER_LEN){
					putHeaderInMessage(recvOutPtr, bytesInRecvBuffer, newMessage);		//this is the same as deserialize
				}
				else{
					break;																// Don't have enough bytes for a message header. Nothing to do but get more.
				}
				messageLenMax = newMessage._payloadSize + GP_MSG_HEADER_LEN;
				msgBytesStillNeeded = messageLenMax;
				bytesToTransfer = std::min(bytesInRecvBuffer, msgBytesStillNeeded);		//first time, copy as many as possible from packet buffer
				memcpy(&messageBuffer, recvOutPtr, bytesToTransfer);
				
			}
			else																		// if(messageStarted)
			{
				
				//copy the min of available and needed from PACKET to MESSAGE buffer
				msgBytesStillNeeded = messageLenMax - messageLenCurrent;
				bytesToTransfer = std::min(bytesInRecvBuffer, msgBytesStillNeeded);
				memcpy(&messageBuffer, recvOutPtr, bytesToTransfer);
				
			}
			messageLenCurrent = bytesToTransfer;
			bytesInRecvBuffer -= bytesToTransfer;									//are there bytes leftover in recvBuffer?
			
			// MESSAGE COMPLETE.
			if(messageLenCurrent == messageLenMax)
			{
				
				// PAYLOAD
				uint8_t *tempPayloadPtr = msgHead + GP_MSG_HEADER_LEN;
				newMessage.setPayload(tempPayloadPtr, newMessage._payloadSize);				//payload to vector
				
				// Create User Object; Parse Message
				// Dynamic allocate new user depending on type of login message received...
				// ...if thread sigpipes in this infinite loop, will this memory get released?
				
				if(user == nullptr){
					if(newMessage._message_type == GP_MSG_TYPE_ASSET_LOGIN){
						user = new GpAssetUser();
						user->_fd = fd;	//assign socket file descriptor received in function parameter
					}
					else if(newMessage._message_type == GP_MSG_TYPE_CONTROLLER_LOGIN){
						user = new GpControllerUser();
						user->_fd = fd;	//assign socket file descriptor received in function parameter
					}
				}

				
				if(user!= nullptr)								// Ignore inbound messages if not logged in first. Only message getting through is login request
					handle_message(newMessage, *user);			// Only handle the message if the user has been created (aka is logged in)
				
				
				
				// Reset the message buffer.

				recvOutPtr+= bytesToTransfer;
				messageStarted = false;
				messageLenCurrent = 0;
				messageLenMax = 0;
				newMessage.clear();
			}
			
		}	// while(bytesInRecvBuffer > 0)
		
		recvInPtr = recvHead;			// reset both buffer pointers to head if it's empty
		recvOutPtr = recvHead;
		
		
	}
	
	// Only reached if recv returns 0 and break in infinite loop occurs.
	
	if(user!=nullptr){
		user->logout();
		delete user;
	}
}




/**
 *  handle_message
 *
 *  Message handler function.
 *
 *  @param   GpMessage & msg, GpUser & user
 */
void GpUavServer::handle_message(GpMessage & msg, GpUser & user){

	
	std::cout << "[" << __func__ << "] "  << "Received message type: " << int(msg._message_type) << " , payload size: " << msg._payloadSize << " , and time: " << uint32_t(msg._timestamp) << std::endl;
	
	if(user._isAuthenticated){
		
		switch (msg._message_type) {
			
			case GP_MSG_TYPE_COMMAND:
			{
				std::cout << "[" << __func__ << "] "  << "GP_MSG_TYPE_COMMAND from " << user._username << " on socket: " << user._fd  << std::endl;

				//GpMavlink::printMavFromGpMessage(msg);
				
				//If command is inbound from a controller, find the asset it is connected to and forward.
				
				if(typeid(user).name() == typeid(GpControllerUser).name()){
					
					GpControllerUser &controller = (dynamic_cast<GpControllerUser&>(user));
					if(controller._isConnectedToPartner){
						std::cout << "[" << __func__ << "] "  << "Forwarding GP_MSG_TYPE_COMMAND to: " << controller._asset._username << " on socket: " << controller._asset._fd << std::endl;
						
						_send_message(msg, controller._asset);
						
					}
					else{

						std::cout << "[" << __func__ << "] "  << "Cannot forward GP_MSG_TYPE_COMMAND to: " << controller._asset._username << ". Asset offline. (socket: " << controller._asset._fd << ")" << std::endl;
						
						
						// refresh based on asset ID
						// re-request that asset to hook it back up to this controller
						
						
						// see if we can reconnect
						controller.requestConnectionToAsset(controller._assetLastRequested);
						

						if(controller._isConnectedToPartner)
							_send_message(msg, controller._asset);
					}

				}
				break;
				
			}
			case GP_MSG_TYPE_PONG:
			case GP_MSG_TYPE_PING:
			{
				if(user._isConnectedToPartner){
					
					if(user._user_type == GpUser::GP_USER_TYPE_CONTROLLER && dynamic_cast<GpControllerUser &>(user)._asset._isConnectedToPartner)
					{
						// Send to controller's asset
						_send_message(msg, dynamic_cast<GpControllerUser &>(user)._asset);
					}
					else if(user._user_type == GpUser::GP_USER_TYPE_ASSET && dynamic_cast<GpAssetUser &>(user)._connected_owner->_isConnectedToPartner)
					{
						// Send to asset's owner/controller.
						_send_message(msg, *(dynamic_cast<GpAssetUser &>(user)._connected_owner));
						
					}
				}
				
				
				
			}
				
				break;
				
			case GP_MSG_TYPE_LOGOUT:
			{
				
				//GpDatabase::logout();
				//user.isAuthenticated set to false in db logout
				std::cout << "[" << __func__ << "] "  << "Processing logout message" << std::endl;
				
				user.logout();
				
				break;
			}
			case GP_MSG_TYPE_HEARTBEAT:
			{
				
				//GpDatabase::logout();
				//user.isAuthenticated set to false in db logout
				std::cout << "[" << __func__ << "] "  << "GP_MSG_TYPE_HEARTBEAT from " << user._username << " on socket " << user._fd << std::endl;
				
				// If message is inbound from an asset, forward to its connected controller.
				
				if(typeid(user).name() == typeid(GpAssetUser).name()){

					// If asset isn't connected t a controller, keep polling for a connect. Update version of asset in
					// the database if connected, stop polling for connection and start forwarding heartbeats to controller.
					GpAssetUser & asset =  dynamic_cast<GpAssetUser &>(user);
					if((asset._isConnectedToPartner == false) || (asset._connected_owner ==nullptr)){

						// POLL: when a controller requests an asset
						
						asset.refresh();
					}
					

					if(asset._isConnectedToPartner){
						
						std::cout << "[" << __func__ << "] "  << "Forwarding GP_MSG_TYPE_HEARTBEAT to " << asset._connected_owner->_username << " on socket " << asset._connected_owner->_fd << std::endl;

						if(_send_message(msg, *(asset._connected_owner)) == false){

							
							// Controller might be offline. That status would have been updated by user logout() and database would now reflect
							// asset change in status (_isConnectedToPartner) but this asset user wouldn't know cause they're not shared memory.
							// So update yourself, asset.
							
							asset.refresh();
						};
					}
				}
				break;
			}
			default:
			{
				std::cout << "[" << __func__ << "] "  << "Message type: "<< int(msg._message_type) << std::endl;
				break;
			}
		}
	}
	else
	{
		// If you're not authenticated, the only message you can even send is an authentication request.
		switch (msg._message_type) {
				
			case GP_MSG_TYPE_CONTROLLER_LOGIN:
			{
				std::cout << "[" << __func__ << "] "  << "CONTROLLER login request from " << user._username << std::endl;
				GpMessage_Login loginMsg(msg._payLd_vec);
				
				// AUTHENTICATE
				if(true == (user.authenticate(loginMsg.username(), loginMsg.key())   ))
				{
					std::cout << "[" << __func__ << "] "  << "Sending authentication confirmation to: " << user._username << " on socket " << user._fd << std::endl;
					
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
 *  Send serialized message to the controller over TCP. Thread guarded.
 *
 *  @param GpMessage & msg
 *  @returns Bool
 */
bool GpUavServer::_send_message(GpMessage & msg, GpUser & user){
	
	std::lock_guard<std::mutex> sendGuard(_send_mutex);		// when guard class is destroyed at end of function the lock is released
	
	// SERIALIZE
	
	std::vector<uint8_t> byteVect;
	byteVect.reserve(msg.size());
	msg.serialize(byteVect);
	
	// SEND TCP
	
	if(user._isOnline){
		
		ssize_t retVal;
#ifdef GP_OSX
		retVal = send(user._fd, byteVect.data(), byteVect.size(), 0);				//	Apple version
#else
#ifdef GP_LINUX
		retVal = send(user._fd, byteVect.data(), byteVect.size(), MSG_NOSIGNAL);	//  Prevent SIGPIPE on Linux
#endif
#endif
		if(retVal == -1){
			perror((boost::format("[%s] Error %d:")  % __func__ % errno).str().c_str() );
			
			
			// Socket error
			
			if(errno == EPIPE){
				
				// client disconnected; clean up. Mark client disconnected. Remove from controller list.
				std::cout << "[" << __func__ << "] "  <<  user._username << " disconnected." << std::endl;

				// Log out if the socket disconnected.
				if(user._isOnline)
					user.logout();
				
			}
			return false;
		}
	}
	else{
		std::cout << "[" << __func__ << "] "  <<  user._username << " no longer online: " << std::endl;
		return false;
	}
	return true;
}



/**
 *  SendLoginConfirmationMessageTo: user
 *
 *  @param GpUser & user
 *
 */
void GpUavServer::sendLoginConfirmationMessageTo(GpUser & user){
	
	// Send login complete message to client
	uint8_t *payload = nullptr;	// No payload required, just a ref to a pointer needed for constructor.
	GpMessage msgLoginComplete(GP_MSG_TYPE_AUTHENTICATED_BY_SERVER, 0, payload);
	
	if(_send_message(msgLoginComplete, user) == false){
		std::cout << "[" << __func__ << "] "  << "Error sending login confirmation" << std::endl;
	}else{
		std::cout << "[" << __func__ << "] "  << "Authentication confirmation sent." << std::endl;
	}
}




/**
 *  putHeaderInMessage (private)
 *
 */
void GpUavServer::putHeaderInMessage(uint8_t *&buffer, long size, GpMessage & message){
	
	// Message_Type (1 byte)
	message._message_type = *buffer; //GP_MSG_TYPE_CONTROLLER_LOGIN;
	
	// Payload Size (2)
	uint8_t *sizePtr = buffer + 1;
	uint16_t pSize = 0;
	GpMessage::byteUnpack16(sizePtr, pSize);
	message._payloadSize = pSize;			//GP_MSG_LOGIN_LEN;
	buffer+= (sizeof(uint16_t));
	
	
	// Timestamp (4)
	sizePtr = buffer;
	uint32_t timestamp = 0;
	GpMessage::byteUnpack32(sizePtr, timestamp);
	message._timestamp = timestamp;
	buffer+= (sizeof(uint32_t));
	
	
	
	
	
	
	
	
	
	
	return;
}

