
all:uav

uav:
	g++ -std=c++0x -I . -lSDL2 -o uav GpControllerEvent.cpp GpGameController.cpp GpControllerNetwork.cpp GpMavlink.cpp GpUavServer.cpp GpUavAsset.cpp GpGroundController.cpp main.cpp

clean:
	rm uav