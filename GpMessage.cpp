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
#include <vector>

#include "GpMessage.h"
#include "GpMessage_Login.h"


GpMessage::GpMessage(){
	_payLd_vec.reserve(GP_MSG_MAX_LEN);

};


// Construct with a LOGIN message

GpMessage::GpMessage(GpMessage_Login & loginMessage, int login_source_type){

	// TYPE / SIZE
	
	_message_type = login_source_type; // GP_MSG_TYPE_CONTROLLER_LOGIN or GP_MSG_TYPE_ASSET_LOGIN	
	_payloadSize = GP_MSG_LOGIN_LEN;

	// PAYLOAD
	
	_payLd_vec.clear();
	_payLd_vec.reserve(GP_MSG_LOGIN_LEN);
	loginMessage.serialize(_payLd_vec);	//returns filled vector by reference
	
	//Now _payLd_vec has the login message raw data for use as this message's payload
	
}


/**
 *  Constructor
 *
 *  Create a message with a message type and comprised of a previously serialized payload of bytes.
 *
 */
GpMessage::GpMessage(uint8_t messageType, uint16_t payloadSize, uint8_t *&payload):_message_type(messageType),_payloadSize(payloadSize),_payload(payload) {
	_payLd_vec.reserve(payloadSize);
	
	
	pushBytesToVector(_payLd_vec, payload, payloadSize);
	
};




GpMessage::~GpMessage(){
	_payLd_vec.reserve(GP_MSG_MAX_LEN);

	//if(_payload != nullptr)
	//	delete _payload;

};








/**
 *  Serialize this message.
 *
 *  Provide an empty vector and it will return filled with this object's binary data, ready for transmission.
 *
 */
void
GpMessage::serialize(std::vector<uint8_t> & byteVect){
	byteVect.push_back(_message_type);
	uint16_t netSize = htons(_payloadSize);

	byteVect.push_back((uint8_t)netSize);
	byteVect.push_back((uint8_t)(netSize >> 8));
	
	pushVectorToVector(byteVect, _payLd_vec);
	
}






/*
 
 
 Provide an empty buffer and the buffer size. When written, size is changed to actual bytes written.
 
 */
/*

void GpMessage::serialize(uint8_t *&bytes, uint16_t &buffer_size){
	std::cout << "[" << __func__ << "] "  << "ERROR DEPRECATED, use the other serialize with a vector instead" << std::endl;
	if(buffer_size >= 3){
		
		
		
		
		int byteCount = 0;
		uint8_t *ptr = bytes;

		
		
		*ptr++ = _message_type;
		
		byteCount++;
		
		
		bitStuff16(ptr, _payloadSize);		//put payload size in buffer
		byteCount+=2;
		
		
		memcpy(ptr, _payload, _payloadSize);

		
		ptr+= _payloadSize;

		buffer_size = GP_MSG_HEADER_LEN + _payloadSize;
	
	}
	 
}
*/

void GpMessage::deserialize(std::vector<uint8_t> & bytes){
	if(bytes.size() < GP_MSG_HEADER_LEN){
		std::cout << "[" << __func__ << "] "  << "Error: header not big enough." << std::endl;
		return;
	}
	
	int i=0;	//in case I add a magic number here later, don't have to change all the other indices.
	
	// TYPE
	_message_type = bytes[i++];
	
	// PAYLOAD SIZE
	uint16_t netPayloadSize = (bytes[i] << 8) | bytes[i+1];	//https://stackoverflow.com/questions/300808/c-how-to-cast-2-bytes-in-an-array-to-an-unsigned-short
	_payloadSize = ntohs(netPayloadSize);
	i+=2;
	
	
	// PAYLOAD
	std::vector<uint8_t> tempVect;
	tempVect.reserve(_payloadSize);
	tempVect.insert(tempVect.begin(), bytes.begin() + i, bytes.end());
	
	
	
}

/*
void GpMessage::deserialize(uint8_t *&bytes, uint16_t &buffer_size){
	std::cout << "[" << __func__ << "] "  << "ERROR DEPRECATED, use the other deserialize with a vector instead" << std::endl;
	
	if(buffer_size >= 3){
		int byteCount =0;
		uint8_t *ptr = bytes;
		
		_message_type =	*ptr;	//1 byte
		byteCount++;
		ptr++;					
		
		bitUnstuff16(ptr, _payloadSize);
		byteCount+=2;
 
		_payload = new uint8_t[_payloadSize];
 
		
		if(_payloadSize > 0)
			memcpy(_payload, ptr, _payloadSize);
		
		setPayload(ptr, _payloadSize);
		
 
	}
}
*/

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


uint16_t GpMessage::size(){
	
	return GP_MSG_HEADER_LEN + _payLd_vec.size();
	
}


void GpMessage::clear(){
	
	_message_type = 0;
	_payload = nullptr;
	_payloadSize = 0;
	
	_payLd_vec.clear();
	
}




/**
 *  Copy bytes into a vector.
 *
 */
void GpMessage::insertBytesToVectorAtLocation(std::vector<uint8_t> & vec, long vecStartPoint, uint8_t *& bytes, long numBytes){
	
	if(numBytes > 0){
		
		vec.reserve(vecStartPoint + numBytes);
		
		std::vector<uint8_t> tempVector(bytes, bytes + numBytes);
		
		std::vector<uint8_t>::const_iterator itr = vec.begin();
		vec.insert(itr + vecStartPoint, tempVector.begin(), tempVector.end());
	
	}
	
	//Don't waste time if bytes are zero.
	

}

/**
 *  Copy bytes to the end of a vector.
 *
 */
void GpMessage::pushBytesToVector(std::vector<uint8_t> & vec, uint8_t *&bytes, long numBytes){
	insertBytesToVectorAtLocation(vec, vec.size(), bytes, numBytes);
}


/**
 *  Push a vector onto a vector.
 *
 */ 
void GpMessage::pushVectorToVector(std::vector<uint8_t> & dest, std::vector<uint8_t> & vecToInsert){
	
	std::vector<uint8_t>::const_iterator itr = dest.end();
	
	dest.reserve(dest.size() + vecToInsert.size());
	dest.insert(itr, vecToInsert.begin(), vecToInsert.end());
	
}




/**
 *  Insert a vector into a vector at location.
 *
 */
void GpMessage::insertVectorToVectorAtLocation(long idx, std::vector<uint8_t> & dest, std::vector<uint8_t> & vecToInsert)
{
	std::vector<uint8_t>::const_iterator itr = dest.begin();

	dest.reserve(dest.size()+vecToInsert.size());
	
	itr+= idx;
	dest.insert(itr, vecToInsert.begin(), vecToInsert.end());
	
	
}


/**
 *  Set Payload (with a vector or with bytes)
 *
 */
void GpMessage::setPayload(std::vector<uint8_t> payLdVect){
	
	_payLd_vec.swap(payLdVect);
	
}


void GpMessage::setPayload(uint8_t *& bytes, long numBytes){
	
	pushBytesToVector(_payLd_vec, bytes, numBytes);
	
	
	
	
}
