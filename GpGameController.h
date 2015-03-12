//
//  GpGameController.h
//
//
//	// Connects to USB game controller and captures button/joystick events.
//

#ifndef __controllerHello__GpGameController__
#define __controllerHello__GpGameController__

#include "GpClientNet.h"	//can only forward declare pointers and refs
#include "SDL2/SDL.h"

// #include "GpControllerNetwork.h"

class GpClientNet;

class GpGameController{
public:
	GpGameController();
	
	void startGameControllerThread(GpClientNet & net);
	
	
private:
	
	// void runGameController(GpClientNet & net);
	
	
	bool initSDL(SDL_GameController **gameController);

	
	
	
	
	void _controller_thread();
	
	
	GpClientNet *_net = nullptr;
	
};
#endif /* defined(__controllerHello__GpGameController__) */
