
#ifndef __controllerHello__GpIpAddress__
#define __controllerHello__GpIpAddress__


// Server IP Address and Port

//const std::string GP_CONTROLLER_SERVER_IP = "192.168.113.240";
//const std::string GP_CONTROLLER_SERVER_IP = "172.26.48.1";
// const std::string GP_CONTROLLER_SERVER_IP = "52.11.30.229";
// const std::string GP_CONTROLLER_SERVER_IP = "127.0.0.1";


//AWS
// const std::string GP_CONTROLLER_SERVER_IP = "52.64.12.25";		// Sydney, Australia
const std::string GP_CONTROLLER_SERVER_IP = "52.1.73.170";			// Virginia


const std::string GP_CONTROLLER_SERVER_PORT = "11222";


#define MAX_CONNECTION_BACKLOG 10
#define READ_VECTOR_BYTES_MAX 300





// Test Bench User and Password

// (Simulated Database will match these two as a previously paired owner-asset
// relationship. This is in lieu of the full web signup/login site and actual database.

const std::string GP_CONTROLLER_TEST_USERNAME = "MyLongUserName";
const std::string GP_CONTROLLER_TEST_PASSWORD = "My really long password sentence could be an HMAC.";

const std::string GP_ASSET_TEST_USERNAME = "Asset123456";
const std::string GP_ASSET_TEST_PASSWORD = "My really long password sentence could be an HMAC.";
#define GP_ASSET_ID_TEST_ONLY 12345 // this would be a unique id assigned by the database module




// Test Bench:

const bool GP_INSTRUMENTATION_ON = true;

const bool GP_SHOULD_SEND_HEARTBEAT_SERVER_TO_CONTROLLER = false;
const bool GP_SHOULD_SEND_HEARTBEAT_TO_SERVER_FROM_ASSET= true;


#endif