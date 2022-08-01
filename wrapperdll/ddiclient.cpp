#include <exception>
#include <thread>
#include "pch.h"
#include "ddiclient.h"
#include "ddi.hpp"
#include "ritms_dps.hpp"
#include "DPSInfoReloadHandler.hpp"

namespace HkbClient {

    CallbackDispatcher* CreateDispatcher(
        ConfigRequestCallbackFunction configRequest, 
        DeploymentActionCallbackFunction deploymentAction, 
        CancelActionCallbackFunction cancelAction) {
        auto dispatcher = new CallbackDispatcher(configRequest, deploymentAction, cancelAction);
        return dispatcher;
    }

    void AddConfigAttribute(ddi::ConfigResponseBuilder* responseBuilder, const char* key, const char* value) {
        responseBuilder->addData(key, value);
    }

    void DownloadArtifact(ddi::Artifact* artifact, const char* location) {
        artifact->downloadTo(location + artifact->getFilename());
    }

    void RunClient(const char* clientCertificate, const char* provisioningEndpoint, const char* xApigToken, CallbackDispatcher* dispatcher, AuthErrorCallbackFunction authErrorAction) {
        auto dpsBuilder = CloudProvisioningClientBuilder::newInstance();
        auto dpsClient = dpsBuilder->setEndpoint(provisioningEndpoint)
            ->setAuthCrt(clientCertificate)
            ->addHeader("X-Apig-AppCode", std::string(xApigToken))
            ->build();

        auto authErrorHandler = std::shared_ptr<AuthErrorHandler>(new DPSInfoReloadHandler(std::move(dpsClient), authErrorAction));


        auto builder = DDIClientBuilder::newInstance();

        builder->setAuthErrorHandler(authErrorHandler)
            ->setEventHandler(std::shared_ptr<EventHandler>(dispatcher))
            ->build()
            ->run();

    }

    void DeleteDispatcher(CallbackDispatcher* dispatcher) {
        delete dispatcher;
    }

}
