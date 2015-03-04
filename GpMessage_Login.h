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

// #include <string>
#define GP_MSG_LOGIN_LEN 192 // 64 + 128
#define GP_MSG_LOGIN_USER_LEN 64
#define GP_MSG_LOGIN_KEY_LEN 128


class GpMessage_Login{

public:
	GpMessage_Login(std::string user64, std::string key2048);
	GpMessage_Login(uint8_t *& rawNetBytes);
	
	uint32_t serialize(uint8_t *&buffer);
	bool deserialize(uint8_t *& rawNetBytes);

	std::string username();
	std::string key();
	
private:
	
	uint8_t _user64[GP_MSG_LOGIN_USER_LEN];
	uint8_t _key2048[GP_MSG_LOGIN_KEY_LEN];
};



#endif /* defined(__UavServer__GpMessage_Login__) */
