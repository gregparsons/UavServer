// ********************************************************************************
//
//  GpMessage.h
//  UavServer
//
// ********************************************************************************


#ifndef __UavServer__GpMessage__
#define __UavServer__GpMessage__

// #define GP_MSG_HEADER_LEN 3 //bytes
// #define GP_MSG_MAX_LEN 512 //bytes

#include <vector>
#include <ctime>


typedef std::vector<uint8_t> byte_vector;

class GpMessage_Login;

/**
 *  GpMessage
 *
 *  Class to construct a single application-layer message to/from client and server over TCP or UDP.
 *
 */
enum {
	GP_MSG_TYPE_NONE_ZERO,
	GP_MSG_TYPE_CONTROLLER_LOGIN,
	GP_MSG_TYPE_LOGOUT,
	GP_MSG_TYPE_COMMAND,
	GP_MSG_TYPE_AUTHENTICATED_BY_SERVER,
	GP_MSG_TYPE_HEARTBEAT,
	GP_MSG_TYPE_ASSET_LOGIN,
	GP_MSG_TYPE_MATCH_CONNECTED,		//a controller receives this when a requested asset connects, asset receives when paired with a controller
	GP_MSG_TYPE_MATCH_DISCONNECTED,		//notify controller that the asset dropped off, or notify the asset that the controller isn't connected
	GP_MSG_ASSET_HEARTBEAT,
	GP_MSG_TYPE_PING,
	GP_MSG_TYPE_PONG,
	GP_MSG_TYPE_MAX_DONT_USE

};


enum{
	GP_MSG_TYPE_PING_LEN = 10,
	GP_MSG_TYPE_PONG_LEN = 10,
	GP_MSG_HEADER_LEN = 7,			// 3 without timestamp
	GP_MSG_MAX_LEN = 512
	
};


class GpMessage {
public:
	GpMessage();
	
	// The typical constructor
	GpMessage(uint8_t messageType, uint16_t payloadSize, uint8_t *&payload);
	
	
	// Constructor based off a login message only
	GpMessage(GpMessage_Login & loginMessage, int login_source_type);	//source controller or asset
	
	
	~GpMessage();
	
	// Serialize or deserialize this message
	void serialize(std::vector<uint8_t> & bytes);
	void deserialize(std::vector<uint8_t> & bytes);

	void setTimestampToNow();
	uint16_t size();
	void clear();
	
	

	// Payload
	
	void setPayload(uint8_t *& bytes, long numBytes);
	void setPayload(std::vector<uint8_t> payLdVect);
	
	// Byte utils
	static void bytePack16(uint8_t *&buffer, const uint16_t & value);
	static void byteUnpack16(uint8_t *&buffer, uint16_t & value);
	static void bytePack32(uint8_t *&buffer, const uint32_t & value);
	static void byteUnpack32(uint8_t *&buffer, uint32_t & value);

	// Vector utils
	static void pushBytesToVector(std::vector<uint8_t> & vec, uint8_t *&bytes, long numBytes);
	static void insertBytesToVectorAtLocation(std::vector<uint8_t> & vec, long vecStartPoint, uint8_t *& bytes, long numBytes);
	static void pushVectorToVector(std::vector<uint8_t> & dest, std::vector<uint8_t> & vecToInsert);
	static void insertVectorToVectorAtLocation(long idx, std::vector<uint8_t> & dest, std::vector<uint8_t> & vecToInsert);

	
	
	
	// Header: 3 Bytes + 4 bytes timestamp
	uint8_t _message_type = GP_MSG_TYPE_NONE_ZERO;		// 1 byte
	uint16_t _payloadSize = 0;							// 2 bytes
	time_t _timestamp = 0;								// 4 bytes
	
	// Payload
	std::vector<uint8_t> _payLd_vec;
	
private:
	
	
};




#endif /* defined(__UavServer__GpMessage__) */
