#pragma once

#include <iostream>
#include <thread>
#include "ritms_dps.hpp"
#include "ddi.hpp"

using namespace ritms::dps;
using namespace ddi;

namespace HkbClient {

    class DPSInfoReloadHandler : public AuthErrorHandler {
        std::unique_ptr<ProvisioningClient> client;
        AuthErrorCallbackFunction authErrorAction;

    public:
        explicit DPSInfoReloadHandler(std::unique_ptr<ProvisioningClient> client_, AuthErrorCallbackFunction authErrorAction_) : client(std::move(client_)) {
            authErrorAction = authErrorAction_;
        };

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
                    authErrorAction("");
                    return;
                }
                catch (std::exception &e) {
                    std::cout << "provisioning error: " << e.what() << " still trying..." << std::endl;
                    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
                    authErrorAction(e.what());
                }
            }
        }
    };

}
