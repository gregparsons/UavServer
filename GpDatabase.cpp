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

	try {
		user = assets.at(assetId);
		return true;
	} catch (const std::out_of_range & oor) {
		std::cout << "[" << __func__ << "] "  << "Asset Id "<< assetId <<" not found" << std::endl;
		return false;
	}
	
}


bool GpDatabase::authenticateUserForAsset(GpControllerUser & user, int asset_id){
	
	
	//if asset_id exists and user._user_id is allowed to use asset_id then...

	if(	(getAsset(asset_id, user._asset) == true) /* && user owns asset */){

		std::cout << "[" << __func__ << "] "  << "[checking asset exists only] User " << user._username << " authorized to use asset id: " << asset_id << std::endl;
	

		user._asset._connected = true;
		user._asset._connected_owner = &user;
		
		updateAsset(user._asset);
		
		return true;
	}
	
	return false;
}



bool GpDatabase::insertAsset(GpAssetUser &asset){
	
	std::lock_guard<std::mutex> lock(assets_mutex);
	
	std::pair<std::unordered_map<int, GpAssetUser>::iterator, bool> result;
	result = assets.insert(std::pair<int, GpAssetUser>(asset._user_id, asset));
	if(result.second == false){
		//insert failed
		std::cout << "[" << __func__ << "] "  << "Asset insert failed. Duplicate." << std::endl;
		
		return false;
		
	}
	else{
		std::cout << "[" << __func__ << "] "  << "Asset insert succeeded." << std::endl;
	
		return true;
	}
}


bool GpDatabase::updateAsset(GpAssetUser &asset){

	try{
		
		std::lock_guard<std::mutex> lock(assets_mutex);
		
		assets.at(asset._user_id);
		assets[asset._user_id]._connected = asset._connected;
		assets[asset._user_id]._connected_owner = asset._connected_owner;
		assets[asset._user_id]._isAuthenticated = asset._isAuthenticated;
		return true;
	
	} catch (const std::out_of_range & oor) {
	
		std::cout << "[" << __func__ << "] "  << "Asset Id "<< asset._user_id <<" not found" << std::endl;
		return false;
	}
}
