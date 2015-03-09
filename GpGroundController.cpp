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
#include "GpControllerNetwork.h"
#include "GpMessage_Login.h"
#include "GpMessage.h"
#include "GpClientNet.h"


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
	

	
	// START LISTENER THREAD (for messages from server)
	// Do we need to be listening on a separate thread? How about just listen after sending credentials? Do one thing at a time?
	// network.startListenerThread();
	
	
	// typedef bool (*gp_message_handler)(GpMessage & message);
	// _net.startListenerAsThread(GpClientNet::gp_message_handler);
	
	
	_net.startListenerAsThread(GpGroundController::handle_messages);
	
	
	
	
	// LOGIN
	// Listener thread gets replies, parses authentication confirmation message, then initiates sending
	// controller commands.
	_net.sendAuthenticationRequest(GP_CONTROLLER_TEST_USERNAME, GP_CONTROLLER_TEST_PASSWORD);
	// network.sendAuthenticationRequest(GP_CONTROLLER_TEST_USERNAME, GP_CONTROLLER_TEST_PASSWORD);
	
	// Now Listen and block on listen (instead of running this as its own thread).
	// network.listenThread();		//will this start fast enough to catch authentication reply?
	
	

	
	
	
	
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
	std::cout << "[" << __func__ << "] "  <<  "GpControllerNetwork called me." << std::endl;
	
	
	switch (msg._message_type) {
		case GP_MSG_TYPE_AUTHENTICATED_BY_SERVER:
		{
			// START SENDING MESSAGES
			std::cout << "[" << __func__ << "] Client received GP_MSG_TYPE_AUTHENTICATED_BY_SERVER: setting _shouldSendControllerOutput = true" << std::endl;
			
			
			// start sending asset commands
			
			
			GpGameController controller;
			
			
			
			
			
			// TO DO: remove network from Game Controller
			controller.runGameController(net);
			
			
			
			
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





