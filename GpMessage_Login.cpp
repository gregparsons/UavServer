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
#include "GpMessage.h"


// Construct (use at client)
/**
 *  Login request message for use at client.
 *
 *  @param std::string user, std::string key
 */
GpMessage_Login::GpMessage_Login(std::string user64, std::string loginKey){

	// _userVect.resize(GP_MSG_LOGIN_USER_LEN);	//this is crucial because message formatting requires these to be fixed length
	// _keyVect.resize(GP_MSG_LOGIN_KEY_LEN);
	
	setUsername(user64);
	setKey(loginKey);
}

/**
 *  Login Request from raw net bytes constructor from raw network bytes (use at server to receive).
 *
 */
GpMessage_Login::GpMessage_Login(std::vector<uint8_t> & rawVect){
//GpMessage_Login::GpMessage_Login(uint8_t *&rawNetBytes){

	// Should be of size GP_MSG_LOGIN_LEN. Can't really help if it's not.
	
	_userVect.resize(GP_MSG_LOGIN_USER_LEN);	//this is crucial because message formatting requires these to be fixed length
	_keyVect.resize(GP_MSG_LOGIN_KEY_LEN);
	
	deserialize(rawVect);	//copy raw to _userVect and _keyVect
}


std::string GpMessage_Login::username(){
	//return std::string(reinterpret_cast<char const*>(_user64), sizeof(_user64));

	
	// This might not work the way it seems. What if there are special characters? Straight up string compare later won't work out.
	std::string username;
	username.assign(_userVect.begin(), _userVect.end());
	
	
	
	std::cout << "USERNAME() NOT IMPLEMENTED" << std::endl;
	
	return username;
}

std::string GpMessage_Login::key(){
	std::cout << "key() NOT IMPLEMENTED" << std::endl;

	// This might not work the way it seems. What if there are special characters? Straight up string compare later won't work out.
	std::string key;
	key.assign(_userVect.begin(), _userVect.end());

	
	return "";
	//return std::string(reinterpret_cast<char const*>(_loginKey), sizeof(_loginKey));
}


/**
 *  Serializes this login request message for transmission (from client).
 *
 */
long GpMessage_Login::serialize(std::vector<uint8_t> & serializedVect){

	GpMessage::pushVectorToVector(serializedVect, _userVect);
	GpMessage::pushVectorToVector(serializedVect, _keyVect);
	
	return serializedVect.size();
	
}



/*
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
	ptrU = _loginKey;
	memcpy(ptr, ptrU, sizeof(_loginKey));
	// ptr += sizeof(_loginKey);	//in case more items added for serialize

	return sizeof(_loginKey) + sizeof(_user64);
	
	
}
*/







// Deserialize

void GpMessage_Login::deserialize(std::vector<uint8_t> & rawVect){

	/*
	 
	 The range used is [first,last), which includes all the elements between first and last, including the element pointed by first but not the element pointed by last.
	 
	 */
	
	
	std::vector<uint8_t>::const_iterator first = rawVect.begin();
	std::vector<uint8_t>::const_iterator last = first + GP_MSG_LOGIN_USER_LEN;
	std::vector<uint8_t> tempVect(first, last);
	_userVect.swap(tempVect);
	
	first = last;
	last = rawVect.end();
	std::vector<uint8_t> tempVect2(first, last);
	_keyVect.swap(tempVect2);
	
	/*
	uint8_t *ptr = rawNetBytes;
	GpMessage::pushBytesToVector(_userVect, ptr, GP_MSG_LOGIN_USER_LEN);
	ptr+= GP_MSG_LOGIN_USER_LEN;
	GpMessage::pushBytesToVector(_keyVect, ptr, GP_MSG_LOGIN_KEY_LEN);
*/
	/*
	 bzero(_user64, sizeof(_user64));
	 memcpy(_user64, ptr, sizeof(_user64));
	ptr+= sizeof(_user64);
	 bzero(_loginKey, sizeof(_loginKey));
	 memcpy(_loginKey, ptr, sizeof(_loginKey));

*/

}



void GpMessage_Login::setUsername(std::string username){
	
	_userVect.insert(_userVect.begin(), username.begin(), username.end());
	_userVect.resize(GP_MSG_LOGIN_USER_LEN);
	
}

void GpMessage_Login::setKey(std::string key){
	_keyVect.insert(_keyVect.begin(), key.begin(), key.end());
	_keyVect.resize(GP_MSG_LOGIN_KEY_LEN);
}
