// ********************************************************************************
//
//  GpUavAsset.h
//  UavServer
//  2/25/15
//
// ********************************************************************************


#define MAX_CONNECTION_BACKLOG 10

#include <iostream>
#include <bitset>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>	//
#include <signal.h>
#include <unistd.h>		//fork()
#include <mavlink/c_library/common/mavlink.h>

#include "GpUavServer.h"
#include "GpIpAddress.h"
#include "GpMavlink.h"

using namespace std;

/**
 *  signalHandler(): Clean up zombie processes.
 *
 */
void
signalHandler(int signal)
{
	while(waitpid(-1, NULL, WNOHANG) > 0);
	cout << "signalHandler()" << endl;
}


bool
GpUavServer::start(){

	std::cout << "Server started" <<std::endl;


	
	// TCP Socket Setup
	
	

	int listen_fd = -1, client_fd = -1;
	struct addrinfo hintAddrInfo, *resSave = nullptr, *res = nullptr;
	struct sockaddr_storage inboundAddress;
	socklen_t addrLen = 0;
	struct sigaction signalAction;
	int result = 0;
	int yes = 1;
	
	memset(&hintAddrInfo, 0, sizeof(addrinfo));
	hintAddrInfo.ai_family = AF_UNSPEC;		//any protocol family
	hintAddrInfo.ai_socktype = SOCK_STREAM;	//stream = TCP
	hintAddrInfo.ai_flags = AI_PASSIVE;		//
	
	result = getaddrinfo(GP_CONTROLLER_SERVER_IP.c_str(), GP_CONTROLLER_SERVER_PORT.c_str(), &hintAddrInfo, &resSave);
	if(result < 0){ //error
		cout << "getaddrinfo() error" << endl;
		return 1;
	}
	


	// Socket
	

	for(res = resSave; res != nullptr; res = res->ai_next){
	
		listen_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if(listen_fd == -1){ //error, else file descriptor
			cout << "Socket error" << endl;
			continue;
		}
		result = setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
		if(result == -1){
			cout << "socket options error" << endl;
			exit(1);
		}
		
		
		// Bind
		
		result = ::bind(listen_fd, res->ai_addr, res->ai_addrlen);		
		if(result == -1){
			cout << "Bind Error" << endl;
			exit(1);
		}
		break;
		
	}
	
	// Either found a good socket or there are none
	
	if(res == nullptr){
		cout << "Failed to bind" << endl;
		return 2;
	}
	freeaddrinfo(resSave);	//done with this
	
	
	
	// Listen
	
	result = listen
	(listen_fd, MAX_CONNECTION_BACKLOG);
	if(result == -1){
		cout << "Listen error" << endl;
		exit(1);
	}
	
	
	
	// Handle zombie processes
	
	signalAction.sa_handler = signalHandler;
	sigemptyset(&signalAction.sa_mask);
	signalAction.sa_flags = SA_RESTART;
	result = sigaction(SIGCHLD, &signalAction, nullptr);
	if(result == -1){
		cout << "Error: sigaction" << endl;
		exit(1);
	}
	
	
	
	
	// Listener Loop
	
	
	
	cout << "Server: waiting for connections" << endl;
	for(;;){
		

		// Accept
		
		
		addrLen = sizeof(inboundAddress);		//address len for accept should be size of address struct, but on return it gets set to bytes of the actual string (or struct?) address
		client_fd = accept(listen_fd, (struct sockaddr *)&inboundAddress, &addrLen);
		
		if(client_fd == -1){
			cout << "Error: accept(): " << errno << endl;
			switch (errno) {
				case EBADF:		//errno.h
					break;
				default:
					break;
			}
			continue;
		}
		
		
		
		// Fork
		
		
		result = fork();		//returns child PID to parent and 0 to child, error = -1
		if(result == 0){
			
			
			
			// Child / recv()
			
			
			close(listen_fd);
			size_t length = MAVLINK_MSG_ID_RC_CHANNELS_OVERRIDE_LEN;
			uint8_t buffer[length];
			
			for(;;){
			
				size_t numBytes = recv(client_fd, &buffer, length, 0);		//blocks if no data, returns zero if no data and shutdown occurred
				cout << "numBytes: " << numBytes << endl;

				if(numBytes == -1){
					cout << "Error: recv()" << endl;
					exit(1);
				}
				else if(numBytes == 0){
					//shutdown
					exit(0);

				
				}
				
				
				//Do something with received bytes;
				
				
				//GpMavlink::printMavMessage(const mavlink_message_t &msg);
				uint8_t *buf = buffer;
				GpMavlink::decodeMavlinkBytesToControlEvent(buf, numBytes);
				
				
				
			}
			
			
			
			// Close child process
			close(client_fd);
			exit(0);
			
		}
	}	// for(;;) Parent listen_fd loop.

	
	
	
	// close(listen_fd);

	
	return true;
}

