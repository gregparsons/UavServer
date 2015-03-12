//
//  GpControllerEvent.cpp
//

#include "GpControllerEvent.h"
#include <iostream>
#include <bitset>
#include <arpa/inet.h>


using namespace std;

//default constructor
GpControllerEvent::GpControllerEvent():_leftX(0), _leftY(0), _rightX(0), _rightY(0), _timestamp(0){

}

//Received event from raw
GpControllerEvent::GpControllerEvent(uint8_t* &buf, int bufSize):_buffer(buf), _bufferSize(bufSize){ }		//change to references



//constructor for event to send
GpControllerEvent::GpControllerEvent(int lx, int ly, int rx, int ry, Uint32 ts):
	_leftX(lx), _leftY(ly), _rightX(rx), _rightY(ry), _timestamp(ts) {

}


// ********* Getters


int GpControllerEvent::leftX(){
	return _leftX;
}

int GpControllerEvent::leftY(){
	return _leftY;
}

int GpControllerEvent::rightX(){
	return _rightX;
}

int GpControllerEvent::rightY(){
	return _rightY;
}

Uint32 GpControllerEvent::timestamp(){
	return _timestamp;
}


// ******** Setters


void GpControllerEvent::setLX(int lx){
	_leftX = lx;
}

void GpControllerEvent::setLY(int ly){
	_leftY = ly;
}

void GpControllerEvent::setRX(int rx){
	_rightX = rx;
}

void GpControllerEvent::setRY(int ry){
	_rightY = ry;
}

void GpControllerEvent::setTimestamp(Uint32 ts){
	_timestamp = ts;
}


/**
 Serialize this instance for network transmission
 */
bool GpControllerEvent::serializeAsNetworkOrderBytes(uint8_t * &bytes, int & size){

	//allocate
	size = sizeof(GpControllerEvent);
	bytes = new uint8_t[sizeof(GpControllerEvent)];

	//make a couple copies of the head pointer
	//uint8_t *ptr = bytes;
	uint8_t *data = bytes;

	
	// Timestamp
	// to network order then to byte array
	*data++ = timestamp() >>24;
	*data++ = timestamp()>>16;
	*data++ = timestamp()>>8;
	*data++ = timestamp();
	//ptr+=4;
	
	// Event Left X
	*data++ = leftX() >>24;
	*data++ = leftX()>>16;
	*data++ = leftX()>>8;
	*data++ = leftX();
	
	// Event Left Y
	*data++ = leftY() >>24;
	*data++ = leftY() >>16;
	*data++ = leftY() >>8;
	*data++ = leftY();
	
	// Event Right X
	*data++ = rightX() >>24;
	*data++ = rightX()>>16;
	*data++ = rightX()>>8;
	*data++ = rightX();
	
	// Event Right Y
	*data++ = rightY() >>24;
	*data++ = rightY()>>16;
	*data++ = rightY()>>8;
	*data++ = rightY();
	
	
	return true;
}


// deserialize a GpControllerEvent object back into a class object
bool GpControllerEvent::deserializeToObjectCopy(){
	
	
	
	//cout << "Deserialize: \n";
	
	//e2.setTimestamp((head[i]<<24) | (head[i+1]<<16) | (head[i+2]<<8) | (head[i+3]));
	uint32_t *temp = (uint32_t*)_buffer;
	setTimestamp(ntohl(*temp));
	temp++;
	
	//Extract the four axes from the byte array. Move temp along to point to the next one. It's incrementing
	//by it's size of uint32_t=4.
	setLX(ntohl(*temp++));
	setLY(ntohl(*temp++));
	setRX(ntohl(*temp++));
	setRY(ntohl(*temp++));
	
	// cout << "Received: [" << timestamp() << "] LX " << leftX() << ", LY " << leftY() << ", RX " << rightX() << ", RY " << rightY() << endl;

	
	return true;
	
}


// deserialize a GpControllerEvent object back into a class object
bool GpControllerEvent::deserializeToObjectCopy(GpControllerEvent &evtCopy, uint8_t * const & bytes){

	
	
	//cout << "Deserialize: \n";
	
	//e2.setTimestamp((head[i]<<24) | (head[i+1]<<16) | (head[i+2]<<8) | (head[i+3]));
	uint32_t *temp = (uint32_t*) bytes;
	evtCopy.setTimestamp(ntohl(*temp));
	temp++;
	
	//Extract the four axes from the byte array. Move temp along to point to the next one. It's incrementing
	//by it's size of uint32_t=4.
	evtCopy.setLX(ntohl(*temp++));
	evtCopy.setLY(ntohl(*temp++));
	evtCopy.setRX(ntohl(*temp++));
	evtCopy.setRY(ntohl(*temp++));

	
	return true;

}







