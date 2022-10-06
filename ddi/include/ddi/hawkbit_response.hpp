#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>


namespace ddi {
    ///\brief  Interface that can be implemented to check that response delivered(not) to hawkBit.
    /*! If update is installed, but successful status response not delivered to hawkBit, you will get this update again.
    *  To prevent this you can store updateId, and send successful status response on the next request.
    */
    class ResponseDeliveryListener {
    public:
        ///\brief Called if response successful delivered to hawkBit.
        virtual void onSuccessfulDelivery() = 0;

        ///\brief Called if response not delivered to hawkBit.
        virtual void onError() = 0;

        virtual ~ResponseDeliveryListener() = default;
    };

    ///\brief  Response abstraction.
    /*! Returned as callbacks result in ddi::EventHandler.
    * @note Use ddo::ResponseBuilder to create ddi::Response instance.
    */
    class Response {
    public:
        ///\brief Indicates if the action is finished.
        /// <a href="https://www.eclipse.org/hawkbit/apis/ddi_api/">State Machine Mapping</a>
        enum Finished {
            SUCCESS,
            FAILURE,
            NONE
        };

        ///\brief Indicates status of the action
        /// <a href="https://github.com/eclipse/hawkbit/blob/3d3481110e5131958d8c2d3292b438eed77332c4/hawkbit-rest/hawkbit-ddi-api/src/main/java/org/eclipse/hawkbit/ddi/json/model/DdiStatus.java#L87">State Machine Mapping</a>
        enum Execution {
            CLOSED, PROCEEDING,
            CANCELED, SCHEDULED,
            REJECTED, RESUMED,
            DOWNLOAD, DOWNLOADED
        };

        // Next methods should not be used by user code (they are not documented).
        // ----------------------------------------------------------------
        /// @note should not be used by user code.
        static std::string finishedToString(const Finished &);

        /// @note should not be used by user code.
        static std::string executionToString(const Execution &);

        /// @note should not be used by user code.
        virtual Finished getFinished() = 0;

        /// @note should not be used by user code.
        virtual Execution getExecution() = 0;

        /// @note should not be used by user code.
        virtual std::vector<std::string> getDetails() = 0;

        /// @note should not be used by user code.
        virtual std::shared_ptr<ResponseDeliveryListener> getDeliveryListener() = 0;

        /// @note should not be used by user code.
        virtual bool isIgnoredSleep() = 0;

        virtual ~Response() = default;
        // ----------------------------------------------------------------
    };

    ///\brief Used to build ddi::Response.
    class ResponseBuilder {
    public:
        ///\brief Get instance of builder.
        static std::shared_ptr<ResponseBuilder> newInstance();

        ///\brief Set Finished flag.
        virtual ResponseBuilder *setFinished(Response::Finished) = 0;

        ///\brief Set Execution flag.
        virtual ResponseBuilder *setExecution(Response::Execution) = 0;

        ///\brief Add details (string). Will be sent as array to hawkBit.
        virtual ResponseBuilder *addDetail(const std::string &) = 0;

        ///\brief Set delivery listener.
        /// @note You can set own for each request.
        virtual ResponseBuilder *setResponseDeliveryListener(std::shared_ptr<ResponseDeliveryListener>) = 0;

        ///\brief If this flag is set, next action will be received without wait polling interval.
        // @note Is very useful when have many actions in queue.
        virtual ResponseBuilder *setIgnoreSleep() = 0;

        ///\brief Build ddi::Response.
        virtual std::unique_ptr<Response> build() = 0;

        virtual ~ResponseBuilder() = default;
    };

    ///\brief Config Response abstraction.
    /// @note Use ddi::ConfigResponseBuilder to create ddi::ConfigResponse instance.
    class ConfigResponse {
    public:
        enum Mode {
            MERGE,
            REPLACE,
            REMOVE
        };
        // Next methods should not be used by user code (they are not documented).
        // ----------------------------------------------------------------
        /// @note should not be used by user code.
        virtual std::map<std::string, std::string> getData() = 0;

        static std::string modeToString(Mode mode);

        /// @note should not be used by user code.
        virtual Mode getMode() = 0;

        /// @note should not be used by user code.
        virtual bool isIgnoredSleep() = 0;

        virtual ~ConfigResponse() = default;
        // ----------------------------------------------------------------
    };

    ///\brief ddi::ConfigResponseBuilder is used to build ddi::ConfigResponse.
    class ConfigResponseBuilder {
    public:
        ///\brief Get instance of builder.
        static std::shared_ptr<ConfigResponseBuilder> newInstance();

        ///\brief Add key, value data (config data)
        /// @note This data will be displayed in hawkBit GUI.
        virtual ConfigResponseBuilder *addData(const std::string &, const std::string &) = 0;

        ///\brief If this flag is set, next action will be received without wait polling interval.
        /// @note Is very useful when have many actions in queue.
        virtual ConfigResponseBuilder *setIgnoreSleep() = 0;

        ///\brief set config store mode (check ddi api docks)
        /// @note default merge
        virtual ConfigResponseBuilder *setMode(ConfigResponse::Mode mode) = 0;

        ///\brief Build ddi::ConfigResponse.
        virtual std::unique_ptr<ConfigResponse> build() = 0;

        virtual ~ConfigResponseBuilder() = default;
    };

}