// ********************************************************************************
//
//  GpDatabase.h
//  UavServer
//
//	Server user/asset database.
//
// ********************************************************************************



#ifndef __UavServer__GpDatabase__
#define __UavServer__GpDatabase__

/*
#import "mysql_connection.h"
#import "mysql_driver.h"
#import "mysql_error.h"


#import <cppconn/connection.h>
#import <cppconn/driver.h>
#import <cppconn/prepared_statement.h>
#import <cppconn/resultset.h>
#import <cppconn/resultset_metadata.h>
#import <cppconn/statement.h>
*/

#include <thread>
#include <mutex>
#include <unordered_map>
#include "GpUser.h"

class GpDatabase {
	
	
public:

	
	static bool authenticateUser(std::string username, std::string key);
	static void logoutUser(GpUser & user);
	
	
	static bool authenticateUserForAsset(GpControllerUser & user, int asset_id);
	


	
	static bool getAsset(int assetId, GpAssetUser & user);
	static bool insertAsset(GpAssetUser & asset);
	static bool updateAsset(GpAssetUser &asset);

private:
	
	static std::unordered_map<int, GpAssetUser> assets;
	
	static std::mutex assets_mutex;
	
	
};







#endif /* defined(__UavServer__GpDatabase__) */
