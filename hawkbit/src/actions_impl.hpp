#pragma once

#include <memory>
#include "hawkbit/hawkbit_actions.hpp"
#include "uriparse.hpp"
#include "httplib.h"

namespace hawkbit {
    // Define actions from hawkBit
    enum Actions_ {
        GET_CONFIG_DATA, CANCEL_ACTION, DEPLOYMENT_BASE,
        // if no actions
        NONE
    };

    class PollingData_ {

    public:
        int getSleepTime();

        Actions_ getAction();

        uri::URI getFollowURI();

        static std::unique_ptr<PollingData_> fromString(const std::string &);

    private:
        int sleepTime;
        Actions_ action;
        uri::URI followURI;
    };


    // internal CancelAction implementation
    class CancelAction_ : public CancelAction {
    private:
        int id;
        int stopId;

    public:

        int getId() override;

        int getStopId() override;

        static std::unique_ptr<CancelAction> fromString(const std::string &);
    };

    // used for get httpClient and its Headers
    class DownloadProvider {
    public:
        virtual void downloadTo(uri::URI, const std::string&) = 0;

        // get file as string
        virtual std::string getBody(uri::URI) = 0;

        // get file with defined Receiver. Return true from function to continue
        //  false to stop request
        virtual void downloadWithReceiver(uri::URI uri, std::function<bool(const char *data, size_t data_length)>) = 0;
    };

    // internal DeploymentBase implementation
    class DeploymentBase_ : public DeploymentBase {
    public:

        int getId() override;

        std::string getDownloadType() override;

        std::string getUpdateType() override;

        bool isInMaintenanceWindow() override;

        std::vector<std::shared_ptr<Chunk>> getChunks() override;

        static std::unique_ptr<DeploymentBase> from(const std::string &, DownloadProvider *);

    private:
        int id;
        std::string downloadType;
        std::string updateType;
        bool inMaintenanceWindow;
        std::vector<std::shared_ptr<Chunk>> chunks;

    };

    class Chunk_ : public Chunk {
    public:

        std::string getPart() override;

        std::string getVersion() override;

        std::string getName() override;

        std::vector<std::shared_ptr<Artifact>> getArtifacts() override;

    private:
        std::string part;
        std::string version;
        std::string name;
        std::vector<std::shared_ptr<Artifact>> artifacts;

        friend class DeploymentBase_;
    };

    class Artifact_ : public Artifact {
    public:

        void downloadTo(std::string path) override;

        std::string getBody() override;

        void downloadWithReceiver(std::function<bool(const char *, size_t)> function) override;

        std::string getFilename() override;

        Hashes getFileHashes() override;

        int size() override;

    private:
        std::string filename;
        Hashes fileHash;
        int fileSize;
        uri::URI downloadURI;
        DownloadProvider *downloadProvider;

        friend class DeploymentBase_;
    };

}