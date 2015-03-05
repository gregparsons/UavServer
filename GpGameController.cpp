
/**
 
 Listens for input events from a common USB video game controller. Requires the SDL2
 framework: /Library/Frameworks/SDL2.framework (build phases / link binary with libraries).
 
 
 
 References:
 Thanks for the basic how-to here:
 http://lazyfoo.net/tutorials/SDL/19_gamepads_and_joysticks/index.php
 
 
 */

#include <mavlink/c_library/common/mavlink.h>
#include <iostream>
#include <SDL2/SDL.h>

#include "GpGameController.h"
#include "GpMavlink.h"
#include "GpMessage.h"

using namespace std;

const int CONTROLLER_DEAD_ZONE = 5000;


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
 
void GpGameController::runGameController(GpControllerNetwork & controlNet){
	
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
									
									
									
									
									
									
									/*
									
									
									mavlink_message_t mavlinkMsg;
									bzero(&mavlinkMsg, sizeof(mavlink_message_t));
						
									GpMavlink::encodeControllerEventAsMavlink(axisValue[0], axisValue[1], axisValue[2], axisValue[3], e.caxis.timestamp, mavlinkMsg);	//fill channel message

									controlNet.sendTCP(mavlinkMsg);

									
									*/
									
									
									uint8_t message[] = "hello greg what's happening?";

									// Variable size buffer to hold message and message header
									uint16_t bufferSize = sizeof(message) + GP_MSG_HEADER_LEN;
									uint8_t buffer[bufferSize];
									bzero(buffer, sizeof(buffer));
									
									uint8_t *ptr = buffer;
									uint8_t *messageRef = message;
									
									uint16_t messageSize = sizeof(message);
									GpMessage gpMesg {2, messageSize, messageRef};	//create message based on the actual payload and payload size

									gpMesg.serialize(ptr, bufferSize);		//serialize to buffer
									
									GpMessage gpMesg2;
									gpMesg2.deserialize(ptr, bufferSize);
									
									
									
									
									
									/*
									//Fill in the GpControllerEvent class for packaging in prep for network serialization
									gpEvent.setLX(axisValue[0]);
									gpEvent.setLY(axisValue[1]);
									gpEvent.setRX(axisValue[2]);
									gpEvent.setRY(axisValue[3]);
									gpEvent.setTimestamp(e.caxis.timestamp);
									 */
								}

								
								// std::cout << "Sending: [" << gpEvent.timestamp() << "] LX " << gpEvent.leftX() << ", LY " << gpEvent.leftY() << ", RX " << gpEvent.rightX() << ", RY " << gpEvent.rightY() << std::endl;

								
								

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

