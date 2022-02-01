#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>


namespace hawkbit {
    class ResponseDeliveryListener {
    public:
        virtual void onSuccessfulDelivery() = 0;

        virtual void onError() = 0;

        virtual ~ResponseDeliveryListener() = default;
    };

    class Response {
    public:
        enum Finished {
            SUCCESS,
            FAILURE,
            NONE
        };
        enum Execution {
            CLOSED, PROCEEDING,
            CANCELED, SCHEDULED,
            REJECTED, RESUMED
        };

        static std::string finishedToString(const Finished &);

        static std::string executionToString(const Execution &);

        virtual  Finished getFinished() = 0;

        virtual Execution getExecution() = 0;

        virtual std::vector<std::string> getDetails() = 0;

        virtual std::shared_ptr<ResponseDeliveryListener> getDeliveryListener() = 0;

        virtual bool isIgnoredSleep() = 0;

        virtual ~Response() = default;
    };

    class ResponseBuilder {
    public:
        static std::shared_ptr<ResponseBuilder> newInstance();

        virtual ResponseBuilder *setFinished(Response::Finished) = 0;

        virtual ResponseBuilder *setExecution(Response::Execution) = 0;

        virtual ResponseBuilder *addDetail(const std::string &) = 0;

        virtual ResponseBuilder *setResponseDeliveryListener(std::shared_ptr<ResponseDeliveryListener>) = 0;

        virtual ResponseBuilder *setIgnoreSleep() = 0;

        virtual std::unique_ptr<Response> build() = 0;

        virtual ~ResponseBuilder() = default;
    };

    class ConfigResponse {
    public:
        virtual std::map<std::string, std::string> getData() = 0;

        virtual bool isIgnoredSleep() = 0;

        virtual ~ConfigResponse() = default;
    };

    class ConfigResponseBuilder {
    public:
        static std::shared_ptr<ConfigResponseBuilder> newInstance();

        virtual ConfigResponseBuilder *addData(const std::string &, const std::string &) = 0;

        virtual ConfigResponseBuilder *setIgnoreSleep() = 0;

        virtual std::unique_ptr<ConfigResponse> build() = 0;

        virtual ~ConfigResponseBuilder() = default;
    };

}