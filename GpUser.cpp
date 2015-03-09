// ********************************************************************************
//
//  GpUser.cpp
//  UavServer
//
// ********************************************************************************

#include <iostream>


#include "GpUser.h"
#include "GpDatabase.h"
#include "GpIpAddress.h"


GpUser::GpUser(){

};

GpUser::~GpUser(){};

bool GpUser::authenticate(std::string username, std::string key){
	_username = username;
	
	if(GpDatabase::authenticateUser(username, key) == true){
		_isAuthenticated = true;
		
		
		
		_user_id = rand();		//this would be the unique key returned from the database
		
		
		
		
		
	}
	else{
		_isAuthenticated=false;
	}
	
	
	return _isAuthenticated;
	
}


GpAssetUser::GpAssetUser(){

};



GpControllerUser::GpControllerUser(){

};



bool
GpControllerUser::requestConnectionToAsset(int assetId){
	
	// Check database. Does this assetId exist?
	// Check permissions. Is this user allowed to use this asset? aka, does user "own" asset?
	
	
	
	
	std::cout << "[" << __func__ << "]" << std::endl;
	
	
	return GpDatabase::authenticateUserForAsset(this, assetId);
	
	
}