// ********************************************************************************
//
//  GpMessage.cpp
//  UavServer
//  3/2/15
//
// ********************************************************************************



#include <iostream>
#include <string.h> //bzero memset
#include <netinet/in.h>


#include "GpMessage.h"
#include "GpMessage_Login.h"

// void bitStuff16(uint8_t *&buffer, const uint16_t & value);
// void bitUnstuff16(uint8_t *&buffer, uint16_t & value);


GpMessage::GpMessage(){};
GpMessage::GpMessage(GpMessage_Login & loginMessage){
	
	
	
	_message_type = GP_MSG_TYPE_LOGIN;
	_payloadSize = GP_MSG_LOGIN_LEN;
	
	if(_payload !=nullptr)
		delete _payload;
	_payload = new uint8_t[_payloadSize];
	bzero(_payload, _payloadSize);
	
	
	uint8_t *bufPtr = _payload;
	
	int numBytes = loginMessage.serialize(bufPtr);
	if(0 == numBytes){	//ref returns ptr to serialized login message

		std::cout << "[GpMessage::GpMessage]" << std::endl;

	}
	
}
GpMessage::GpMessage(uint8_t messageType, uint16_t payloadSize, uint8_t *&payload):_message_type(messageType),_payloadSize(payloadSize),_payload(payload) {

};

GpMessage::~GpMessage(){

	if(_payload != nullptr)
		delete _payload;

};


/*
 Provide a buffer and the buffer size. When written, size is changed to actual bytes written.
 
 */
void GpMessage::serialize(uint8_t *&bytes, uint16_t &buffer_size){
	// std::cout << "Serialize: " << (int)_message_type << ", " << (int)_payloadSize << std::endl;
	
	if(buffer_size >= 3){
		int byteCount = 0;
		uint8_t *ptr = bytes;

		
		
		*ptr++ = _message_type;
		
		byteCount++;
		
		
		bitStuff16(ptr, _payloadSize);
		byteCount+=2;
		
		
		memcpy(ptr, _payload, _payloadSize);
		ptr+= _payloadSize;

		buffer_size = GP_MSG_HEADER_LEN + _payloadSize;
	
	}
}

void GpMessage::deserialize(uint8_t *&bytes, uint16_t &buffer_size){
	
	if(buffer_size >= 3){
		int byteCount =0;
		uint8_t *ptr = bytes;
		
		_message_type =	*ptr;	//1 byte
		ptr++;					//should be incrementing ptr instead of *ptr???
		byteCount++;
		
		bitUnstuff16(ptr, _payloadSize);		// 		ptr+=2; done by bitstuff
		byteCount+=2;
		
		
		//if(_payload != NULL)
		//	delete _payload;
		_payload = new uint8_t[_payloadSize];

		
		
		//uint8_t *payPtr = _payload;
		
		
		memcpy(_payload, ptr, _payloadSize);
		
		/*
		for(; byteCount<buffer_size && byteCount < (_payloadSize + GP_MSG_HEADER_LEN); byteCount++){
			memcpy(payPtr++,ptr++, 1);
		}
		 */
		
	}
	std::cout << "Deserialize: " << (int)_message_type << ", " << (int)_payloadSize << ", " << _payload <<  std::endl;
}


// push a short and increment the pointer passed by 2 bytes
void GpMessage::bitStuff16(uint8_t *&buffer, const uint16_t & value){

	uint16_t temp = htons(value);
	uint8_t *ptr = buffer;
	
	*ptr++ = temp;
	*ptr = temp >> 8;

	buffer+=2;
}

// pull a short, increment pointer by 2 bytes
void GpMessage::bitUnstuff16(uint8_t *&buffer, uint16_t & value){
	uint8_t *ptr = buffer;
	uint16_t temp = 0;
	memcpy(&temp, ptr, 2);
	value = ntohs(temp);

	buffer+=2;
}
