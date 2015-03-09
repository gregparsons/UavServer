//
//  GpGameController.h
//  controllerHello
//
//  Created by gp on 2/2/15.
//  Copyright (c) 2015 swimr. All rights reserved.
//

#ifndef __controllerHello__GpGameController__
#define __controllerHello__GpGameController__

#include "SDL2/SDL.h"

#include "GpControllerNetwork.h"

class GpGameController{
public:
	
	void runGameController(GpClientNet & controlNet);
	
	
private:
	
	bool initSDL(SDL_GameController **gameController);
	
	
	//GameController Unique ID?
	
};
#endif /* defined(__controllerHello__GpGameController__) */
