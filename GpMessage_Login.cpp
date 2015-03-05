// ********************************************************************************
//
//  GpMessage_Login.cpp
//  UavServer
//  3/2/15
//
// ********************************************************************************

#include <iostream>
#include <string.h> //bzero memset

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
		std::cout << "[" <<  __func__ << "] "  << "Error deserialize constructor" << std::endl;
	}
}


std::string GpMessage_Login::username(){
	return std::string(reinterpret_cast<char const*>(_user64), sizeof(_user64));
}

std::string GpMessage_Login::key(){
	return std::string(reinterpret_cast<char const*>(_key2048), sizeof(_key2048));
}




// Serialize username and password, return in buffer.

uint32_t GpMessage_Login::serialize(uint8_t *&buffer){

	if(buffer == nullptr)
		return 0;
	
	uint8_t *ptr = buffer;
	
	//write username
	uint8_t *ptrU = _user64;
	memcpy(ptr, ptrU, sizeof(_user64));
	ptr += sizeof(_user64);  //increment what ptr points two by 64 bytes

	//write key
	ptrU = _key2048;
	memcpy(ptr, ptrU, sizeof(_key2048));
	// ptr += sizeof(_key2048);	//in case more items added for serialize

	return sizeof(_key2048) + sizeof(_user64);
	
	
}

// Deserialize

bool GpMessage_Login::deserialize(uint8_t *&rawNetBytes){
	
	uint8_t *ptr = rawNetBytes;
	
	bzero(_user64, sizeof(_user64));
	memcpy(_user64, ptr, sizeof(_user64));
	
	ptr+= sizeof(_user64);
	
	bzero(_key2048, sizeof(_key2048));
	memcpy(_key2048, ptr, sizeof(_key2048));

	return true;
}