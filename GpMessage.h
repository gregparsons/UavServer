// ********************************************************************************
//
//  GpMessage.h
//  UavServer
//
// ********************************************************************************


#ifndef __UavServer__GpMessage__
#define __UavServer__GpMessage__

#define GP_MSG_HEADER_LEN 3 //bytes

class GpMessage_Login;

/**
 *  GpMessage
 *
 *  Class to construct a single application-layer message to/from client and server over TCP or UDP.
 *
 */
enum { GP_MSG_TYPE_GENERIC, GP_MSG_TYPE_LOGIN, GP_MSG_TYPE_LOGOUT, GP_MSG_TYPE_COMMAND};

class GpMessage {
public:
	GpMessage();
	GpMessage(GpMessage_Login & loginMessage);
	GpMessage(uint8_t messageType, uint16_t payloadSize, uint8_t *&payload);
	~GpMessage();

	void serialize(uint8_t *&bytes, uint16_t & size);
	void deserialize(uint8_t *&bytes, uint16_t & size);
	
private:
	uint16_t _message_type = GP_MSG_TYPE_GENERIC;
	uint16_t _payloadSize = 0;					//16 bits,
	uint8_t *_payload = nullptr;					//byte buffer

	
	
};




#endif /* defined(__UavServer__GpMessage__) */
