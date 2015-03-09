
#ifndef __controllerHello__GpIpAddress__
#define __controllerHello__GpIpAddress__

//#define GP_FLY_IP_ADDRESS "127.0.0.1"
//#define GP_FLY_IP_ADDRESS "192.168.113.90"
//#define GP_CONTROL_PORT "11222"

// const std::string GP_CONTROLLER_SERVER_IP = "127.0.0.1";

//const std::string GP_CONTROLLER_SERVER_IP = "192.168.113.90";
// const std::string GP_CONTROLLER_SERVER_IP = "172.26.48.1";

const std::string GP_CONTROLLER_SERVER_IP = "127.0.0.1";
const std::string GP_CONTROLLER_SERVER_PORT = "11222";
const std::string GP_ASSET_SERVER_PORT = "11223";




#define MAX_CONNECTION_BACKLOG 10
#define READ_VECTOR_BYTES_MAX 300



// Debug stuff:

#define GP_ASSET_ID_TEST_ONLY 12345
const bool GP_SHOULD_SEND_HEARTBEAT_SERVER_TO_CONTROLLER = false;
const bool GP_SHOULD_SEND_HEARTBEAT_TO_SERVER_FROM_ASSET= true;

const std::string GP_CONTROLLER_TEST_USERNAME = "MyLongUserName";
const std::string GP_CONTROLLER_TEST_PASSWORD = "My really long password sentence could be an HMAC.";

const std::string GP_ASSET_TEST_USERNAME = "Asset123456";
const std::string GP_ASSET_TEST_PASSWORD = "My really long password sentence could be an HMAC.";

// #define GP_SHOULD_INSERT_FAKE_ASSET



#endif