
all:uav

uav:
	g++ -std=c++0x -I . -I ./lib/ -lSDL2 -o uav GpMessage_Login.cpp GpMessage.cpp GpDatabase.cpp GpControllerEvent.cpp GpGameController.cpp GpControllerNetwork.cpp GpMavlink.cpp GpUavServer.cpp GpUavAsset.cpp GpGroundController.cpp main.cpp

clean:
	rm uav