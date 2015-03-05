
all:uav

uav:
	g++ -std=c++0x -I . -I ./lib/ -I/usr/local/include/SDL2 -D_REENTRANT -L/usr/local/lib -Wl,-rpath,/usr/local/lib -lSDL2 -lpthread -o uav GpMessage_Login.cpp GpMessage.cpp GpDatabase.cpp GpControllerEvent.cpp GpGameController.cpp GpControllerNetwork.cpp GpMavlink.cpp GpUavServer.cpp GpUavAsset.cpp GpGroundController.cpp main.cpp

clean:
	rm uav

# -I/usr/local/include/SDL2 -D_REENTRANT -L/usr/local/lib -Wl,-rpath,/usr/local/lib -lSDL2 -lpthread

# For SDL2 params> sdl2-config --cflags --libs
# -L/usr/local/lib -Wl,-rpath,/usr/local/lib -lSDL2 -lpthread
