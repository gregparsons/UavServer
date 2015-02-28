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
#include "GpNetworkTransmitter.h"

using namespace std;

void signalHandler(int signal)
{
	while(waitpid(-1, NULL, WNOHANG) > 0);
}


bool GpUavServer::start(){

	std::cout << "Server started" <<std::endl;
	
//	GpMavlink gpMavlink;
//	GpNetworkTransmitter net;
//	net.listen();
/*
		mavlink_message_t message;
		gpMavlink.receiveTestMessage(message);
		
*/


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
	//hintAddrInfo.ai_protocol = IPPROTO_TCP;
	hintAddrInfo.ai_flags = AI_PASSIVE;		//
	
	
	
	
	// Get network interface IP addresses
	
	result = getaddrinfo(GP_FLY_IP_ADDRESS, GP_CONTROL_PORT, &hintAddrInfo, &resSave);
	if(result < 0){ //error
		cout << "getaddrinfo() error" << endl;
		return 1;
	}
	

	for(res = resSave; res != nullptr; res = res->ai_next){

		
		// Socket
		
		
		
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
	
	
	
	
	
	signalAction.sa_handler = signalHandler;	//"reap dead processes"
	sigemptyset(&signalAction.sa_mask);
	signalAction.sa_flags = SA_RESTART;
	result = sigaction(SIGCHLD, &signalAction, nullptr);
	if(result == -1){
		
		cout << "Error: sigaction" << endl;
		exit(1);
		
	}
	
	cout << "Server: waiting for connections" << endl;
	
	
	// Main Loop
	
	for(;;){
		
		
		

		// Accept
		
		
		
		
		addrLen = sizeof(inboundAddress);		//address len for accept should be size of address struct, but on return it gets set to bytes of the actual string (or struct?) address
		client_fd = accept(listen_fd, (struct sockaddr *)&inboundAddress, &addrLen);
		
		/*
		 // Print IP address
		char s[INET6_ADDRSTRLEN];
		inet_ntop(resSave->ai_family, get_in_addr((struct sockaddr *)&inboundAddress), s, inboundAddress.ss_len);
		cout << "Address: " << s << endl;
*/
		 
		 
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
		
		// good FD
		
		result = fork();		//returns child PID to parent and 0 to child, error = -1
		if(result == 0){
			
			
			// CHILD PROCESS
			// Keep client connection open. HOW TO CLOSE?
			
			
			
			//cout << "Child process spawned" << endl;
			
			
			close(listen_fd);	//Close copy of parent's listener socket.
			size_t length = 18;
			char buffer[length];
			
			for(;;){
			
				size_t numBytes = recv(client_fd, &buffer, length, 0);
				if(numBytes == -1){
					cout << "Error: recv()" << endl;
					exit(1);
				}
				cout << "numBytes: " << numBytes << endl;
				
				
				
				
				
				
				
				
				
			}
			
			
			
			// Close child process
			close(client_fd);
			exit(0);
			
		}
	}	// for(;;) Parent listen_fd loop.

	
	
	
	// close(listen_fd);

	
	return true;
}

