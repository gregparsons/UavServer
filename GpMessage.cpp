// ********************************************************************************
//
//  GpMessage.cpp
//  UavServer
//  3/2/15
//
// ********************************************************************************

#include <iostream>

#include "GpMessage.h"

void bitStuff16(uint8_t *&buffer, const uint16_t & value);
void bitUnstuff16(uint8_t *&buffer, uint16_t & value);


GpMessage::GpMessage(){};
GpMessage::GpMessage(uint8_t messageType, uint16_t payloadSize, uint8_t *&payload):_message_type(messageType),_payloadSize(payloadSize),_payload(payload) {

};

GpMessage::~GpMessage(){};

/*
 Provide a buffer and the buffer size. When written, size is changed to actual bytes written.
 
 */
void GpMessage::serialize(uint8_t *&bytes, uint16_t &buffer_size){
	std::cout << "Serialize: " << (int)_message_type << ", " << (int)_payloadSize << std::endl;
	
	if(buffer_size >= 3){
		uint8_t *ptr = bytes;

		
		
		*ptr = _message_type;
		*ptr++;
		
		bitStuff16(ptr, _payloadSize);
		std::cout << "Hello" << std::endl;
	
	}
	buffer_size = 3;
}

void GpMessage::deserialize(uint8_t *&bytes, uint16_t &buffer_size){
	
	if(buffer_size >= 3){
		
		uint8_t *ptr = bytes;
		
		_message_type =	*ptr;	//1 byte
		*ptr++;
		
		bitUnstuff16(ptr, _payloadSize);
		 
		
		
		
	}
	buffer_size = 3;
	
	std::cout << "Deserialize: " << (int)_message_type << ", " << (int)_payloadSize << std::endl;
}

void bitStuff16(uint8_t *&buffer, const uint16_t & value){

	uint16_t temp = htons(value);
	uint8_t *ptr = buffer;
	
	*ptr++ = temp;
	*ptr = temp >> 8;

}

void bitUnstuff16(uint8_t *&buffer, uint16_t & value){
	uint8_t *ptr = buffer;
	uint16_t temp = 0;
	memcpy(&temp, ptr, 2);
	value = ntohs(temp);
	
}
