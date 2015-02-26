all:uav

uav:
	g++ -std=c++0x -I . -o uav GpUavServer.cpp GpUavAsset.cpp GpUavController.cpp main.cpp
#	g++ -std=c++0x GpControllerEvent.cpp GpNetworkTransmitter.cpp GpGameController.cpp main.cpp GpFlightController.cpp GpRouter.cpp -lSDL2 -o uav