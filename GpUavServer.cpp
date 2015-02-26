// ********************************************************************************
//
//  GpUavAsset.h
//  UavServer
//  2/25/15
//
// ********************************************************************************


#include <iostream>
#include "GpUavServer.h"
#include "GpMavlink.h"
#include "GpNetworkTransmitter.h"


#include <bitset>


// #define MAVLINK_USE_CONVENIENCE_FUNCTIONS
#include <mavlink/c_library/common/mavlink.h>




// Startmes()

bool GpUavServer::start(){
	

	std::cout << "Server started" <<std::endl;
	
	GpMavlink gpMavlink;
	GpNetworkTransmitter net;
	net.listenOnNetwork();
/*
		mavlink_message_t message;
		gpMavlink.receiveTestMessage(message);
		
*/	
	return true;
}

