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
	
	
	
	
	
	// 0. Connect to Server
	
	GpControllerNetwork network;
	bool connectResult = network.gpConnect(GP_CONTROLLER_SERVER_IP, GP_CONTROLLER_SERVER_PORT);
	if(connectResult == false){
		std::cout<< "connect fail" << std::endl;
		return false;
	}
	
	
	// Do Login

	
	network.gpAuthenticateUser("myUsername up to 64 bytes", "My really long password key 2048 byte hash");
	
	
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











