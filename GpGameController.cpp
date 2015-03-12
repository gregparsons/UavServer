//
//  GpGameController.cpp
//
//
//
// Connects to USB game controller and captures button/joystick events.
//

/**
 
 Listens for input events from a common USB video game. Requires the SDL2
 framework: /Library/Frameworks/SDL2.framework (build phases / link binary with libraries).
 
 
 References:
 http://lazyfoo.net/tutorials/SDL/19_gamepads_and_joysticks/index.php
 
 */

#include <mavlink/c_library/common/mavlink.h>
#include <iostream>
#include <unistd.h>
#include <thread>

#include "SDL2/SDL.h"

#include "GpGameController.h"
#include "GpMavlink.h"
#include "GpMessage.h"
#include "GpClientNet.h"



using namespace std;

const int CONTROLLER_DEAD_ZONE = 5000;

GpGameController::GpGameController(){


}


void GpGameController::startGameControllerThread(GpClientNet & net){
	
	_net = &net;
	
	std::thread game_controller_thread(&GpGameController::_controller_thread, this);
	game_controller_thread.detach();
	
}




void GpGameController::_controller_thread(){
	
	
	GpClientNet & net = *_net;
	
	SDL_GameController *gameController = nullptr;


	if(initSDL(&gameController) == true){

		bool quit = false;
		SDL_Event e;
		int newVal = 0;
		
		//0 = leftx, 1=lefty, 2=right x 3=right y
		int axisValue[4];
		for(int i=0; i<4;i++){
			axisValue[i]=0;
		}
		
		
		if(gameController != nullptr){
			
			// GpControllerEvent gpEvent;	//init a blank event
			
			while(!quit){
				
				while(SDL_PollEvent(&e)>=0){
					
					//user requests quit
					switch (e.type) {
						case SDL_QUIT:
							quit = true;
							break;
						case SDL_JOYAXISMOTION:
						{
							//if it's changing, print an event, if it doesn't change, don't fill up the screen
							//if it's less than X000, call it a zero
							
							
							// (1)Make sure not to overrun the axis array with some weird axis > 3
							if(e.caxis.axis < 4){
								if(abs(e.caxis.value) > CONTROLLER_DEAD_ZONE){
									newVal = e.caxis.value;	//only change the axis that gets changed
								}
								else{
									newVal = 0;
								}
								
								//Only continue if the new value is different from the old value. Otherwise, don't print if no change.
								if(axisValue[e.caxis.axis] != newVal){
									axisValue[e.caxis.axis] = newVal;
									
									mavlink_message_t mavlinkMsg;
									bzero(&mavlinkMsg, sizeof(mavlink_message_t));
									
									uint16_t msgLen = 0;
									GpMavlink::encodeControllerEventAsMavlink(axisValue[0], axisValue[1], axisValue[2], axisValue[3], e.caxis.timestamp, mavlinkMsg, msgLen);	//fill channel message
									
									
									if(msgLen == 0){
										
										std::cout << "[" << __func__ << "] "  << "Mavlink encoded payload was size 0" << std::endl;
										break;
									}
									
									
									
									
									uint8_t *ptr = reinterpret_cast<uint8_t*>(&mavlinkMsg);
									GpMessage cmdMessage(GP_MSG_TYPE_COMMAND, msgLen, ptr);
									if(net._isConnected){


										// This is the heart of this class, the controller output is converted to
										// a MAVLink payload in a GpMessage
										// and sent to the server for forwarding to the asset.
										
										
										GpMavlink::printMavFromGpMessage(cmdMessage);
										net.sendMessage(cmdMessage);

									
									}else{
										std::cout << "[" << __func__ << "] "  << "Can't send message. Net is offline." << std::endl;

									}
									
									
								}
							}
							break;
						}
						default:
							break;
							
					}
				}
			}
			SDL_GameControllerClose(gameController);
		}
		SDL_Quit();
		
	}
	if(gameController != nullptr){
		SDL_GameControllerClose(gameController);
		SDL_Quit();
	}
}



/**
 *  Init SDL for game controller
 *
 */
bool GpGameController::initSDL(SDL_GameController **gameController){
	
	bool success = true;
	//init SDL
	if(SDL_Init(SDL_INIT_GAMECONTROLLER) < 0){
		std::cout << "Could not init SDL with controller module: " << SDL_GetError() << std::endl;
		success = false;
	}
	else{
		
		if(SDL_NumJoysticks() < 1){
			cout << "No controllers connected." << endl;
		}
		else{
			cout << SDL_NumJoysticks() << " joysticks/controllers connected." << endl;
			
			//Load first game controller
			*gameController = SDL_GameControllerOpen(0);
			if(*gameController == nullptr){
				cout << "Couldn't open controller: " << SDL_GetError() << std::endl;
			}
			else{
				cout << "Controller 0 open." << endl;
			}
		}
	}
	return success;
}


/**
 Start listening to USB game controller. Serialize and transmit input events.
 
 
 */
 /*
void GpGameController::runGameController(GpClientNet & net){
	
	SDL_GameController *gameController = nullptr;

	
	if(initSDL(&gameController) == true){
		
		
		bool quit = false;
		SDL_Event e;
		int newVal = 0;
		
		//0 = leftx, 1=lefty, 2=right x 3=right y
		int axisValue[4];
		for(int i=0; i<4;i++){
			axisValue[i]=0;
		}
		
		
		if(gameController != nullptr){
			
			// GpControllerEvent gpEvent;	//init a blank event
			
			while(!quit){
				
				while(SDL_PollEvent(&e)>=0){
					
					//user requests quit
					switch (e.type) {
						case SDL_QUIT:
							quit = true;
							break;
						case SDL_JOYAXISMOTION:
						{
							//if it's changing, print an event, if it doesn't change, don't fill up the screen
							//if it's less than X000, call it a zero
							
							
							// (1)Make sure not to overrun the axis array with some weird axis > 3
							if(e.caxis.axis < 4){
								if(abs(e.caxis.value) > CONTROLLER_DEAD_ZONE){
									newVal = e.caxis.value;	//only change the axis that gets changed
								}
								else{
									newVal = 0;
								}
								
								//Only continue if the new value is different from the old value. Otherwise, don't print if no change.
								if(axisValue[e.caxis.axis] != newVal){
									axisValue[e.caxis.axis] = newVal;
									
									mavlink_message_t mavlinkMsg;
									bzero(&mavlinkMsg, sizeof(mavlink_message_t));
						
									uint16_t msgLen = 0;
									GpMavlink::encodeControllerEventAsMavlink(axisValue[0], axisValue[1], axisValue[2], axisValue[3], e.caxis.timestamp, mavlinkMsg, msgLen);	//fill channel message


									if(msgLen == 0){
										
										std::cout << "[" << __func__ << "] "  << "mavlink encoded message was size 0" << std::endl;
										break;
									}
										


									// GpMessage(uint8_t messageType, uint16_t payloadSize, uint8_t *&payload);
									uint8_t *ptr = reinterpret_cast<uint8_t*>(&mavlinkMsg);
 									GpMessage cmdMessage(GP_MSG_TYPE_COMMAND, msgLen, ptr);
									
									
									
									net.sendMessage(cmdMessage);

								
								}
							}
							break;
						}
						default:
							break;
							
					}
				}
			}
			SDL_GameControllerClose(gameController);
		}
		SDL_Quit();
	}
}
*/
