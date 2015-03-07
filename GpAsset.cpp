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

GpAsset::GpAsset(){}


bool GpAsset::connectServer(){
	
	GpClientNet net;
	//if(net.connectToServer(GP_CONTROLLER_SERVER_IP, GP_ASSET_SERVER_PORT) == false){
	
	
	
	// CHANGE TO ASSET PORT!
	
	if(net.connectToServer(GP_CONTROLLER_SERVER_IP, GP_CONTROLLER_SERVER_PORT) == false){
	
		std::cout << "[" << __func__ << "] "  << "No server at ip/port?" << std::endl;
	
		
	
		return false;
	}
	
	net.startListenerAsThread(GpAsset::handle_messages);
	
	
	return true;
	
}


/**
 *  Handle Messages
 *
 *  Passed as a function pointer to GpClientNet::startListenerAsThread to handle GpMessages when they arrive over TCP.
 *
 *  @param GpMessage & message
 *  @returns bool success
 */
bool GpAsset::handle_messages(GpMessage & message){
	
	std::cout << "[" << __func__ << "] "  << "Message Type: " << int(message._message_type) << std::endl;

	return true;
}