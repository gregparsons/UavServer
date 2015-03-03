// ********************************************************************************
//
//  GpMessage.h
//  UavServer
//
// ********************************************************************************


#ifndef __UavServer__GpMessage__
#define __UavServer__GpMessage__

#define GP_MSG_HEADER_LEN 3 //bytes

/**
 *  GpMessage
 *
 *  Class to construct a single application-layer message to/from client and server over TCP or UDP.
 *
 */

class GpMessage {
public:
	enum { GP_MESG_GENERIC, GP_MESG_LOGIN, GP_MESG_LOGOUT, GP_MESG_COMMAND};
	GpMessage();
	GpMessage(uint8_t messageType, uint16_t payloadSize, uint8_t *&payload);
	~GpMessage();

	void serialize(uint8_t *&bytes, uint16_t & size);
	void deserialize(uint8_t *&bytes, uint16_t & size);
	
private:
	uint8_t _message_type = GP_MESG_GENERIC;
	uint16_t _payloadSize = 0;					//16 bits,
	uint8_t *_payload = nullptr;					//byte buffer

	
	
};




#endif /* defined(__UavServer__GpMessage__) */
