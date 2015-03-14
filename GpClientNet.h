// ********************************************************************************
//
//  GpClientNet.h
//  UavServer
//
// ********************************************************************************


#ifndef __UavServer__GpClientNet__
#define __UavServer__GpClientNet__

#include <sys/socket.h>
#include <vector>
#include <mutex>
#include <thread>
#include <fstream>


class GpMessage;		// forward declare

class GpClientNet{
	

	

	
	
public:

	
	typedef bool (*gp_message_handler)(GpMessage & message, GpClientNet & net);
	
	GpClientNet();
	GpClientNet(gp_message_handler message_handler, bool loggingOn);
	
	
	~GpClientNet();
	

	void turnOnLogging(bool shouldTurnOnLogging);
	
	
	bool connectToServer(std::string ip, std::string port); // to server
	
	/**
	 *  Send
	 *
	 *  Send a message. Thread safe. Message writes are atomic.
	 *
	 *  @param GpMessage & message
	 *  @returns bool result
	 */
	bool sendMessage(GpMessage & message);
	
	/**
	 *  Start Listener Thread
	 *
	 *  Starts _listen_for_TCP_messages() as a dispatched thread. Sets the _message_handler function pointer
	 *	to handle GpMessages generated. Hopefully that's thread safe....
	 *
	 *
	 *  @param GpClientNet::gp_message_handler
	 *  @returns void
	 */
	bool startListenerAsThread();
	void startListenerAsThread(GpClientNet::gp_message_handler);
	// void startListenerAsThread(GpClientNet::gp_message_handler, std::thread * listenThread);

	
	
	
	/**
	 *  sendAuthenticationRequest
	 *
	 *  Sends a GP_MSG_...LOGIN_MESSAGE
	 *
	 *  @param std::string username, std::string key
	 *  @returns void
	 */
	void sendAuthenticationRequest(std::string username, std::string key, int gp_msg_source_type);

	
	

	
	void startBackgroundHeartbeat();
	void sendHeartbeat();

	
	bool _isConnected = false;
	
	//INSTRUMENTATION_ON	
	void startBackgroundPing();

private:
	int _fd = 0;
	
	
	
	
	std::mutex _send_mutex;
	std::mutex _message_handler_mutex;
	

	
	
	
	
	
	
	// Pointer to the function the calling entity wants to have happen when a new message is received.

	gp_message_handler _message_handler = nullptr;

	/**
	 *  Send a GpMessage over TCP
	 *
	 *  The not thread-safe version. Obtains mutex during send() to prevent other threads sending at the same
	 *  time and garbling TCP bytes. Keeps messages in order.
	 *
	 *  @param GpMessage & message
	 *  @returns bool success
=	 */
	bool _sendMessage(GpMessage & message);

	
	/**
	 *  listen_for_TCP_messages
	 *
	 *  Blocks on recv to get bytes from TCP/server. When enough arrive to comprise
		a GpMessage, the message_handler function given by the calling code
		handles the message.
	 *
	 *  @param bool message_handler(GpMessage & mesg)
	 *  @returns never
	 */
	bool _listen_for_TCP_messages();


	// CRAP from old code
	void _receiveDataAndParseMessage();

	void callMessageHandler(GpMessage & msg);

	//instrumentation_on
	void _sendPing();

	void compareRoundTripTime(GpMessage & msg);

	
	static std::ofstream _metricsFile;

	static void _interruptHandler(int s);
	void _catchSignals();
	static void _closeLogging();

	
};

#endif /* defined(__UavServer__GpClientNet__) */
