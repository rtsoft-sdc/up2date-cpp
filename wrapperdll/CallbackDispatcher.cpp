#include <iostream>
#include <fstream>

#include "ddi.hpp"
#include "CallbackDispatcher.hpp"

using namespace ddi;

namespace HkbClient {

    void CallbackDispatcher::SetConfig(const std::vector<KEYVALUEPAIR> config) {
        configInfo = config;
    }

    void CallbackDispatcher::SetDownloadLocation(std::string location) {
        downloadLocation = location;
    }

    std::unique_ptr<ConfigResponse> CallbackDispatcher::onConfigRequest() {
        auto builder = ConfigResponseBuilder::newInstance();
        configRequest(builder.get());

        return builder->build();
    }

    std::unique_ptr<Response> CallbackDispatcher::onDeploymentAction(std::unique_ptr<DeploymentBase> dp) {
        auto builder = ResponseBuilder::newInstance();
        builder->addDetail("Deployment started");

        DEPLOYMENTINFO info;
        info.id = dp->getId();
        info.updateType = dp->getUpdateType();
        info.downloadType = dp->getDownloadType();
        info.isInMaintenanceWindow = dp->isInMaintenanceWindow();

        bool anyFailed = false;

        for (const auto &chunk: dp->getChunks()) {
            info.chunkName = chunk->getName();
            info.chunkPart = chunk->getPart();
            info.chunkVersion = chunk->getVersion();

            for (const auto &artifact: chunk->getArtifacts()) {
                builder->addDetail("Attribute deployment started " + artifact->getFilename());

                info.artifactFileName = artifact->getFilename();
                info.artifactFileHashMd5 = artifact->getFileHashes().md5;
                info.artifactFileHashSha1 = artifact->getFileHashes().sha1;
                info.artifactFileHashSha256 = artifact->getFileHashes().sha256;

                bool attrDeployed = DeployArtifact(artifact, info);
                anyFailed |= !attrDeployed;

                builder->addDetail("Attribute deployment " + attrDeployed ? "completed " : "failed " + artifact->getFilename());
            }
        }

        if (!anyFailed) {
            builder->setIgnoreSleep();
        }

        return builder->addDetail("Work done. Sending response")
                ->setExecution(Response::CLOSED)
                ->setFinished(anyFailed ? Response::FAILURE : Response::SUCCESS)
                ->setResponseDeliveryListener(std::shared_ptr<ResponseDeliveryListener>(new DeploymentBaseFeedbackDeliveryListener()))
                ->build();
    }

    std::unique_ptr<Response> CallbackDispatcher::onCancelAction(std::unique_ptr<CancelAction> action) {
        bool cancelled = cancelAction(action->getStopId());
        
        return ResponseBuilder::newInstance()
                ->setExecution(ddi::Response::CLOSED)
                ->setFinished(cancelled ? ddi::Response::SUCCESS : ddi::Response::FAILURE)
                ->addDetail("Some feedback")
                ->addDetail("One more feedback")
                ->addDetail("Really important feedback")
                ->setResponseDeliveryListener(
                        std::shared_ptr<ResponseDeliveryListener>(new CancelActionFeedbackDeliveryListener()))
                ->setIgnoreSleep()
                ->build();
    }

    void CallbackDispatcher::onNoActions() {
    }

    bool CallbackDispatcher::DeployArtifact(const std::shared_ptr<::Artifact> artifact, DEPLOYMENTINFO info)
    {
        _DEPLOYMENTINFO callback_info;
        callback_info.id = info.id;
        callback_info.updateType = info.updateType.c_str();
        callback_info.downloadType = info.downloadType.c_str();
        callback_info.isInMaintenanceWindow = info.isInMaintenanceWindow;
        callback_info.chunkPart = info.chunkPart.c_str();
        callback_info.chunkName = info.chunkName.c_str();
        callback_info.chunkVersion = info.chunkVersion.c_str();
        callback_info.artifactFileName = info.artifactFileName.c_str();
        callback_info.artifactFileHashMd5 = info.artifactFileHashMd5.c_str();
        callback_info.artifactFileHashSha1 = info.artifactFileHashSha1.c_str();
        callback_info.artifactFileHashSha256 = info.artifactFileHashSha256.c_str();

        return deploymentAction(artifact.get(), callback_info);
    }

}
