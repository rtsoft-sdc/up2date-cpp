#pragma once

#include <iostream>
#include <fstream>

#include "ddi.hpp"

using namespace ddi;

namespace HkbClient {

    typedef struct
    {
        int id;
        const char* updateType;
        const char* downloadType;
        bool        isInMaintenanceWindow;
        const char* chunkPart;
        const char* chunkName;
        const char* chunkVersion;
        const char* artifactFileName;
        const char* artifactFileHashMd5;
        const char* artifactFileHashSha1;
        const char* artifactFileHashSha256;
    } _DEPLOYMENTINFO;

    typedef struct
    {
        int         id;
        std::string updateType;
        std::string downloadType;
        bool        isInMaintenanceWindow;
        std::string chunkPart;
        std::string chunkName;
        std::string chunkVersion;
        std::string artifactFileName;
        std::string artifactFileHashMd5;
        std::string artifactFileHashSha1;
        std::string artifactFileHashSha256;
    } DEPLOYMENTINFO;

    typedef void (__stdcall *ConfigRequestCallbackFunction)(ddi::ConfigResponseBuilder* responseBuilder);
    typedef bool (__stdcall *DeploymentActionCallbackFunction)(ddi::Artifact* artifact, _DEPLOYMENTINFO info);
    typedef bool (__stdcall *CancelActionCallbackFunction)(int stopId);

    typedef struct 
    {
        char* key;
        char* value;
    } _KEYVALUEPAIR;

    typedef struct 
    {
        std::string key;
        std::string value;
    } KEYVALUEPAIR;

    class CancelActionFeedbackDeliveryListener : public ResponseDeliveryListener {
    public:
        void onSuccessfulDelivery() override {
            std::cout << ">> Successful delivered cancelAction response" << std::endl;
        }

        void onError() override {
            std::cout << ">> Error delivery cancelAction response" << std::endl;
        }
    };

    class DeploymentBaseFeedbackDeliveryListener : public ResponseDeliveryListener {
    public:
        void onSuccessfulDelivery() override {
            std::cout << ">> Successful delivered deliveryAction response" << std::endl;
        }

        void onError() override {
            std::cout << ">> Error delivery deliveryAction response" << std::endl;
        }
    };

    class CallbackDispatcher : public EventHandler {

    public:

        void SetConfig(const std::vector<KEYVALUEPAIR> config);
    
        void CallbackDispatcher::SetDownloadLocation(const std::string location);

        std::unique_ptr<ConfigResponse> onConfigRequest() override;

        std::unique_ptr<Response> onDeploymentAction(std::unique_ptr<DeploymentBase> dp) override;

        std::unique_ptr<Response> onCancelAction(std::unique_ptr<CancelAction> action) override;

        void onNoActions() override;

        ~CallbackDispatcher() = default;

        CallbackDispatcher(ConfigRequestCallbackFunction _configRequest, DeploymentActionCallbackFunction _deploymentAction, CancelActionCallbackFunction _cancelAction) {
            configRequest = _configRequest;
            deploymentAction = _deploymentAction;
            cancelAction = _cancelAction;
        };

    private:

        bool DeployArtifact(const std::shared_ptr<::Artifact> artifact, DEPLOYMENTINFO info);

        ConfigRequestCallbackFunction configRequest;
        DeploymentActionCallbackFunction deploymentAction;
        CancelActionCallbackFunction cancelAction;
        std::vector<KEYVALUEPAIR> configInfo;
        std::string downloadLocation;
    };
}