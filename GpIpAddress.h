
#ifndef __controllerHello__GpIpAddress__
#define __controllerHello__GpIpAddress__

//#define GP_FLY_IP_ADDRESS "127.0.0.1"
//#define GP_FLY_IP_ADDRESS "192.168.113.90"
//#define GP_CONTROL_PORT "11222"

// const std::string GP_CONTROLLER_SERVER_IP = "127.0.0.1";

const std::string GP_CONTROLLER_SERVER_IP = "192.168.113.240";
// const std::string GP_CONTROLLER_SERVER_IP = "172.26.48.1";
const std::string GP_CONTROLLER_SERVER_PORT = "11222";

const std::string GP_CONTROLLER_TEST_USERNAME = "MyLongUserName";
const std::string GP_CONTROLLER_TEST_PASSWORD = "My really long password sentence could be an HMAC.";


#define MAX_CONNECTION_BACKLOG 10
#define READ_VECTOR_BYTES_MAX 300


#endif