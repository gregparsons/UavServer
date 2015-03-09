// ********************************************************************************
//
//  GpUavAsset.h
//  UavServer
//  2/25/15
//
// ********************************************************************************
/*
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>				//INET6_ADDRSTRLEN
#include <netdb.h>
#include <arpa/inet.h>
#include <signal.h>
#include <iostream>
#include <mavlink/c_library/common/mavlink.h>
*/
#include <unistd.h>					//usleep, fork
#include <iostream>
#include <signal.h>
#include <thread>
#include <sys/wait.h>


#include "GpGroundController.h"
#include "GpGameController.h"
#include "GpIpAddress.h"
// #include "GpControllerNetwork.h"
#include "GpMessage_Login.h"
#include "GpMessage.h"
#include "GpClientNet.h"



// Statics
GpGameController GpGroundController::_game_controller;





bool GpGroundController::start(){
	
	std::cout << "[" << __func__ << "] "  <<  "" << std::endl;
	/*
	// Handle zombie processes
	
	struct sigaction signalAction;
	signalAction.sa_handler = GpGroundController::signalHandler;
	sigemptyset(&signalAction.sa_mask);
	signalAction.sa_flags = SA_RESTART;
	int result = sigaction(SIGCHLD, &signalAction, nullptr);
	if(result == -1){
		std::cout << "Error: sigaction" << std::endl;
		exit(1);
	}
	*/
	
	
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

	
	
	while(1);

	return true;
}



/**
 *  signalHandler(): Clean up zombie processes.
 *
 */

void
GpGroundController::signalHandler(int signal)
{
	while(waitpid(-1, NULL, WNOHANG) > 0);
	std::cout << "signalHandler()" << std::endl;
}




bool GpGroundController::handle_messages(GpMessage & msg, GpClientNet & net){
	std::cout << "[" << __func__ << "] "  <<  "GpGroundController funct* called in GpClientNet.cpp" << std::endl;
	
	
	switch (msg._message_type) {
		case GP_MSG_TYPE_AUTHENTICATED_BY_SERVER:
		{
			// START SENDING MESSAGES
			std::cout << "[" << __func__ << "] Client received GP_MSG_TYPE_AUTHENTICATED_BY_SERVER: setting _shouldSendControllerOutput = true" << std::endl;
			
			

			
			
			// start sending asset commands
			
			// GpGameController controller;

			
			_game_controller.startGameControllerThread(net);

			
			// _game_controller.runGameController(net);
			
			/*******
			
			 
			 
			 This needs to start as a thread. Otherwise message processing stops.
			
			 
			 
			 
			 *******/
			
			
			
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
		case GP_MSG_TYPE_CONTROLLER_LOGIN:
		case GP_MSG_TYPE_LOGOUT:
		default:
			break;
	}
	
	return true;
}





