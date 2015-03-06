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
	
	std::cout << "NOT IMPLEMENTED: GpDatabase::authenticateUser(). All users return true." << std::endl;


	
	if(username == GP_CONTROLLER_TEST_USERNAME && key == GP_CONTROLLER_TEST_PASSWORD){
		std::cout << "[" << __func__ << "] "  << "User: " << username << " authenticated." << std::endl;

		
		
		return true;
	}else{
		std::cout << "[" << __func__ << "] "  << "User: " << username << "NOT authenticated." << std::endl;

		
		
		return true;

	}



}
