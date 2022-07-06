#define USE_DEFAULT_HTTPLIB
#include "basic_handler.hpp"

const char *GATEWAY_TOKEN_ENV_NAME = "GATEWAY_TOKEN";
const char *HAWKBIT_ENDPOINT_ENV_NAME = "HAWKBIT_ENDPOINT";
const char *CONTROLLER_ID_ENV_NAME = "CONTROLLER_ID";

int main() {
    std::cout << "simple hawkBit-cpp client started..." << std::endl;

    auto gatewayToken = getEnvOrExit(GATEWAY_TOKEN_ENV_NAME);
    auto hawkbitEndpoint = getEnvOrExit(HAWKBIT_ENDPOINT_ENV_NAME);
    auto controllerId = getEnvOrExit(CONTROLLER_ID_ENV_NAME);


    auto builder = DDIClientBuilder::newInstance();
    builder->setHawkbitEndpoint(hawkbitEndpoint, controllerId)
        ->setGatewayToken(gatewayToken)
        ->setEventHandler(std::shared_ptr<EventHandler>(new Handler()))
        ->notVerifyServerCertificate()
        ->build()
        ->run();
}