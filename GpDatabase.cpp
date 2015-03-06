// ********************************************************************************
//
//  GpDatabase.cpp
//  UavServer
//  3/1/15
//
// ********************************************************************************

#include <iostream>

#include "GpDatabase.h"
#include "GpIpAddress.h"

bool GpDatabase::authenticateUser(std::string username, std::string key){
	
	std::cout << "[" << __func__ << "] "  << "Does nothing. All users return true." <<std::endl;

	std::string test1 = GP_CONTROLLER_TEST_USERNAME;
	std::string test2 = username;
	
	if(test1 == test2)
		;
	
	if(username.compare(GP_CONTROLLER_SERVER_IP) && key.compare(GP_CONTROLLER_TEST_PASSWORD)){
		std::cout << "[" << __func__ << "] "  << "User: " << username << " authenticated." << std::endl;

		
		
		return true;
	}else{
		std::cout << "[" << __func__ << "] "  << "User: " << username << " not authenticated." << std::endl;

		
		
		return true;

	}



}
