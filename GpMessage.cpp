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







void GpMessage::setTimestampToNow(){
	
	
	
	//drop the upper four bytes. Don't need years, etc.
	uint32_t now =(uint32_t) std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	
	//std::cout << "[" << __func__ << "] " << "Timestamp set: " << uint32_t(now) << std::endl;
	
	_timestamp = now;

	
	
}







/**
 *  Constructor
 *
 *  Create a message with a message type and comprised of a previously serialized payload of bytes.
 *
 */
GpMessage::GpMessage(uint8_t messageType, uint16_t payloadSize, uint8_t *&payload)
{
	_message_type = messageType;
	_payloadSize = payloadSize;
	_payLd_vec.reserve(payloadSize);
	pushBytesToVector(_payLd_vec, payload, payloadSize);

	//setTimestampToNow();		// wait until message send
	
	
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







GpMessage::~GpMessage(){
	_payLd_vec.reserve(GP_MSG_MAX_LEN);


};





/**
 *  Serialize this message.
 *
 *  Provide an empty vector and it will return filled with this object's binary data, ready for transmission.
 *
 */
void
GpMessage::serialize(std::vector<uint8_t> & byteVect){
	
	byteVect.clear();
	byteVect.reserve(GP_MSG_HEADER_LEN + _payloadSize);

	// Message Type
	byteVect.push_back(_message_type);
	
	// Payload Size
	uint16_t netSize = htons(_payloadSize);
	byteVect.push_back((uint8_t)netSize);
	byteVect.push_back((uint8_t)(netSize >> 8));
	
	
	// Timestamp to vector
	uint32_t ts = htonl(_timestamp);

	uint8_t b1 =(uint8_t)(ts >> 24) & 0xFF;
	uint8_t b2 =(uint8_t)(ts >> 16) & 0xFF;
	uint8_t b3 =(uint8_t)(ts >> 8) & 0xFF;
	uint8_t b4 =(uint8_t)(ts) & 0xFF;
	
	byteVect.push_back(b1);
	byteVect.push_back(b2);
	byteVect.push_back(b3);
	byteVect.push_back(b4);
	
	
	// Payload vector to serialized vector
	pushVectorToVector(byteVect, _payLd_vec);

	std::cout << "Serialized Time: " << std::bitset<32>( _timestamp) << " (" << _timestamp << ") \nnet: " << std::bitset<32>(ts) << " (" << ts << ")" << std::endl;
//	std::cout << "Serialized Time: " << _timestamp << std::endl;

	
	
	
	
	
	
	
	
	
	
	
	// deserialize(byteVect);
	
	
	
	
	
	
	
	
	
	
}





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
	i+=sizeof(uint16_t);	// 2
	
	uint32_t timePackets = (bytes[i] | bytes[i+1] << 8 | bytes[i+2] << 16 | bytes[i+3] << 24 );
	_timestamp = ntohl(timePackets);
	i+=sizeof(uint32_t);	// 4
	
	
	
	
	
	
	
	
	//std::cout << "Deserialized Time: " << std::bitset<32>( _timestamp) << std::endl;
	//std::cout << "Deserialized Time: " << _timestamp << std::endl;
	
	
	
	
	
	
	
	
	
	// PAYLOAD
	std::vector<uint8_t> tempVect;
	tempVect.reserve(_payloadSize);
	tempVect.insert(tempVect.begin(), bytes.begin() + i, bytes.end());
	
	
	
	
	
	
	
	
	
	
	// What does this do? What happened to the copy to *this?
	
	
	
	
	
	
	
	
	
	
	
}



// push a short and increment the pointer passed by 2 bytes
void GpMessage::bytePack16(uint8_t *&buffer, const uint16_t & value){

	uint16_t temp = htons(value);
	uint8_t *ptr = buffer;
	
	*ptr++ = temp;
	*ptr = temp >> 8;

	buffer+=sizeof(uint16_t);
}

// pull a short, increment pointer by 2 bytes
void GpMessage::byteUnpack16(uint8_t *&buffer, uint16_t & value){
	uint8_t *ptr = buffer;
	uint16_t temp = 0;
	
	memcpy(&temp, ptr, sizeof(uint16_t));
	value = ntohs(temp);

	buffer+=sizeof(uint16_t);  // 2
}

// push a short and increment the pointer passed by 2 bytes
void GpMessage::bytePack32(uint8_t *&buffer, const uint32_t & value){
	
	uint32_t temp = htonl(value);
	uint8_t *ptr = buffer;
	

	
	buffer+= sizeof(uint32_t);	// 4

	std::cout << "Packing 32: " << std::bitset<32>(temp) << " (" << uint32_t(temp) << ")" << std::endl;

	
	
	
	//TEST
	ptr -= 4;
	uint32_t tempval = 0;
	byteUnpack32(ptr, tempval);
	std::cout << tempval << std::endl;
	

}

// pull a short, increment pointer by 2 bytes
void GpMessage::byteUnpack32(uint8_t *&buffer, uint32_t & value){
	
	/*
	uint32_t timePackets = (bytes[i] | bytes[i+1] << 8 | bytes[i+2] << 16 | bytes[i+3] << 24 );
	_timestamp = ntohl(timePackets);
	
	*/
	
	
	
	uint8_t *ptr = buffer;
	uint32_t temp = 0;
	
	
	uint8_t b0 =(uint8_t)(*ptr) & 0xFF;
	uint8_t b1 =(uint8_t)(*(ptr+1)) & 0xFF;
	uint8_t b2 =(uint8_t)(*(ptr+2)) & 0xFF;
	uint8_t b3 =(uint8_t)(*(ptr+3)) & 0xFF;
	
	temp = b3 | b2 << 8 | b1 << 16 | b0 << 24;
	
	
	
	// memcpy(&temp, ptr, sizeof(uint32_t));
	value = ntohl(temp);
	
	std::cout << "Unpacking 32: " << std::bitset<32>(value) << " (" << uint32_t(value) << ") net (" << uint32_t(temp)<< ")" << std::endl;
	
	buffer+= sizeof(uint32_t);
}



uint16_t GpMessage::size(){
	
	return GP_MSG_HEADER_LEN + _payLd_vec.size();
	
}


void GpMessage::clear(){
	
	_message_type = 0;
	_payloadSize = 0;
	_timestamp = 0;
	
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
