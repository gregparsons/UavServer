// ********************************************************************************
//
//  GpUavAsset.h
//  UavServer
//
// ********************************************************************************


#ifndef __UavServer__GpUavServer__
#define __UavServer__GpUavServer__

#include <thread>
#include <mutex>


class GpMessage;
class GpUser;





class GpUavServer
{
	
public:
	
	bool start();
	
	~GpUavServer();
	
	
private:

	bool _send_message(GpMessage & msg, GpUser & user);
	
	void putHeaderInMessage(uint8_t *&buffer, long size, GpMessage & message);

	void handle_message(GpMessage & msg, GpUser & user);
	
	void sendHeartbeatTo(GpUser user);

	bool startNetwork();

	void client_listener_thread(int fd);
	
	void sendLoginConfirmationMessageTo(GpUser & user);

	int _listen_fd = 0;		// socket used to listen for new clients
	
	
	// outbound message send() mutex
	std::mutex _send_mutex;

	
};



#endif /* defined(__UavServer__GpUavServer__) */

