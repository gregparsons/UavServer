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
		
		
		
		_user_id = rand();		//this would be the unique key returned from the database
		
		
		
		
		
		
		// If ASSET:
		
		// insert into database (list of active assets)
		
		if(typeid(*this).name() == typeid(GpAssetUser).name()){

			
			_user_id = GP_ASSET_ID_TEST_ONLY ; 
			
			
			
			if(GpDatabase::insertAsset(dynamic_cast<GpAssetUser &>(*this)) == true)
				 (dynamic_cast<GpAssetUser &>(*this))._connected = true;
		
		}
		else if(typeid(*this).name() == typeid(GpControllerUser).name()){
		
			// If CONTROLLER:
			
			// check if asset is active:
			
			
			std::cout << "[" << __func__ << "] A controller is requesting asset: " << GP_ASSET_ID_TEST_ONLY << std::endl;
			
			
			
			
			// ALERT HARD CODED TEST!!
			int ASSET_ID = GP_ASSET_ID_TEST_ONLY;
			
			
			
			
			GpControllerUser & controller = (dynamic_cast<GpControllerUser &>(*this));
			
			// Request a connection with an asset
			// This would better go in a separate message handler after controller specifically requested a connection.
			// Here I'm lumping this in with the login request to save dev time.
			bool assetReqResult = controller.requestConnectionToAsset(ASSET_ID);
		
			if(assetReqResult){
				// authorized to use this asset_id

				
				
				
				// I didn't intend for this call to GPDatabase::getAsset to come out of private GpDatabase.
				// I'd prefer, as originally intended, that the asset is set in requestConnectionToAsset.
				// This is basically circumventing the security I was going for to prevent any old code from
				// getting ANY asset.  DB was going to handle that logic.
				//GpDatabase::getAsset(ASSET_ID, controller._asset);
				

				
				
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