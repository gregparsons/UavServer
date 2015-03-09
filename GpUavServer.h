// ********************************************************************************
//
//  GpUavAsset.h
//  UavServer
//  2/25/15
//
// ********************************************************************************


#ifndef __UavServer__GpUavServer__
#define __UavServer__GpUavServer__



class GpMessage;
class GpUser;





class GpUavServer
{
	
public:
	
	bool start();
	
	
private:
	void putHeaderInMessage(uint8_t *&buffer, long size, GpMessage & message);
	
	void processMessage(GpMessage & msg, GpUser & user);
	
	bool sendMessageToController(GpMessage & msg, GpUser & user);
	
	void sendHeartbeat(GpUser user);

	bool startNetwork();

	void threadClientRecv(int fd);
	
	void sendLoginConfirmationMessageTo(GpUser & user);

	
	
//	int _client_fd = 0;		// socket for this client (when using fork())
	int _listen_fd = 0;		// socket used to listen for new clients
	
	
//	std::vector<GpAssetConnect> _assets;
//	std::vector<GpControllerConnect> _controllers;
	
	
	
	
};



#endif /* defined(__UavServer__GpUavServer__) */



/*


To do:
 
 --listen on a port
 
 --for each unique connection spawn a process or thread
 --connections will be from either a controller or asset
 
 
 --if controller:
 --authenticate
 --get list of assets controller is allowed to control
 --forward controller packets to assets, if connected


 --if asset:
 --authenticate
 --get list of controllers authorized to control
 --forward packets to connected controllers
 --or, only forward packets to controller previously established, as current "owner"
 

 --controller (client type 1)
 
 
 --uav (client type 2)










*/