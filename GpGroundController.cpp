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


bool GpGroundController::start(){
	
	std::cout << "GpGroundController::start()" << std::endl;
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
	
	GpControllerNetwork network;
	bool connectResult = network.gpConnect(GP_CONTROLLER_SERVER_IP, GP_CONTROLLER_SERVER_PORT);
	if(connectResult == false){
		std::cout<< "[GpGroundController::start] Controller connect fail: " << strerror(errno)<<  std::endl;
		return false;
	}
	
	
	
	// START LISTENER THREAD (for messages from server)
	// Do we need to be listening on a separate thread? How about just listen after sending credentials? Do one thing at a time?
	network.startListenerThread();
	
	
	
	// LOGIN
	// Listener thread gets replies, parses authentication confirmation message, then initiates sending
	// controller commands.
	network.sendAuthenticationRequest(GP_CONTROLLER_TEST_USERNAME, GP_CONTROLLER_TEST_PASSWORD);
	
	// Now Listen and block on listen (instead of running this as its own thread).
	// network.listenThread();		//will this start fast enough to catch authentication reply?
	
	
	while(! network._shouldSendControllerOutput){
		usleep(10000);
	}
	
	GpGameController controller;

	
	
	// TO DO: remove network from Game Controller

	// controller.runGameController(network);
		
		
	
	
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











