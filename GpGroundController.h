// ********************************************************************************
//
//  GpUavAsset.h
//  UavServer
//
//	Class implementing the ground controller. Starts a network connection to the
//	server, attempts to login, then connects with game controller and begins
//	sending game controller events to server for forwarding to the requested asset.
//
// ********************************************************************************


#ifndef __UavServer__GpGroundController__
#define __UavServer__GpGroundController__

#include "GpGameController.h"
#include "GpClientNet.h"

class GpMessage;

class GpGroundController
{


public:

	
	bool start();
	
	int controllerSend();
	
	
private:
	
	bool _isLoggedIn = false;
	
	static bool handle_messages(GpMessage & msg, GpClientNet & net);
	
	
	
	
	GpClientNet _net;
	static GpGameController _game_controller;
	
};

#endif /* defined(__UavServer__GpUavControl__) */
