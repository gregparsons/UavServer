// ********************************************************************************
//
//  GpMessage_Login.h
//  UavServer
//  3/2/15
//
//  gp
//    Copyright (c) 2015 swimr. All rights reserved.
//
// ********************************************************************************


#ifndef __UavServer__GpMessage_Login__
#define __UavServer__GpMessage_Login__

#include <vector>

#define GP_MSG_LOGIN_LEN 192 // 64 + 128
#define GP_MSG_LOGIN_USER_LEN 64
#define GP_MSG_LOGIN_KEY_LEN 128

class GpMessage_Login{

public:
	GpMessage_Login(std::string user64, std::string loginKey);
	GpMessage_Login(std::vector<uint8_t> & rawVect);
	
	long serialize(std::vector<uint8_t> & serializedVect);
	void deserialize(std::vector<uint8_t> & rawVect);

	std::string username();
	void setUsername(std::string username);
	std::string key();
	void setKey(std::string key);
	
	
private:
	
	uint8_t _user64[GP_MSG_LOGIN_USER_LEN];
	uint8_t _loginKey[GP_MSG_LOGIN_KEY_LEN];
	
	std::vector<uint8_t> _userVect;		//Probably better off as std::array since they need to be fixed size for message format
	std::vector<uint8_t> _keyVect;
	
};



#endif /* defined(__UavServer__GpMessage_Login__) */
