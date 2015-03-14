// ********************************************************************************
//
//  GpAsset.cpp
//  UavServer
//  3/7/15
//
// ********************************************************************************

#include <iostream>

#include "GpAsset.h"
#include "GpClientNet.h"
#include "GpIpAddress.h"
#include "GpMessage.h"
#include "GpMavlink.h"




GpAsset::GpAsset(){}


bool GpAsset::connectServer(){

	
	for(;;){
	
		GpClientNet net(GpAsset::handle_messages, false); // GP_INSTRUMENTATION_ON);
		
		
		if(net.connectToServer(GP_CONTROLLER_SERVER_IP, GP_CONTROLLER_SERVER_PORT) == false){
			std::cout << "[" << __func__ << "] "  << "No server at ip/port?" << std::endl;
			return false;
		}
		
		net.startListenerAsThread();

		net.sendAuthenticationRequest(GP_ASSET_TEST_USERNAME, GP_ASSET_TEST_PASSWORD, GP_MSG_TYPE_ASSET_LOGIN);
		
		
		
		// need to block doing something
		net.sendHeartbeat();
	
	}
	
	

	
	return true;
	
}


/**
 *  Handle Messages
 *
 *  Passed as a function pointer to GpClientNet::startListenerAsThread to handle GpMessages when they arrive over TCP. Have
 to pass pointer to net because this function is static and can't use instance member variables. Net just passes *this.
 *
 *  @param GpMessage & message
 *  @returns bool success
 */
bool GpAsset::handle_messages(GpMessage & message, GpClientNet & net){

	switch (message._message_type) {
		case GP_MSG_TYPE_AUTHENTICATED_BY_SERVER:
		{

			std::cout << "[" << __func__ << "] Asset received GP_MSG_TYPE_AUTHENTICATED_BY_SERVER" << std::endl;
			
			// START SENDING HEARTBEAT TO SERVER FROM ASSET HERE
			
			
			//net.startBackgroundHeartbeat();
			
			
			
			break;
		}
		case GP_MSG_TYPE_HEARTBEAT:
		{
			std::cout << "[" << __func__ << "] Asset received GP_MSG_TYPE_HEARTBEAT" << std::endl;
			
			break;
		}
		case GP_MSG_TYPE_COMMAND:
		{
			std::cout << "[" << __func__ << "] Asset received GP_MSG_TYPE_COMMAND" << std::endl;
			
			
			GpMavlink::printMavFromGpMessage(message);
			
			/*
			 
			 
			 
			 Here, send message to ASSET hardware
			 
			 
			 
			 */
			
			break;
			
		}
			
			
			//all the rest fall through
		default:
			break;
	}
	
	
	
	
	
	
	
	
	
	// std::cout << "[" << __func__ << "] "  << "Message Type: " << int(message._message_type) << std::endl;

	return true;
}





