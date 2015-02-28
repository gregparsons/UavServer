//
//  GpControllerEvent.cpp
//  controllerHello
//
//  Created by gp on 2/2/15.
//  Copyright (c) 2015 swimr. All rights reserved.
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

	
	
/*
		int ts = timestamp();
		cout << "1(host): " << ts << ": " << (bitset<32>)ts << endl;
		cout << "1(net) : " << ts << ": " << (bitset<32>)htonl(ts) << endl;
*/
	
	// Timestamp
	// to network order then to byte array
	*data++ = timestamp() >>24;
	*data++ = timestamp()>>16;
	*data++ = timestamp()>>8;
	*data++ = timestamp();
	//ptr+=4;
	
/*		//Test after
		int ts2 = (ptr[0]<<24) | (ptr[1]<<16) | (ptr[2]<<8) | (ptr[3]);
		cout << "2(host): " << ts2 << ": " << (bitset<32>)ts2 << endl;
		cout << "2(net) : " << ts2 << ": " << (bitset<32>)htonl(ts2) << endl;
		ptr+=4;
*/
	
	// Event Left X
	*data++ = leftX() >>24;
	*data++ = leftX()>>16;
	*data++ = leftX()>>8;
	*data++ = leftX();
	//ptr+=4;

	
	
	/*
	
		//Test before
		int ts = leftY();
		cout << "LY1(host): " << ts << ": " << (bitset<32>)ts << endl;
		cout << "LY1(net) : " << ts << ": " << (bitset<32>)htonl(ts) << endl;
	*/
	
	// Event Left Y
	*data++ = leftY() >>24;
	*data++ = leftY() >>16;
	*data++ = leftY() >>8;
	*data++ = leftY();
	
	/*
		//Test after
		int ts2 = (ptr[0]<<24) | (ptr[1]<<16) | (ptr[2]<<8) | (ptr[3]);
		cout << "LY2(host): " << ts2 << ": " << (bitset<32>)ts2 << endl;
		cout << "LY2(net) : " << ts2 << ": " << (bitset<32>)htonl(ts2) << endl;
	ptr+=4;
	
	*/

		
		
	
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
	
	cout << "Received: [" << timestamp() << "] LX " << leftX() << ", LY " << leftY() << ", RX " << rightX() << ", RY " << rightY() << endl;

	//cout << "TS: " << evtCopy.timestamp() << ": " << (bitset<32>)evtCopy.timestamp() << endl;
	//cout << "LX: "<< evtCopy.leftX() << ": " << (bitset<32>)evtCopy.leftX() << endl;
	//cout << "LY: "<< evtCopy.leftY() << ": " << (bitset<32>)evtCopy.leftY() << endl;
	//cout << "RX: "<< evtCopy.rightX() << ": " << (bitset<32>)evtCopy.rightX() << endl;
	//cout << "RY: "<< evtCopy.rightY() << ": " << (bitset<32>)evtCopy.rightY() << endl;
	
	
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
	
	//cout << "TS: " << evtCopy.timestamp() << ": " << (bitset<32>)evtCopy.timestamp() << endl;
	//cout << "LX: "<< evtCopy.leftX() << ": " << (bitset<32>)evtCopy.leftX() << endl;
	//cout << "LY: "<< evtCopy.leftY() << ": " << (bitset<32>)evtCopy.leftY() << endl;
	//cout << "RX: "<< evtCopy.rightX() << ": " << (bitset<32>)evtCopy.rightX() << endl;
	//cout << "RY: "<< evtCopy.rightY() << ": " << (bitset<32>)evtCopy.rightY() << endl;

	
	return true;

}







