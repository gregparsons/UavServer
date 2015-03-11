 // ********************************************************************************
//
//  GpDatabase.cpp
//  UavServer
//  3/1/15
//
// ********************************************************************************

#include <iostream>
#include <unordered_map>
#include <thread>
#include <mutex>

#include "GpDatabase.h"
#include "GpIpAddress.h"


//Statics

std::unordered_map<int, GpAssetUser> GpDatabase::assets;	//This literally took two hours to compile. Thanks stackoverflow.
std::mutex GpDatabase::assets_mutex;




bool GpDatabase::authenticateUser(std::string username, std::string key){
	
	std::cout << "[" << __func__ << "] "  << "Does nothing. All users return true." <<std::endl;


	
	if(username.compare(GP_CONTROLLER_SERVER_IP) && key.compare(GP_CONTROLLER_TEST_PASSWORD)){
		std::cout << "[" << __func__ << "] "  << "User: " << username << " authenticated." << std::endl;

		
		
		return true;
	}else{
		std::cout << "[" << __func__ << "] "  << "User: " << username << " not authenticated." << std::endl;

		
		
		return true;

	}
}


bool GpDatabase::getAsset(int assetId, GpAssetUser & user){

	std::lock_guard<std::mutex> lock(assets_mutex);
	bool returnValue = false;
	try {
		user = assets.at(assetId);			//this is doing a full operator=.....hmmm.
		returnValue = true;
	} catch (const std::out_of_range & oor) {
		std::cout << "[" << __func__ << "] "  << "Asset Id "<< assetId <<" not found" << std::endl;
		returnValue = false;
	}
	return returnValue;
	
}


bool GpDatabase::authenticateUserForAsset(GpControllerUser & controller, int asset_id){
	
	
	//if asset_id exists and user._user_id is allowed to use asset_id then...

	if(	(getAsset(asset_id, controller._asset) == true) /* && user owns asset */){

			if(controller._asset._isOnline){
			
			// TEST
			std::cout << "[" << __func__ << "] "  << "[checking asset exists only] User " << controller._username << " authorized to use asset id: " << asset_id << std::endl;
		

			controller._asset._connected_owner = &controller;
			controller._asset._isConnectedToPartner = true;
			controller._isConnectedToPartner = true;
			
			insertUpdateAsset(controller._asset);
			
			return true;
		}
	}
	
	return false;
}


/*
bool GpDatabase::insertAsset(GpAssetUser &asset){
	
	std::lock_guard<std::mutex> lock(assets_mutex);
	
	std::pair<std::unordered_map<int, GpAssetUser>::iterator, bool> result;
	result = assets.insert(std::pair<int, GpAssetUser>(asset._user_id, asset));
	if(result.second == false){
		//insert failed
		
		std::cout << "[" << __func__ << "] "  << "Asset insert failed. Duplicate. Updating instead." << std::endl;
		
		
		return updateAsset(asset);
		
	}
	else{
		std::cout << "[" << __func__ << "] "  << "Asset insert succeeded." << std::endl;
	
		return true;
	}
}
*/

/*
bool GpDatabase::updateAsset(GpAssetUser &asset){

	try{
		
		std::lock_guard<std::mutex> lock(assets_mutex);
		
		assets.at(asset._user_id);
		// assets[asset._user_id]
		assets[asset._user_id]._user_type = asset._user_type;
		assets[asset._user_id]._username = asset._username;

		assets[asset._user_id]._isAuthenticated = asset._isAuthenticated;
		assets[asset._user_id]._isOnline = asset._isOnline;
		assets[asset._user_id]._isConnectedToPartner = asset._isConnectedToPartner;
		assets[asset._user_id]._connected_owner = asset._connected_owner;
		return true;
	
	} catch (const std::out_of_range & oor) {
	
		std::cout << "[" << __func__ << "] "  << "Asset Id "<< asset._user_id <<" not found" << std::endl;
		return false;
	}
}
*/


/*
 If k does not match the key of any element in the container, the function inserts a new element with that key and returns a reference to its mapped value. Notice that this always increases the container size by one, even if no mapped value is assigned to the element (the element is constructed using its default constructor).
 */

void GpDatabase::insertUpdateAsset(GpAssetUser &asset){
	
	std::lock_guard<std::mutex> lock(assets_mutex);
	
	assets[asset._user_id] = asset;	//copy contents of the inbound asset to the one stored in the database
	
	
}




void GpDatabase::logoutUser(GpUser & user){

	std::cout << "[" << __func__ << "] Logging out: " << user._username << " on socket: " << user._fd << std::endl;

	
	if(user._user_type == GpUser::GP_USER_TYPE_CONTROLLER){
		GpControllerUser & controller = (GpControllerUser &)user;
		
		//If controller is logging out, remove reference to it in a still logged-in asset.
		
		if(controller._isConnectedToPartner == true) //then the user.asset is valid
		{
			controller._asset._isConnectedToPartner = false;
			controller._asset._connected_owner = nullptr;
			
			insertUpdateAsset(controller._asset);
		}
		controller._isConnectedToPartner = false;
		controller._isAuthenticated = false;
		controller._isOnline = false;
		
	}
	else if(user._user_type == GpUser::GP_USER_TYPE_ASSET){

		// If an asset is logging out, update its connection status and owner_pointer
		
		GpAssetUser & asset = (GpAssetUser &)user;
		
		if(asset._isConnectedToPartner == true){

			// Remove 1:1 relationship with controller
			
			asset._connected_owner->_isConnectedToPartner = false;
			asset._connected_owner = nullptr;
			
		}
		asset._isConnectedToPartner = false;
		asset._isAuthenticated = false;
		asset._isOnline = false;
		
		
	}
}