// ********************************************************************************
//
//  GpUavAsset.h
//  UavServer
//  2/25/15
//
// ********************************************************************************



#include <mavlink/c_library/common/mavlink.h>
#include <iostream>

#include "GpUavController.h"

using namespace std;


void sendMavlinkMessage(mavlink_message_t & msg){
	
	
	cout << "GpUavController::sendMavlinkMessage()" << endl;
	
	
	
	
	
	
}







bool GpUavController::start(){
	
	cout << "GpUavController::start()" << endl;
	
	
	mavlink_message_t mesg;
	sendMavlinkMessage(mesg);
	
	
	
	
	return true;
}