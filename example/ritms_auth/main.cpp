#include "ritms_dps.hpp"

#include <iostream>
#include <thread>
#include <fstream>

#define RAPIDJSON_HAS_STDSTRING 1

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"


const char *AUTH_CERT_PATH_ENV_NAME = "CERT_PATH";
const char *PROVISIONING_ENDPOINT_ENV_NAME = "PROVISIONING_ENDPOINT";
const char *X_APIG_TOKEN_ENV_NAME = "X_APIG_TOKEN";

const int RETRY_TIMEOUT = 500;
const int RETRY_ATTEMPTS = 5;

const char *MTLS_KEYPAIR_FIELD_NAME = "mTLSKeypair";
const char *MTLS_CERTIFICATE_FIELD_NAME = "certificate";
const char *MTLS_KEY_FIELD_NAME = "key";

const char *UP2DATE_ENDPOINT_FIELD_NAME = "up2DateEndpoint";


char *getEnvOrExit(const char *name) {
    auto env = std::getenv(name);
    if (env == nullptr) {
        std::cout << "Environment variable " << name << " not set" << std::endl;
        exit(2);
    }
    return env;
}

using namespace ritms::dps;

int main() {
    // Certificate Path
    auto clientCertificatePath = getEnvOrExit(AUTH_CERT_PATH_ENV_NAME);
    // Provisioning Endpoint
    auto provisioningEndpoint = getEnvOrExit(PROVISIONING_ENDPOINT_ENV_NAME);
    // Magic header for cloud
    auto xApigToken = getEnvOrExit(X_APIG_TOKEN_ENV_NAME);

    std::ifstream t((std::string(clientCertificatePath)));
    if (!t.is_open()) {
        std::cout << "File " << clientCertificatePath << " not exists" << std::endl;
        throw std::runtime_error(std::string("fail: cannot open file :").append(clientCertificatePath));
    }
    std::string crt((std::istreambuf_iterator<char>(t)),
                    std::istreambuf_iterator<char>());

    auto dpsBuilder = CloudProvisioningClientBuilder::newInstance();
    auto dpsClient = dpsBuilder->setEndpoint(provisioningEndpoint)
            ->setAuthCrt(crt)
            ->addHeader("X-Apig-AppCode", std::string(xApigToken))
            ->build();

    std::unique_ptr<ProvisioningData> payload = nullptr;
    for (int i = 0; i<RETRY_ATTEMPTS; ++i) {
        try {
            payload = dpsClient->doProvisioning();
            break;
        }
        catch (std::exception &e) {
            std::cerr << "Provisioning error: " << e.what() << " still trying... Attempt [" <<
                      i+1 << "/" << RETRY_ATTEMPTS << "]"<< std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(RETRY_TIMEOUT));
        }
    }

    if (payload == nullptr) {
        std::cerr << "Error. Cannot do provisioning. Exiting..." << std::endl;
        exit(102);
    }

    // Format provisioning output
    rapidjson::Document document;
    document.SetObject();

    // add mTLSKeypair block
    rapidjson::Value mTLSKeypair(rapidjson::kObjectType);
    auto keyPair = payload->getKeyPair();
    mTLSKeypair.AddMember(rapidjson::StringRef(MTLS_CERTIFICATE_FIELD_NAME), keyPair->getCrt(), document.GetAllocator());
    mTLSKeypair.AddMember(rapidjson::StringRef(MTLS_KEY_FIELD_NAME), keyPair->getKey(), document.GetAllocator());
    document.AddMember(rapidjson::StringRef(MTLS_KEYPAIR_FIELD_NAME), mTLSKeypair, document.GetAllocator());

    // add up2dateEndpoint
    document.AddMember(rapidjson::StringRef(UP2DATE_ENDPOINT_FIELD_NAME), payload->getUp2DateEndpoint(), document.GetAllocator());

    // print JSON (unix-way)
    rapidjson::StringBuffer buf;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buf);
    document.Accept(writer);
    std::cout << buf.GetString() << std::endl;
}