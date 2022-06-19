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

    CallbackDispatcher* CreateDispatcher(
        ConfigRequestCallbackFunction configRequest, 
        DeploymentActionCallbackFunction deploymentAction, 
        CancelActionCallbackFunction cancelAction)
    {
        auto dispatcher = new CallbackDispatcher(configRequest, deploymentAction, cancelAction);
        return dispatcher;
    }

    void AddConfigAttribute(ddi::ConfigResponseBuilder* responseBuilder, const char* key, const char* value)
    {
        responseBuilder->addData(key, value);
    }

    void DownloadArtifact(ddi::Artifact* artifact, const char* location)
    {
        artifact->downloadTo(location + artifact->getFilename());
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
