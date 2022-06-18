#include <exception>
#include <thread>
#include "pch.h"
#include "ddiclient.h"
#include "..\ddi\include\ddi.hpp"
#include "..\dps\include\ritms_dps.hpp"
#include "DPSInfoReloadHandler.hpp"

using namespace ritms::dps;
using namespace ddi;

namespace HkbClient {

    CallbackDispatcher* CreateDispatcher(callback_function callback)
    {
        auto dispatcher = new CallbackDispatcher(callback);
        return dispatcher;
    }

    void SetConfig(CallbackDispatcher* dispatcher, _KEYVALUEPAIR* keyvaluepairs, int size)
    {
        std::vector<KEYVALUEPAIR> configInfo;
        for(int i = 0; i < size; ++i)
        {
            configInfo.push_back(KEYVALUEPAIR { std::string(keyvaluepairs[i].key), std::string(keyvaluepairs[i].value) } );
        }
        dispatcher->SetConfig(configInfo);
    }

    void SetDownloadLocation(CallbackDispatcher* dispatcher, const char* location)
    {
        dispatcher->SetDownloadLocation(location);
    }

    void RunClient(const char* clientCertificatePath, const char* provisioningEndpoint, const char* xApigToken, CallbackDispatcher* dispatcher) {
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
            ->setEventHandler(std::shared_ptr<EventHandler>(dispatcher))
            ->build()
            ->run();

    }

    void ReleaseDispatcher(CallbackDispatcher* dispatcher)
    {
        delete dispatcher;
    }

}
