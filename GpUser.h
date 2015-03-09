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
	
};


class GpControllerUser:public GpUser{
public:
	GpControllerUser();
	GpAssetUser *_connectedAsset;
	
	GpAssetUser _asset;
	
	
	int _asset_fd;
	
	
	bool requestConnectionToAsset(int assetId);
};

#endif /* defined(__UavServer__GpUser__) */
