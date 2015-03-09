// ********************************************************************************
//
//  GpAsset.h
//  UavServer
//  3/7/15
//
//  gp
//    Copyright (c) 2015 swimr. All rights reserved.
//
// ********************************************************************************


#ifndef __UavServer__GpAsset__
#define __UavServer__GpAsset__

class GpMessage;
class GpClientNet;

class GpAsset{
public:
	
	GpAsset();
	
	bool connectServer();
	
	static bool handle_messages(GpMessage & message, GpClientNet & Net);
	
private:
};


#endif /* defined(__UavServer__GpAsset__) */
