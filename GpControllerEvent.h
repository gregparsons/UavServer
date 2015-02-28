//
//  GpControllerEvent.h
//  controllerHello
//
//  Created by gp on 2/2/15.
//  Copyright (c) 2015 swimr. All rights reserved.
//

#ifndef __controllerHello__GpControllerEvent__
#define __controllerHello__GpControllerEvent__


#include <SDL2/SDL.h>

class GpControllerEvent{
public:

	//Constructors
	GpControllerEvent();
	
	
	//Received event from raw
	GpControllerEvent(uint8_t* & buf, int i);
	
	
	GpControllerEvent(int lx, int ly, int rx, int ry, Uint32 ts);
	
	
	
	//Getters
	int leftX();
	int leftY();
	int rightX();
	int rightY();
	Uint32 timestamp();
	SDL_JoystickID id();
	
	//Setters
	void setLX(int lx);
	void setLY(int ly);
	void setRX(int rx);
	void setRY(int ry);
	void setTimestamp(Uint32 ts);
	//void setSDL_JoystickID(SDL_JoystickID id);
	
	//Serialize this instance for network transmission
	bool serializeAsNetworkOrderBytes(uint8_t * &bytes, int &size);

	bool deserializeToObjectCopy();

	bool deserializeToObjectCopy(GpControllerEvent & evtCopy, uint8_t * const & bytes);
	
	//need a deserialize constructor
	
private:
	
	//Based on http://wiki.libsdl.org/SDL_ControllerAxisEvent
	Uint32 _timestamp = 0;

	//current axis values
	int _leftX=0, _leftY=0, _rightX=0, _rightY=0;

	uint8_t *_buffer = nullptr;
	ssize_t _bufferSize = 0;
	
};


#endif /* defined(__controllerHello__GpControllerEvent__) */
