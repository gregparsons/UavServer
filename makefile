
all:uav

uav:
	g++ -std=c++0x -I . -o uav GpNetworkTransmitter.cpp GpMavlink.cpp GpUavServer.cpp GpUavAsset.cpp GpUavController.cpp main.cpp
#	g++ -std=c++0x GpControllerEvent.cpp GpNetworkTransmitter.cpp GpGameController.cpp main.cpp GpFlightController.cpp GpRouter.cpp -lSDL2 -o uav

clean:
	rm uav