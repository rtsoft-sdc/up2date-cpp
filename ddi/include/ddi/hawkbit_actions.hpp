#pragma once

#include <string>
#include <functional>

namespace ddi {
    // This part contains actions that will be given to the EventHandler callbacks.
    // Do not copy or use these classes outside the handler class.

    ///\brief Contain meta for cancelAction request.
    class CancelAction {
    public:
        ///\brief Get action id.
        virtual int getId() = 0;

        ///\brief Get deployment id which required to stop.
        /// @note Usually id and stopId are equal, but you should stop action that stored in stopId field
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
        ///\brief Save file to path.
        virtual void downloadTo(std::string path) = 0;

        ///\brief Get response body as string.
        virtual std::string getBody() = 0;

        ///\brief Get file with user-defined Receiver.
        /// @note Return true from function to continue false to stop downloading.
        virtual void downloadWithReceiver(std::function<bool(const char *data, size_t data_length)>) = 0;

        ///\brief Get file name.
        virtual std::string getFilename() = 0;

        ///\brief Get Hashes struct that contain hashes.
        /// @note Can be used for check received file health.
        virtual Hashes getFileHashes() = 0;

        ///\brief Get file size.
        virtual int size() = 0;

        virtual ~Artifact() = default;
    };

    class Chunk {
    public:
        ///\brief Get chunk type.
        virtual std::string getPart() = 0;

        ///\brief Get chunk version.
        virtual std::string getVersion() = 0;

        ///\brief Get chunk name.
        virtual std::string getName() = 0;

        ///\brief Get \link ddi::Artifact Artifacts \endlink assigned to this chunk.
        virtual std::vector<std::shared_ptr<Artifact>> getArtifacts() = 0;

        virtual ~Chunk() = default;
    };

    ///\brief Data for deployment base (update) request.
    ///  <br><a href="https://www.eclipse.org/hawkbit/rest-api/rootcontroller-api-guide/#_deployment_or_update_action">docs</a>
    /// Structure of this package is equals to response fields in <a href="https://www.eclipse.org/hawkbit/rest-api/rootcontroller-api-guide/#_response_fields_4">structure</a><br>
    /// Simple files scheme is:
    ///```
    ///
    ///    Deployment base (update)
    ///              |
    ///      [chunk_1, ..., chunk_n]  (update can contain 1..n chunks. Chunk can contain many files (ex: 1 chunk -> 1 program update))
    ///         |    ,        |
    ///  [artifact_1, ... ], [artifact_1, ...] (chuck can contain 1..n artifacts (files).
    ///
    /// ```
    class DeploymentBase {
    public:
        ///\brief Returns id of current deployment.
        /// @note this id should be stored because should be used for cancel request
        virtual int getId() = 0;

        ///\brief  Returns download type [ex. forced, soft ...].
        virtual std::string getDownloadType() = 0;

        ///\brief  Returns update type [ex. forced ...].
        virtual std::string getUpdateType() = 0;

        ///\brief Check that client is in maintenance window now.
        /// @note update should be started only in this window
        virtual bool isInMaintenanceWindow() = 0;

        ///\brief Return assigned \link ddi::Chunk chunks \endlink
        virtual std::vector<std::shared_ptr<Chunk>> getChunks() = 0;

        virtual ~DeploymentBase() = default;
    };

}