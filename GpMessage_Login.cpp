// ********************************************************************************
//
//  GpMessage_Login.cpp
//  UavServer
//  3/2/15
//
// ********************************************************************************

#include <iostream>

#include "GpMessage_Login.h"



// Construct (use at client)

GpMessage_Login::GpMessage_Login(std::string user64, std::string key2048){

	bzero(_user64, sizeof(_user64));
	memcpy(_user64, user64.data(), user64.size());
	
	bzero(_key2048, sizeof(_key2048));
	memcpy(_key2048, key2048.data(), key2048.size());

}

// Constructor from raw network bytes (use at server)

GpMessage_Login::GpMessage_Login(uint8_t *&rawNetBytes){
	if(true != deserialize(rawNetBytes)){
		std::cout << "[GpMessage_Login::GpMessage_Login] Error deserialize constructor" << std::endl;
	}
}


// Serialize username and password, return in buffer.

uint32_t GpMessage_Login::serialize(uint8_t *&buffer){

	if(buffer == nullptr)
		return 0;
	
/*
	//buffer must be sizeof GP_MSG_LOGIN_LEN
	
	int len = sizeof((*buffer));
	
	if(len != GP_MSG_LOGIN_LEN){
		std::cout << "[GpMessage_Login::serialize] Buffer not the right size: GP_MSG_LOGIN_LEN" << std::endl;
	}
*/	
	
	uint8_t *ptr = buffer;
	
	//write username
	uint8_t *ptrU = _user64;
	memcpy(ptr, ptrU, sizeof(_user64));
	ptr += sizeof(_user64);  //increment what ptr points two by 64 bytes

	//write key
	ptrU = _key2048;
	memcpy(ptr, ptrU, sizeof(_key2048));
	ptr += sizeof(_key2048);	//in case more items added for serialize

	return sizeof(_key2048) + sizeof(_user64);
	
	
}

// Deserialize

bool GpMessage_Login::deserialize(uint8_t *&rawNetBytes){
	
	
	return true;
}