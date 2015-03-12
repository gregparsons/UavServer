// ********************************************************************************
//
//  GpUavAsset.h
//  UavServer
//
// ********************************************************************************


#include <unistd.h>					//usleep
#include <iostream>
#include <signal.h>
#include <thread>
#include <sys/wait.h>

#include "GpGroundController.h"
#include "GpGameController.h"
#include "GpIpAddress.h"
#include "GpMessage_Login.h"
#include "GpMessage.h"
#include "GpClientNet.h"

// Statics
GpGameController GpGroundController::_game_controller;


// START

bool GpGroundController::start(){
	
	std::cout << "[" << __func__ << "] "  <<  "" << std::endl;
	
	for(;;){
	
	
		// CONNECT SOCKET (with server)
		
		bool connectRes = _net.connectToServer(GP_CONTROLLER_SERVER_IP, GP_CONTROLLER_SERVER_PORT);

		if(connectRes == false){
			std::cout << "[" << __func__ << "] "  <<  "connect() fail: " << strerror(errno) << std::endl;
			return false;
		}
		
		// LISTENER THREAD (for messages from server)

		_net.startListenerAsThread(GpGroundController::handle_messages);

		

		// LOGIN
		
		_net.sendAuthenticationRequest(GP_CONTROLLER_TEST_USERNAME, GP_CONTROLLER_TEST_PASSWORD, GP_MSG_TYPE_CONTROLLER_LOGIN);

		
		
		// In lieu of something else, this keeps the main thread going. If this fails to send, loop repeats and attempts to reconnect.
		
		_net.sendHeartbeat();
		
	}
	

	return true;
}





/**
 *  handle_messages
 *
 *  Function pointer to this function is passed to GpClientNet. When a new GpMessage comes in this gets called so net 
 *  knows what to do with it. GpClientNet can thus handle any message for whatever purpose a calling object wants.
 *
 *  @param GpMessage & msg, GpClientNet & net
 *  @returns bool result
 */
bool GpGroundController::handle_messages(GpMessage & msg, GpClientNet & net){
	std::cout << "[" << __func__ << "] "  <<  "GpGroundController funct* called in GpClientNet.cpp" << std::endl;
	
	
	switch (msg._message_type) {
		case GP_MSG_TYPE_AUTHENTICATED_BY_SERVER:
		{
			// START SENDING MESSAGES
			std::cout << "[" << __func__ << "] Client received GP_MSG_TYPE_AUTHENTICATED_BY_SERVER: setting _shouldSendControllerOutput = true" << std::endl;
			
			// start sending asset commands
			
			_game_controller.startGameControllerThread(net);

			
			break;
		}
		case GP_MSG_TYPE_HEARTBEAT:
		{
			std::cout << "[" << __func__ << "] Received GP_MSG_TYPE_HEARTBEAT" << std::endl;
			break;
		}
		case GP_MSG_TYPE_COMMAND:
		{
			std::cout << "[" << __func__ << "] Received GP_MSG_TYPE_COMMAND" << std::endl;
			
			break;
		}
			//all the rest fall through
		case GP_MSG_TYPE_NONE_ZERO:
		case GP_MSG_TYPE_CONTROLLER_LOGIN:
		case GP_MSG_TYPE_LOGOUT:
		default:
			break;
	}
	
	return true;
}





