// ********************************************************************************
//
//  GpUser.h
//  UavServer
//
// ********************************************************************************


#ifndef __UavServer__GpUser__
#define __UavServer__GpUser__

#include "GpIpAddress.h"	//TEST

/**
 *  GpUser Class
 *
 *  I envision this being more full-featured and including the authentication stuff in the future.
 *
 */

class GpControllerUser;

class GpUser{
public:
	enum
	{
		GP_USER_TYPE_ASSET,
		GP_USER_TYPE_CONTROLLER
	};
	
	GpUser();
	virtual ~GpUser();
	bool authenticate(std::string username, std::string key);
	void logout();

	
	
	int _user_id = -1;
	std::string _username = "";
	int _fd = 0;					// socket file descriptor
	bool _isAuthenticated = false;


	int _user_type;
	bool _isConnectedToPartner = false;		//if a controller is not connected then the _asset object is not valid

	bool _isOnline = false;					//not used for controller
	
	
	
};



class GpAssetUser:public GpUser{
public:
	GpAssetUser();
	GpControllerUser* _connected_owner = nullptr;
	
	void refresh();

	GpAssetUser & operator=(const GpAssetUser & asset);
	
};


class GpControllerUser:public GpUser{
public:
	GpControllerUser();
	
	GpAssetUser _asset;
	
	int _assetLastRequested = GP_ASSET_ID_TEST_ONLY;		// TEST
	
	//GpAssetUser *_assetPtr = nullptr;
	
	
	
	bool requestConnectionToAsset(int assetId);
};

#endif /* defined(__UavServer__GpUser__) */
