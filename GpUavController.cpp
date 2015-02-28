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

#include "GpUavController.h"

//#include "GpMavlink.h"
//#include "GpIpAddress.h"
//#include "GpNetworkTransmitter.h"
#include "GpGameController.h"






bool GpUavController::start(){
	
	std::cout << "GpUavController::start()" << std::endl;
	
	// 1. Start up game controller.
	int f;
	if((f=fork()) > 0){
	
		// Game controller process
		
		GpGameController gameController;
		gameController.runGameController();
	
	}
	
	// 2. Connect() with server
	
	
	// Loop:
	
	
	// 3. Receive events from game controller
	
	// 4. Encapsulate events in Mavlink message format
	
	// 5. Send packet over network.
	
	
	
	
	return true;
}













