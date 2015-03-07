// ********************************************************************************
//
//  GpUavAsset.h
//  UavServer
//  2/25/15
//
// ********************************************************************************


#include <iostream>
#include <signal.h>

#include "GpUavServer.h"
#include "GpGroundController.h"
#include "GpAsset.h"

void printUsage();
void exit_program(int signal);


// ***************************************************************************
//		main()
// ***************************************************************************
int main(int argc, const char * argv[]) {

	// Respond to cntl-c
	signal(SIGINT, &exit_program);
	
	// server, control, or fly
	
	if(argc<=1){
		printUsage();
	}
	else{
		std::string arg1 = argv[1];
		
		if(arg1 == "control")
		{
			std::cout << "Starting game controller output." << std::endl;
			GpGroundController controller;
			controller.start();
		
		}
		else if(arg1 == "fly")
		{
			std::cout << "[" << __func__ << "] Starting flight controller receiver." << std::endl;
			GpAsset asset;
			if(asset.connectServer() == false)
				return -1;
		
		}
		else if(arg1 == "server"){
			GpUavServer server;
			server.start();
		}
	}
	return 0;
}

void
printUsage(){
	std::cout << "Usage: [control], [fly], [server]" << std::endl;
}

void
exit_program(int signal){
	exit(0);
}