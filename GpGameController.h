//
//  GpGameController.h
//  controllerHello
//
//  Created by gp on 2/2/15.
//  Copyright (c) 2015 swimr. All rights reserved.
//

#ifndef __controllerHello__GpGameController__
#define __controllerHello__GpGameController__


// #include </Users/aaa/176b/SDL/SDL2-2.0.3/include/SDL.h>
#include <SDL2/SDL.h>


class GpGameController{
public:
	static void runGameController();
private:
	static bool initSDL(SDL_GameController **gameController);
	
	
	//GameController Unique ID?
	
};
#endif /* defined(__controllerHello__GpGameController__) */
