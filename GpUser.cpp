// ********************************************************************************
//
//  GpUser.cpp
//  UavServer
//
// ********************************************************************************

#include <iostream>

#include <typeinfo>	//required by Raspbian for typeid

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
		_isOnline = true;

		

		// If ASSET:
		// insert into database (list of active assets)
		if(typeid(*this).name() == typeid(GpAssetUser).name()){

			//TEST
			_user_id = GP_ASSET_ID_TEST_ONLY ;
			
			// Add asset to database if not already there
			if(GpDatabase::insertAsset(dynamic_cast<GpAssetUser &>(*this))==false){
				std::cout << "[" << __func__ << "] Error adding/inserting asset: " << _user_id << std::endl;
				
			}
		
		}
		else if(typeid(*this).name() == typeid(GpControllerUser).name()){
		
			// If CONTROLLER:
			
			// check if asset is active:
			
			std::cout << "[" << __func__ << "] A controller is requesting asset: " << GP_ASSET_ID_TEST_ONLY << std::endl;
			
			//TEST:
			_user_id = rand();					//TEST: this would be the unique key returned from the database
			int ASSET_ID = GP_ASSET_ID_TEST_ONLY;
			
			
			
			
			GpControllerUser & controller = (dynamic_cast<GpControllerUser &>(*this));
			
			// Request a connection with an asset
			// This would better go in a separate message handler after controller specifically requested a connection.
			// Here I'm lumping this in with the login request to save dev time.
			bool assetReqResult = controller.requestConnectionToAsset(ASSET_ID);	//sets _isConnectedToPartner for both controller and asset
		
			if(assetReqResult){
				// authorized to use this asset_id
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
	_user_type = GP_USER_TYPE_ASSET;
	
	

};



GpControllerUser::GpControllerUser(){
	_user_type = GP_USER_TYPE_CONTROLLER;
	
};



bool
GpControllerUser::requestConnectionToAsset(int assetId){
	
	// Check database. Does this assetId exist?
	// Check permissions. Is this user allowed to use this asset? aka, does user "own" asset?
	
	
	
	
	std::cout << "[" << __func__ << "]" << std::endl;
	
	
	return GpDatabase::authenticateUserForAsset(*this, assetId);	//sets _isConnectedToPartner for both controller and asset
	
	
}



void
GpUser::logout(){
	
	GpDatabase::logoutUser(*this);
	
	
	
}

void
GpAssetUser::refresh(){
	
	GpAssetUser & thisAsset = (*this);
	
	// Fill this asset object with the version currently in the database
	GpDatabase::getAsset(_user_id, thisAsset);
	
	
}


