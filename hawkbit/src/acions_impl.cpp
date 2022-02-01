#include <vector>
#include <string>

#include "actions_impl.hpp"
#include "rapidjson/document.h"
#include "hawkbit_client_impl.hpp"
#include "utils.hpp"

namespace hawkbit {
    int CancelAction_::getId() {
        return id;
    }

    int CancelAction_::getStopId() {
        return stopId;
    }

    std::unique_ptr<CancelAction> CancelAction_::fromString(const std::string& body) {
        rapidjson::Document document;
        document.Parse<0>(body.c_str());

        if (document.HasParseError())
            throw unexpected_payload();

        if (!document.HasMember("id")) {
            throw unexpected_payload();
        }
        if (!document.HasMember("cancelAction") || !document["cancelAction"].HasMember("stopId")) {
            throw unexpected_payload();
        }

        auto cancelAction = new CancelAction_();
        // if exception while parsing memory will be cleared
        auto retAction = std::unique_ptr<CancelAction>(cancelAction);

        cancelAction->stopId = std::stoi(document["cancelAction"]["stopId"].GetString());
        cancelAction->id = std::stoi(document["id"].GetString());

        return retAction;
    }


    std::unique_ptr<DeploymentBase> DeploymentBase_::from(const std::string &body, DownloadProvider *requestFormatter) {
        rapidjson::Document document;
        document.Parse<0>(body.c_str());

        if (document.HasParseError())
            throw unexpected_payload();

        if (!document.HasMember("id") || !document.HasMember("deployment") ||
                !document["deployment"].HasMember("update") || !document["deployment"].HasMember("download")
                || !document["deployment"].HasMember("chunks")) {
            throw unexpected_payload();
        }

        auto deploymentBase = new DeploymentBase_();
        // if exception while parsing memory will be cleared
        auto retBase = std::unique_ptr<DeploymentBase>(deploymentBase);

        deploymentBase->id = std::stoi(document["id"].GetString());
        deploymentBase->updateType = document["deployment"]["update"].GetString();
        deploymentBase->downloadType = document["deployment"]["download"].GetString();

        if (document["deployment"].HasMember("maintenanceWindow")) {
            std::string window = document["deployment"]["maintenanceWindow"].GetString();
            deploymentBase->inMaintenanceWindow = window != "unavailable";
        } else {
            deploymentBase->inMaintenanceWindow = true;
        }

        const rapidjson::Value& chunks_ = document["deployment"]["chunks"];
        for (rapidjson::Value::ConstValueIterator itr = chunks_.Begin(); itr != chunks_.End(); ++itr) {
            const rapidjson::Value& chunk = *itr;
            if (!chunk.HasMember("part") || !chunk.HasMember("version") || !chunk.HasMember("name")
                || !chunk.HasMember("artifacts")) {
                throw unexpected_payload();
            }

            auto chunkR = new Chunk_();
            auto chunkPtr = std::shared_ptr<Chunk>(chunkR);

            chunkR->name = chunk["name"].GetString();
            chunkR->version = chunk["version"].GetString();
            chunkR->part = chunk["part"].GetString();

            const rapidjson::Value& artifacts_ = chunk["artifacts"];
            for (rapidjson::Value::ConstValueIterator itr_a = artifacts_.Begin(); itr_a != artifacts_.End(); ++itr_a) {
                const rapidjson::Value& artifact = *itr_a;
                if (!artifact.HasMember("filename") || !artifact.HasMember("hashes") || !artifact.HasMember("size")
                        || !artifact.HasMember("_links") || !artifact["hashes"].HasMember("sha256")
                        || !artifact["hashes"].HasMember("sha1") || !artifact["hashes"].HasMember("md5")
                        || !artifact["_links"].HasMember("download-http")) {
                    throw unexpected_payload();
                }
                Hashes hashesR;
                auto artifactR =  new Artifact_();
                auto artifactPtr = std::shared_ptr<Artifact>(artifactR);
                artifactR->filename = artifact["filename"].GetString();
                artifactR->fileSize = artifact["size"].GetInt();
                artifactR->downloadURI = parseHrefObject(artifact["_links"]["download-http"]);
                hashesR.md5 = artifact["hashes"]["md5"].GetString();
                hashesR.sha1 = artifact["hashes"]["sha1"].GetString();
                hashesR.sha256 = artifact["hashes"]["sha256"].GetString();
                artifactR->fileHash = hashesR;
                artifactR->downloadProvider = requestFormatter;

                chunkR->artifacts.push_back(artifactPtr);
            }

            deploymentBase->chunks.push_back(chunkPtr);
        }
        return retBase;
    }



