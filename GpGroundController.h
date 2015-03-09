// ********************************************************************************
//
//  GpUavAsset.h
//  UavServer
//  2/25/15
//
// ********************************************************************************


#ifndef __UavServer__GpGroundController__
#define __UavServer__GpGroundController__

#include "GpClientNet.h"

class GpMessage;

class GpGroundController
{
public:
	bool start();
	int controllerSend();
	
//	void listen();
	
	
private:
	
	bool _isLoggedIn = false;
	
	/**
	 *  signalHandler(): Clean up zombie processes.
	 *
	 */
	static void signalHandler(int signal);

	// Type GpClientNet::gp_message_hander
	static bool handle_messages(GpMessage & msg, GpClientNet & net);

	GpClientNet _net;
	
};

#endif /* defined(__UavServer__GpUavControl__) */
