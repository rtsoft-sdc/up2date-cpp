#pragma once

#include <string>
#include <functional>

namespace hawkbit {
    // This part contains actions that will be given to the EventHandler as callback params
    // Do not copy or use these classes outside the handler class

    // Requires stopping Action with id (in stopId)
    class CancelAction {
    public:
        virtual int getId() = 0;

        // Usually id and stopId are equals, but should stop action that requested in stopId
        virtual int getStopId() = 0;

        virtual ~CancelAction() = default;
    };

    struct Hashes {
        std::string sha1;
        std::string md5;
        std::string sha256;
    };

    class Artifact {
    public:
        // save file to FS
        virtual void downloadTo(std::string path) = 0;

        // get file as string
        virtual std::string getBody() = 0;

        // get file with defined Receiver. Return true from function to continue
        //  false to stop request
        virtual void downloadWithReceiver(std::function<bool(const char *data, size_t data_length)>) = 0;

        virtual std::string getFilename() = 0;

        virtual Hashes getFileHashes() = 0;

        virtual int size() = 0;

        virtual ~Artifact() = default;
    };

    class Chunk {
    public:
        // returns chunk name
        virtual std::string getPart() = 0;

        // returns version
        virtual std::string getVersion() = 0;

        // returns name
        virtual std::string getName() = 0;

        // getArtifacts
        virtual std::vector<std::shared_ptr<Artifact>> getArtifacts() = 0;

        virtual ~Chunk() = default;
    };

    class DeploymentBase {
    public:
        // returns id of current deployment
        virtual int getId() = 0;

        // returns download type [ex. forced, soft ...]
        virtual std::string getDownloadType() = 0;

        // returns update type [ex. forced ...]
        virtual std::string getUpdateType() = 0;

        // if in maintenance window (update should be done only in window :) )
        virtual bool isInMaintenanceWindow() = 0;

        virtual std::vector<std::shared_ptr<Chunk>> getChunks() = 0;

        virtual ~DeploymentBase() = default;
    };

}