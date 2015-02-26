// ********************************************************************************
//
//  GpUavAsset.h
//  UavServer
//  2/25/15
//
// ********************************************************************************



#include <mavlink/c_library/common/mavlink.h>
#include <iostream>
#include <unistd.h>		//usleep

#include "GpUavController.h"
#include "GpMavlink.h"

#include "GpIpAddress.h"

using namespace std;


void sendMavlinkMessage(mavlink_message_t & msg){
	
	cout << "GpUavController::sendMavlinkMessage()" << endl;
	
	GpMavlink gpMavlink;

	for(;;){
		gpMavlink.sendTestMessage();
		usleep(500000);
	}
}







bool GpUavController::start(){
	
	cout << "GpUavController::start()" << endl;
	
	
	mavlink_message_t mesg;
	sendMavlinkMessage(mesg);
	
	
	
	
	return true;
}