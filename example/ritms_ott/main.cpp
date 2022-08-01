#include "ritms_token_exchanger.hpp"
#include <iostream>

const char *EXCHANGER_ENDPOINT_ENV_NAME = "EXCHANGER_ENDPOINT";
const char *TOKEN_ENV_NAME = "TOKEN";

using namespace ritms::dps;

char *getEnvOrExit(const char *name) {
    auto env = std::getenv(name);
    if (env == nullptr) {
        std::cout << "Environment variable " << name << " not set" << std::endl;
        exit(2);
    }
    return env;
}


int main() {
    std::string token = getEnvOrExit(TOKEN_ENV_NAME);
    std::string exchangerEndpoint = getEnvOrExit(EXCHANGER_ENDPOINT_ENV_NAME);
    try {
        std::cout <<
            TokenExchangerFactory::buildCloudExchanger(exchangerEndpoint, token)
                    ->exchange();
    } catch (std::exception &e) {
        std::cout << "Error exchanging token: " << e.what() << std::endl;
        exit(3);
    }
}