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
		
		
		
		
		
		
		// If ASSET: insert into database (list of active assets)
		
		if(typeid(*this).name() == typeid(GpAssetUser).name()){

			
			_user_id = GP_ASSET_ID_TEST_ONLY ; 
			
			
			
			if(GpDatabase::insertAsset(dynamic_cast<GpAssetUser &>(*this)) == true)
				 (dynamic_cast<GpAssetUser &>(*this))._connected = true;
		
		}
		else if(typeid(*this).name() == typeid(GpAssetUser).name()){
		
			// If CONTROLLER: check if the asset is active:
			std::cout << "[" << __func__ << "] This is a controller, requesting asset: " << GP_ASSET_ID_TEST_ONLY << std::endl;
			
			
			// ALERT HARD CODED TEST!!
			
			
			// Request a connection with an asset
			bool assetReqResult = (dynamic_cast<GpControllerUser &>(*this)).requestConnectionToAsset(GP_ASSET_ID_TEST_ONLY);
			// GpDatabase::authenticateUserForAsset((dynamic_cast<GpControllerUser &>(*this)), GP_ASSET_ID_TEST_ONLY);
		
			if(assetReqResult){
				std::cout << "[" << __func__ << "] Asset request approved." << std::endl;
			}
			else{
				std::cout << "[" << __func__ << "] Asset request failed. Need code here to poll until it's available...or what if it will never be available?" << std::endl;
			}
		}
		
		
		
		
		
		
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
	
	
	return GpDatabase::authenticateUserForAsset(*this, assetId);
	
	
}