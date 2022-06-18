#include <iostream>
#include <fstream>

#include "..\ddi\include\ddi.hpp"
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
        std::cout << ">> Sending Config Data" << std::endl;

        auto builder = ConfigResponseBuilder::newInstance();

        for (KEYVALUEPAIR kvp : configInfo)
        {
            builder->addData(kvp.key, kvp.value);
        }

        return builder->build();
    }

    std::unique_ptr<Response> CallbackDispatcher::onDeploymentAction(std::unique_ptr<DeploymentBase> dp) {
        std::cout << ">> Get Deployment base request" << std::endl;
        std::cout << " id: " << dp->getId() << " update: " << dp->getUpdateType() << std::endl;
        std::cout << " download: " << dp->getDownloadType() << " inWindow: " << (bool) dp->isInMaintenanceWindow()
                << std::endl;

        auto builder = ResponseBuilder::newInstance();
        builder->addDetail("Printed deployment base info");
        std::cout << " + CHUNKS:" << std::endl;

        for (const auto &chunk: dp->getChunks()) {
            std::cout << "  part: " << chunk->getPart() << std::endl;
            std::cout << "  name: " << chunk->getName() << " version: " << chunk->getVersion() << std::endl;
            std::cout << "  + ARTIFACTS:" << std::endl;
            for (const auto &artifact: chunk->getArtifacts()) {
                // fill deployment info for callback
                DEPLOYMENTINFO info;
                info.eventType = STARTED;
                info.id = dp->getId();
                info.updateType = dp->getUpdateType();
                info.downloadType = dp->getDownloadType();
                info.isInMaintenanceWindow = dp->isInMaintenanceWindow();
                info.chunkName = chunk->getName();
                info.chunkPart = chunk->getPart();
                info.chunkVersion = chunk->getVersion();
                info.artifactFileName = artifact->getFilename();
                info.artifactFileHashMd5 = artifact->getFileHashes().md5;
                info.artifactFileHashSha1 = artifact->getFileHashes().sha1;
                info.artifactFileHashSha256 = artifact->getFileHashes().sha256;
                ReportDeployment(info);

                std::cout << "   filename: " << artifact->getFilename() << " size: " << artifact->size() << std::endl;
                std::cout << "   md5: " << artifact->getFileHashes().md5 << std::endl;
                std::cout << "   sha1: " << artifact->getFileHashes().sha1 << std::endl;
                std::cout << "   sha256: " << artifact->getFileHashes().sha256 << std::endl;
                builder->addDetail(artifact->getFilename() + " described. Starting download ...");
                std::cout << "  .. downloading " + artifact->getFilename() + "...";
                artifact->downloadTo(downloadLocation + artifact->getFilename());
                builder->addDetail("Downloaded " + artifact->getFilename());
                std::cout << "[OK]" << std::endl;

                info.eventType = FINISHED;
                ReportDeployment(info);
            }
            std::cout << " + ---------------------------" << std::endl;
        }

        return builder->addDetail("Work done. Sending response")
                ->setIgnoreSleep()
                ->setExecution(Response::CLOSED)
                ->setFinished(Response::SUCCESS)
                ->setResponseDeliveryListener(
                        std::shared_ptr<ResponseDeliveryListener>(new DeploymentBaseFeedbackDeliveryListener()))
                ->build();

    }

    std::unique_ptr<Response> CallbackDispatcher::onCancelAction(std::unique_ptr<CancelAction> action) {
        std::cout << ">> CancelAction: id " << action->getId() << ", stopId " << action->getStopId() << std::endl;

        return ResponseBuilder::newInstance()
                ->setExecution(ddi::Response::CLOSED)
                ->setFinished(ddi::Response::SUCCESS)
                ->addDetail("Some feedback")
                ->addDetail("One more feedback")
                ->addDetail("Really important feedback")
                ->setResponseDeliveryListener(
                        std::shared_ptr<ResponseDeliveryListener>(new CancelActionFeedbackDeliveryListener()))
                ->setIgnoreSleep()
                ->build();
    }

    void CallbackDispatcher::onNoActions() {
        std::cout << "No actions from hawkBit" << std::endl;
    }

    void CallbackDispatcher::ReportDeployment(DEPLOYMENTINFO info)
    {
        _DEPLOYMENTINFO callback_info {
            info.eventType,
            info.id,
            info.updateType.c_str(),
            info.downloadType.c_str(),
            info.isInMaintenanceWindow,
            info.chunkPart.c_str(),
            info.chunkName.c_str(),
            info.chunkVersion.c_str(),
            info.artifactFileName.c_str(),
            info.artifactFileHashMd5.c_str(),
            info.artifactFileHashSha1.c_str(),
            info.artifactFileHashSha256.c_str()
        };
        callbackfunction(callback_info);
    }

}
