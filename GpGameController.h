//
//  GpGameController.h
//  controllerHello
//
//  Created by gp on 2/2/15.
//  Copyright (c) 2015 swimr. All rights reserved.
//

#ifndef __controllerHello__GpGameController__
#define __controllerHello__GpGameController__

#include "GpClientNet.h"	//can only forward declare pointers and refs
#include "SDL2/SDL.h"

// #include "GpControllerNetwork.h"

class GpClientNet;

class GpGameController{
public:
	
	void startGameControllerThread(GpClientNet & net);
	
	
private:
	
	// void runGameController(GpClientNet & net);
	
	
	bool initSDL(SDL_GameController **gameController);

	
	
	
	
	void _controller_thread();
	
	
	GpClientNet *_net = nullptr;
	
};
#endif /* defined(__controllerHello__GpGameController__) */
