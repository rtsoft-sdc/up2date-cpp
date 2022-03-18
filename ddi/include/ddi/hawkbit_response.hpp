#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>


namespace ddi {
    // Interface that can be implemented to check that response delivered(not) to hawkBit
    // If update is installed, but successful status response not delivered to hawkBit, you will get this update again.
    //  To prevent this you can store updateId, and send successful status response on the next request.
    class ResponseDeliveryListener {
    public:
        virtual void onSuccessfulDelivery() = 0;

        virtual void onError() = 0;

        virtual ~ResponseDeliveryListener() = default;
    };

    // Response abstraction. Returned as callbacks result in EventHandler .
    // Use ResponseBuilder to create Response instance.
    class Response {
    public:
        // Read about hawkBit state machine here [https://www.eclipse.org/hawkbit/apis/ddi_api/] (State Machine Mapping)
        // Indicates if the action is finished
        enum Finished {
            SUCCESS,
            FAILURE,
            NONE
        };

        // Indicates status of the action
        enum Execution {
            CLOSED, PROCEEDING,
            CANCELED, SCHEDULED,
            REJECTED, RESUMED
        };

        // Next methods should not be used by user code (they are not documented).
        // ----------------------------------------------------------------
        static std::string finishedToString(const Finished &);

        static std::string executionToString(const Execution &);

        virtual Finished getFinished() = 0;

        virtual Execution getExecution() = 0;

        virtual std::vector<std::string> getDetails() = 0;

        virtual std::shared_ptr<ResponseDeliveryListener> getDeliveryListener() = 0;

        virtual bool isIgnoredSleep() = 0;

        virtual ~Response() = default;
        // ----------------------------------------------------------------
    };

    // ResponseBuilder is used to build Response.
    class ResponseBuilder {
    public:
        // Get instance of builder.
        static std::shared_ptr<ResponseBuilder> newInstance();

        // Set Finished flag.
        virtual ResponseBuilder *setFinished(Response::Finished) = 0;

        // Set Execution flag.
        virtual ResponseBuilder *setExecution(Response::Execution) = 0;

        // Add details (string). Will be sent as array to hawkBit.
        virtual ResponseBuilder *addDetail(const std::string &) = 0;

        // Set delivery listener. You can set own for each request.
        virtual ResponseBuilder *setResponseDeliveryListener(std::shared_ptr<ResponseDeliveryListener>) = 0;

        // If this flag is set, next action will be received without wait polling interval.
        // Is very useful when have many actions in queue.
        virtual ResponseBuilder *setIgnoreSleep() = 0;

        // Build Response.
        virtual std::unique_ptr<Response> build() = 0;

        virtual ~ResponseBuilder() = default;
    };

    // Config Response abstraction.
    // Use ConfigResponseBuilder to create ConfigResponse instance.
    class ConfigResponse {
    public:
        // Next methods should not be used by user code (they are not documented).
        // ----------------------------------------------------------------
        virtual std::map<std::string, std::string> getData() = 0;

        virtual bool isIgnoredSleep() = 0;

        virtual ~ConfigResponse() = default;
        // ----------------------------------------------------------------
    };

    // ConfigResponseBuilder is used to build ConfigResponse.
    class ConfigResponseBuilder {
    public:
        // Get instance of builder.
        static std::shared_ptr<ConfigResponseBuilder> newInstance();

        // Add key, value data (config data), will be displayed in hawkBit GUI.
        virtual ConfigResponseBuilder *addData(const std::string &, const std::string &) = 0;

        // If this flag is set, next action will be received without wait polling interval.
        // Is very useful when have many actions in queue.
        virtual ConfigResponseBuilder *setIgnoreSleep() = 0;

        // Build ConfigResponse.
        virtual std::unique_ptr<ConfigResponse> build() = 0;

        virtual ~ConfigResponseBuilder() = default;
    };

}