    int DeploymentBase_::getId() {
        return id;
    }

    std::string DeploymentBase_::getDownloadType() {
        return downloadType;
    }

    std::string DeploymentBase_::getUpdateType() {
        return updateType;
    }

    bool DeploymentBase_::isInMaintenanceWindow() {
        return inMaintenanceWindow;
    }

    std::vector<std::shared_ptr<Chunk>> DeploymentBase_::getChunks() {
        return chunks;
    }

    std::string Chunk_::getPart() {
        return part;
    }

    std::string Chunk_::getVersion() {
        return version;
    }

    std::string Chunk_::getName() {
        return name;
    }

    std::vector<std::shared_ptr<Artifact>> Chunk_::getArtifacts() {
        return artifacts;
    }

    void Artifact_::downloadTo(std::string path) {
        downloadProvider->downloadTo(downloadURI, path);
    }

    std::string Artifact_::getBody() {
        return downloadProvider->getBody(downloadURI);
    }

    void Artifact_::downloadWithReceiver(std::function<bool(const char *, size_t)> func) {
        downloadProvider->downloadWithReceiver(downloadURI, func);
    }

    std::string Artifact_::getFilename() {
        return filename;
    }

    Hashes Artifact_::getFileHashes() {
        return fileHash;
    }

    int Artifact_::size() {
        return fileSize;
    }

    int PollingData_::getSleepTime() {
        return sleepTime;
    }

    Actions_ PollingData_::getAction() {
        return action;
    }

    uri::URI PollingData_::getFollowURI() {
        return followURI;
    }

    std::unique_ptr<PollingData_> PollingData_::fromString(const std::string &body) {
        rapidjson::Document document;
        document.Parse<0>(body.c_str());

        if (document.HasParseError())
            throw unexpected_payload();


        auto data = new PollingData_();
        auto dataPtr = std::unique_ptr<PollingData_>(data);

        // flag that no sleep time set (use default sleep time)
        data->sleepTime = -1;
        data->action = Actions_::NONE;

        // try to get sleepTime:
        std::string sleepTimeString;
        if (document.HasMember("config")) {
            const rapidjson::Value &config = document["config"];
            if (config.HasMember("polling")) {
                const rapidjson::Value &polling = config["polling"];
                if (polling.HasMember("sleep")) {
                    sleepTimeString = polling["sleep"].GetString();
                }
            }
        }
        if (!sleepTimeString.empty()) {
            int hh, mm, ss;
            if (sscanf(sleepTimeString.c_str(), "%d:%d:%d", &hh, &mm, &ss) != 3) {
                throw unexpected_payload();
            }
            data->sleepTime = (hh * 3600 + mm * 60 + ss) * 1000;
        }

        if (!document.HasMember("_links")) {
            return dataPtr;
        }

        const rapidjson::Value &links = document["_links"];
        // there`re 3 variants of actions: configData, deploymentBase, cancelAction
        // priority: configData, (cancelAction/deploymentBase)
        if (links.HasMember("configData")) {
            data->action = Actions_::GET_CONFIG_DATA;
            data->followURI = parseHrefObject(links["configData"]);
        } else if (links.HasMember("cancelAction")) {
            data->action = Actions_::CANCEL_ACTION;
            data->followURI = parseHrefObject(links["cancelAction"]);
        } else if (links.HasMember("deploymentBase")) {
            data->action = Actions_::DEPLOYMENT_BASE;
            data->followURI = parseHrefObject(links["deploymentBase"]);
        }

        return dataPtr;
    }
}