// ********************************************************************************
//
//  GpMessage.h
//  UavServer
//
// ********************************************************************************


#ifndef __UavServer__GpMessage__
#define __UavServer__GpMessage__

#define GP_MSG_HEADER_LEN 3 //bytes
#define GP_MSG_MAX_LEN 512 //bytes

#include <vector>


typedef std::vector<uint8_t> byte_vector;

class GpMessage_Login;

/**
 *  GpMessage
 *
 *  Class to construct a single application-layer message to/from client and server over TCP or UDP.
 *
 */
enum { GP_MSG_TYPE_NONE_ZERO, GP_MSG_TYPE_LOGIN, GP_MSG_TYPE_LOGOUT, GP_MSG_TYPE_COMMAND, GP_MSG_TYPE_AUTHENTICATED_BY_SERVER, GP_MSG_TYPE_HEARTBEAT, GP_MSG_TYPE_MAX_DONT_USE};

class GpMessage {
public:
	GpMessage();
	GpMessage(GpMessage_Login & loginMessage);
	GpMessage(uint8_t messageType, uint16_t payloadSize, uint8_t *&payload);
	~GpMessage();

//	void serialize(uint8_t *&bytes, uint16_t & size);
	void serialize(std::vector<uint8_t> & bytes);
	
	void deserialize(std::vector<uint8_t> & bytes);
//	void deserialize(uint8_t *&bytes, uint16_t & size);

	uint8_t _message_type = GP_MSG_TYPE_NONE_ZERO;
	uint16_t _payloadSize = 0;					//16 bits,
	uint8_t *_payload = nullptr;					//byte buffer
	std::vector<uint8_t> _payLd_vec;
	
	
	
	
	
	uint16_t size();
	void clear();

	static void bitStuff16(uint8_t *&buffer, const uint16_t & value);
	static void bitUnstuff16(uint8_t *&buffer, uint16_t & value);

	
	// Payload
	
	void setPayload(uint8_t *& bytes, long numBytes);
	void setPayload(std::vector<uint8_t> payLdVect);
	
	static void pushBytesToVector(std::vector<uint8_t> & vec, uint8_t *&bytes, long numBytes);
	static void insertBytesToVectorAtLocation(std::vector<uint8_t> & vec, long vecStartPoint, uint8_t *& bytes, long numBytes);
	static void pushVectorToVector(std::vector<uint8_t> & dest, std::vector<uint8_t> & vecToInsert);
	static void insertVectorToVectorAtLocation(long idx, std::vector<uint8_t> & dest, std::vector<uint8_t> & vecToInsert);

	
	
private:
	
	
};




#endif /* defined(__UavServer__GpMessage__) */
