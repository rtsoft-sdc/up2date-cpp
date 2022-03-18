#include "basic_handler.hpp"
#include "ritms_dps.hpp"
#include <exception>
#include <thread>

const char *AUTH_CERT_PATH_ENV_NAME = "CERT_PATH";
const char *PROVISIONING_ENDPOINT_ENV_NAME = "PROVISIONING_ENDPOINT";
const char *X_APIG_TOKEN_ENV_NAME = "X_APIG_TOKEN";

using namespace ritms::dps;
using namespace ddi;

class DPSInfoReloadHandler : public AuthErrorHandler {
    std::unique_ptr<ProvisioningClient> client;

public:
    explicit DPSInfoReloadHandler(std::unique_ptr<ProvisioningClient> client_) : client(std::move(client_)) {};

    void onAuthError(std::unique_ptr<AuthRestoreHandler> ptr) override {
        for (;;) {
            try {
                std::cout << "==============================================" << std::endl;
                std::cout << "|DPSInfoReloadHandler| Starting provisioning..." << std::endl;
                auto payload = client->doProvisioning();
                std::cout << "|DPSInfoReloadHandler|     ... done" << std::endl;
                auto keyPair = payload->getKeyPair();
                std::cout << "|DPSInfoReloadHandler| Setting TLS ..." << std::endl;
                ptr->setTLS(keyPair->getCrt(), keyPair->getKey());
                std::cout << "|DPSInfoReloadHandler| Setting endpoint [" << payload->getUp2DateEndpoint() << "] ..." << std::endl;
                ptr->setEndpoint(payload->getUp2DateEndpoint());
                std::cout << "==============================================" << std::endl;
                return;
            }
            catch (std::exception &e) {
                std::cout << "provisioning error: " << e.what() << " still trying..." << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(3000));
            }
        }
    }
};

int main() {
    std::cout << "up2date hawkBit-cpp client started..." << std::endl;

    auto clientCertificatePath = getEnvOrExit(AUTH_CERT_PATH_ENV_NAME);
    auto provisioningEndpoint = getEnvOrExit(PROVISIONING_ENDPOINT_ENV_NAME);
    // special variable for cloud
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

    auto authErrorHandler = std::shared_ptr<AuthErrorHandler>(new DPSInfoReloadHandler(std::move(dpsClient)));


    auto builder = DDIClientBuilder::newInstance();
    builder->setAuthErrorHandler(authErrorHandler)
        ->setEventHandler(std::shared_ptr<EventHandler>(new Handler()))
        ->build()
        ->run();
}