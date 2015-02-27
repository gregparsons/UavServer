// ********************************************************************************
//
//  GpUavAsset.h
//  UavServer
//  2/25/15
//
// ********************************************************************************

#define SERVER_PORT "5678"
#define MAX_CONNECTION_BACKLOG 10


#include <iostream>
#include "GpUavServer.h"
//#include "GpMavlink.h"
//#include "GpNetworkTransmitter.h"
#include <bitset>

//Net Includes
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>	//
#include <signal.h>
#include <unistd.h>		//fork()
// #define MAVLINK_USE_CONVENIENCE_FUNCTIONS
#include <mavlink/c_library/common/mavlink.h>

using namespace std;

void signalHandler(int signal)
{
	while(waitpid(-1, NULL, WNOHANG) > 0);
}


// Startmes()

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
	struct addrinfo hintAddrInfo, *servInfo = nullptr, *tempInfo = nullptr;
	struct sockaddr_storage inboundAddress;
	socklen_t socketSize = 0;
	struct sigaction signalAction;
	char ipv6[INET6_ADDRSTRLEN];
	int result = 0;
	int yes = 1;
	
	memset(&hintAddrInfo, 0, sizeof(addrinfo));
	hintAddrInfo.ai_family = AF_UNSPEC;		//any protocol family
	hintAddrInfo.ai_socktype = SOCK_STREAM;	//stream = TCP
	//hintAddrInfo.ai_protocol = IPPROTO_TCP;
	hintAddrInfo.ai_flags = AI_PASSIVE;		//
	
	result = getaddrinfo(nullptr, SERVER_PORT, &hintAddrInfo, &servInfo);
	if(result < 0){ //error
		cout << "getaddrinfo() error" << endl;
		return 1;
	}
	
	//Loop through all local interfaces, returned in servInfo from getaddrinfo
	for(tempInfo = servInfo; tempInfo != nullptr; tempInfo = tempInfo->ai_next){

		// Socket
		
		listen_fd = socket(servInfo->ai_family, servInfo->ai_socktype, tempInfo->ai_protocol);
		if(listen_fd == -1){ //error, else file descriptor
			cout << "Socket error" << endl;
			continue;
		}

		// Socket Options
		
		result = setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
		if(result == -1){
			cout << "socket options error" << endl;
			exit(1);
		}
		
		// Bind
		
		result = ::bind(listen_fd, tempInfo->ai_addr, tempInfo->ai_addrlen);		//std::bind conflicts w/o :: using namespace std
		if(result == -1){
			cout << "Bind Error" << endl;
			exit(1);
		}
		
		break;
		
	}
	
	// Either found a good socket or there are none
	if(tempInfo == nullptr){
		cout << "Failed to bind" << endl;
		return 2;
	}
	freeaddrinfo(servInfo);	//done with this
	
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
		
		socketSize = sizeof(inboundAddress);

		// Accept
		
		client_fd = accept(listen_fd, (struct sockaddr *)&inboundAddress, &socketSize);
		if(client_fd == -1){
			cout << "Error: accept" << endl;
			continue;
		}
		
		// good FD
		
		result = fork();		//returns child PID to parent and 0 to child, error = -1
		if(result == 0){
			
			cout << "Child process spawned" << endl;
			
			//This is child; close copied child listener
			
			close(listen_fd);	//child doesn't need listener socket
			
			
			
			// client_fd is whom we talk to now in the child
			// Do stuff here, read() ?
			
			
			size_t length = 18;
			char buffer[length];
			
			
			size_t numBytes = recv(client_fd, &buffer, length, 0);
			if(numBytes == -1){
				cout << "Error: recv()" << endl;
				exit(1);
			}
			
			cout << "numBytes: " << numBytes << endl;
			
			// Close child
			
			close(client_fd);
			exit(0);			//child exits
			
		}
		close(listen_fd);		//back in parent
		
		
	}
	
	
	
	
	
	
	
	



	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	return true;
}

