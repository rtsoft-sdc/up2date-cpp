#pragma once

#include <string>
#include <functional>

namespace ddi {
    // This part contains actions that will be given to the EventHandler callbacks.
    // Do not copy or use these classes outside the handler class.

    // This action contain meta for cancelAction request.
    class CancelAction {
    public:
        virtual int getId() = 0;

        // Usually id and stopId are equal, but you should stop action that stored in stopId field
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
        // Save file to path
        virtual void downloadTo(std::string path) = 0;

        // Get response body as string
        virtual std::string getBody() = 0;

        // Get file with user-defined Receiver. Return true from function to continue
        //  false to stop downloading.
        virtual void downloadWithReceiver(std::function<bool(const char *data, size_t data_length)>) = 0;

        // Get file name.
        virtual std::string getFilename() = 0;

        // Get Hashes struct that contain hashes. Can be used for check received file health
        virtual Hashes getFileHashes() = 0;

        // Get file size.
        virtual int size() = 0;

        virtual ~Artifact() = default;
    };

    class Chunk {
    public:
        // Get chunk type
        virtual std::string getPart() = 0;

        // Get chunk version
        virtual std::string getVersion() = 0;

        // Get chunk name
        virtual std::string getName() = 0;

        // Get Artifacts assigned to this chunk
        virtual std::vector<std::shared_ptr<Artifact>> getArtifacts() = 0;

        virtual ~Chunk() = default;
    };

    // Data for deployment base (update) request.
    //  [https://www.eclipse.org/hawkbit/rest-api/rootcontroller-api-guide/#_deployment_or_update_action]
    // Structure of this package is equals to response fields structure [https://www.eclipse.org/hawkbit/rest-api/rootcontroller-api-guide/#_response_fields_4]
    // Simple files scheme is:
    //    Deployment base (update)
    //               |
    //      [chunk_1, ..., chunk_n]  (update can contain 1..n chunks. Chunk can contain many files (ex: 1 chunk -> 1 program update))
    //         |    ,        |
    //  [artifact_1, ... ], [artifact_1, ...] (chuck can contain 1..n artifacts (files).
    class DeploymentBase {
    public:
        // Returns id of current deployment (this id should be stored because should be used for cancel request)
        virtual int getId() = 0;

        // Returns download type [ex. forced, soft ...]
        virtual std::string getDownloadType() = 0;

        // Returns update type [ex. forced ...]
        virtual std::string getUpdateType() = 0;

        // If in maintenance window (update should be done only in this window :) )
        virtual bool isInMaintenanceWindow() = 0;

        // Return assigned chunks
        virtual std::vector<std::shared_ptr<Chunk>> getChunks() = 0;

        virtual ~DeploymentBase() = default;
    };

}