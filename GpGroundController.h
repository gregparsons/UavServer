// ********************************************************************************
//
//  GpUavAsset.h
//  UavServer
//  2/25/15
//
// ********************************************************************************


#ifndef __UavServer__GpGroundController__
#define __UavServer__GpGroundController__

class GpGroundController
{
public:
	bool start();
	int controllerSend();
private:
	
	bool _isLoggedIn = false;
	
	
	/**
	 *  signalHandler(): Clean up zombie processes.
	 *
	 */
	static void signalHandler(int signal);
};

#endif /* defined(__UavServer__GpUavControl__) */
