#include <iostream>
#include <fstream>

#include "ddi.hpp"

using namespace ddi;

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

class Handler : public EventHandler {

    std::string installCmd;

public:

    explicit Handler(const std::string installCmd_ = "") : installCmd(installCmd_) {}

    std::unique_ptr<ConfigResponse> onConfigRequest() override {
        std::cout << ">> Sending Config Data" << std::endl;

        return ConfigResponseBuilder::newInstance()
                ->addData("Client", "Up2date-cpp example")
                ->addData("Install command", installCmd)
                ->setIgnoreSleep()
                ->build();
    }

    std::unique_ptr<Response> onDeploymentAction(std::unique_ptr<DeploymentBase> dp) override {
        std::cout << ">> Get Deployment base request" << std::endl;
        std::cout << " id: " << dp->getId() << " update: " << dp->getUpdateType() << std::endl;
        std::cout << " download: " << dp->getDownloadType() << " inWindow: " << (bool) dp->isInMaintenanceWindow()
                  << std::endl;

        auto builder = ResponseBuilder::newInstance();
        builder->addDetail("Printed deployment base info");
        std::cout << " + CHUNKS:" << std::endl;

        auto response = Response::SUCCESS;
        for (const auto &chunk: dp->getChunks()) {
            std::cout << "  part: " << chunk->getPart() << std::endl;
            std::cout << "  name: " << chunk->getName() << " version: " << chunk->getVersion() << std::endl;
            std::cout << "  + ARTIFACTS:" << std::endl;
            for (const auto &artifact: chunk->getArtifacts()) {
                std::cout << "   filename: " << artifact->getFilename() << " size: " << artifact->size() << std::endl;
                std::cout << "   md5: " << artifact->getFileHashes().md5 << std::endl;
                std::cout << "   sha1: " << artifact->getFileHashes().sha1 << std::endl;
                std::cout << "   sha256: " << artifact->getFileHashes().sha256 << std::endl;
                builder->addDetail(artifact->getFilename() + " described. Starting download ...");
                std::cout << "  .. downloading " + artifact->getFilename() + "...";
                artifact->downloadTo(artifact->getFilename());
                builder->addDetail("Downloaded " + artifact->getFilename());
                std::cout << "[OK]" << std::endl;
                if (dp->getUpdateType() != "skip") {
                    if (installCmd == "") {
                        builder->addDetail("Installation skipped - no install command is defined.");
                    } else {
                        int result;
                        auto cmd = installCmd + " " + artifact->getFilename();
                        std::cout << "  .. executing command '" + cmd + "'...";
                        auto output = execCommand(cmd, result);
                        if (result == 0) {
                            builder->addDetail("Installed " + artifact->getFilename() + " \n" + output);
                            std::cout << "[OK]" << std::endl;
                        } else {
                            auto err_msg = "Installation command failed: " + cmd + " \n" + output;
                            builder->addDetail(err_msg);
                            std::cout << "[FAIL]" << std::endl << err_msg << std::endl ;
                            response = Response::FAILURE;
                            break;
                        }
                    }
                }
            }
            if (response != Response::SUCCESS) 
                break;
            std::cout << " + ---------------------------" << std::endl;
        }

        return builder->addDetail("Work done. Sending response")
                ->setIgnoreSleep()
                ->setExecution(Response::CLOSED)
                ->setFinished(response)
                ->setResponseDeliveryListener(
                        std::shared_ptr<ResponseDeliveryListener>(new DeploymentBaseFeedbackDeliveryListener()))
                ->build();

    }

    std::unique_ptr<Response> onCancelAction(std::unique_ptr<CancelAction> action) override {
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

    std::string execCommand(const std::string cmd, int& out_exitStatus) {
        out_exitStatus = 0;
        auto pPipe = ::popen(cmd.c_str(), "r");
        if (pPipe == nullptr)
        {
            throw std::runtime_error("Cannot open pipe");
        }

        std::array<char, 256> buffer;

        std::string result;

        while (!std::feof(pPipe))
        {
            auto bytes = std::fread(buffer.data(), 1, buffer.size(), pPipe);
            result.append(buffer.data(), bytes);
        }

        auto rc = ::pclose(pPipe);

        if (WIFEXITED(rc))
        {
            out_exitStatus = WEXITSTATUS(rc);
        }

        return result;
    }

    void onNoActions() override {
        std::cout << "No actions from hawkBit" << std::endl;
    }

    ~Handler() override = default;
};

char *getEnvOrExit(const char *name) {
    auto env = std::getenv(name);
    if (env == nullptr) {
        std::cout << "Environment variable " << name << " not set" << std::endl;
        exit(2);
    }
    return env;
}
