#include "hawkbit/hawkbit_response.hpp"

namespace hawkbit {
    class ResponseImpl : public Response {
    private:
        Finished finished;
        Execution execution;
        std::vector<std::string> details;
        std::shared_ptr<ResponseDeliveryListener> responseDeliveryListener;
        bool ignoreSleep;

        friend class ResponseBuilderImpl;

    public:
        Finished getFinished() override;

        Execution getExecution() override;

        std::vector<std::string> getDetails() override;

        std::shared_ptr<ResponseDeliveryListener> getDeliveryListener() override;

        bool isIgnoredSleep() override;

        friend class ResponseBuilder;
    };

    class ResponseBuilderImpl : public ResponseBuilder {
        Response::Finished finished;
        Response::Execution execution;
        std::vector<std::string> details;
        std::shared_ptr<ResponseDeliveryListener> responseDeliveryListener;
        bool ignoreSleep = false;

    public:

        ResponseBuilder *setFinished(Response::Finished) override;

        ResponseBuilder *setExecution(Response::Execution) override;

        ResponseBuilder *addDetail(const std::string &) override;

        ResponseBuilder *setResponseDeliveryListener(std::shared_ptr<ResponseDeliveryListener>) override;

        ResponseBuilder *setIgnoreSleep() override;

        std::unique_ptr<Response> build() override;
    };

    class ConfigResponseImpl : public ConfigResponse {
        std::map<std::string, std::string> data;
        bool ignoreSleep = false;

        friend class ConfigResponseBuilderImpl;

    public:
        std::map<std::string, std::string> getData() override;

        bool isIgnoredSleep() override;
    };

    class ConfigResponseBuilderImpl : public ConfigResponseBuilder {
        std::map<std::string, std::string> data;

        bool ignoreSleep = false;

    public:

        ConfigResponseBuilder *addData(const std::string &, const std::string &) override;

        ConfigResponseBuilder *setIgnoreSleep() override;

        std::unique_ptr<ConfigResponse> build() override;
    };
}