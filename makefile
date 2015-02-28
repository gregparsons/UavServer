
all:uav

uav:
	g++ -std=c++0x -I . -lSDL2 -o uav GpControllerEvent.cpp GpGameController.cpp GpControllerNetwork.cpp  GpNetworkTransmitter.cpp GpMavlink.cpp GpUavServer.cpp GpUavAsset.cpp GpUavController.cpp main.cpp

clean:
	rm uav