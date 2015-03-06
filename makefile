
all:uav

uav:mac

mac:
	g++ -std=c++0x -I . -I ./lib/ -I/usr/local/include/SDL2 -D_REENTRANT -L/usr/local/lib -Wl,-rpath,/usr/local/lib -o uav GpMessage_Login.cpp GpMessage.cpp GpDatabase.cpp GpControllerEvent.cpp GpGameController.cpp GpControllerNetwork.cpp GpMavlink.cpp GpUavServer.cpp GpUavAsset.cpp GpGroundController.cpp main.cpp -lpthread -lSDL2

#ubuntu 12
ubuntu:
	clang++-3.5 -std=c++11 -stdlib=libc++ -I . -I ./lib/ -I/usr/local/include/SDL2 -D_REENTRANT -L/usr/local/lib -Wl,-rpath,/usr/local/lib -o uav GpMessage_Login.cpp GpMessage.cpp GpDatabase.cpp GpControllerEvent.cpp GpGameController.cpp GpControllerNetwork.cpp GpMavlink.cpp GpUavServer.cpp GpUavAsset.cpp GpGroundController.cpp main.cpp -lpthread -lSDL2


pi:
	g++-4.9 -std=c++0x -I . -I ./lib/ -I/usr/local/include/SDL2 -D_REENTRANT -L/usr/local/lib -Wl,-rpath,/usr/local/lib -o uav GpMessage_Login.cpp GpMessage.cpp GpDatabase.cpp GpControllerEvent.cpp GpGameController.cpp GpControllerNetwork.cpp GpMavlink.cpp GpUavServer.cpp GpUavAsset.cpp GpGroundController.cpp main.cpp -lpthread -lSDL2


# lib/SDL2.framework/Headers

clean:
	rm uav


# For SDL2 params> sdl2-config --cflags --libs

# ubuntu
# -I/usr/local/include/SDL2 -D_REENTRANT -L/usr/local/lib -Wl,-rpath,/usr/local/lib -lSDL2 -lpthread

# mac
# -I/usr/local/include/SDL2 -I/usr/X11/include -D_THREAD_SAFE -L/usr/local/lib -lSDL2