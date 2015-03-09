// ********************************************************************************
//
//  GpUser.h
//  UavServer
//
// ********************************************************************************


#ifndef __UavServer__GpUser__
#define __UavServer__GpUser__



/**
 *  GpUser Class
 *
 *  I envision this being more full-featured and including the authentication stuff in the future.
 *
 */

class GpControllerUser;

class GpUser{
public:
	
	GpUser();
	virtual ~GpUser();
	bool authenticate(std::string username, std::string key);

	
	
	int _user_id = -1;
	std::string _username = "";
	int _fd = 0;					// socket file descriptor
	bool _isAuthenticated = false;

};



class GpAssetUser:public GpUser{
public:
	GpAssetUser();
	bool _connected = false;
	GpControllerUser* _connected_owner = nullptr;
	
};


class GpControllerUser:public GpUser{
public:
	GpControllerUser();
	GpAssetUser _asset;
	
	
	
	bool requestConnectionToAsset(int assetId);
};

#endif /* defined(__UavServer__GpUser__) */
