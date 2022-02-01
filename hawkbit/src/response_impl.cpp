#include "response_impl.hpp"

namespace hawkbit {
    Response::Finished ResponseImpl::getFinished() {
        return finished;
    }

    Response::Execution ResponseImpl::getExecution() {
        return execution;
    }

    std::vector<std::string> ResponseImpl::getDetails() {
        return details;
    }

    std::shared_ptr<ResponseDeliveryListener> ResponseImpl::getDeliveryListener() {
        return responseDeliveryListener;
    }

    bool ResponseImpl::isIgnoredSleep() {
        return ignoreSleep;
    }

    ResponseBuilder *ResponseBuilderImpl::addDetail(const std::string &detail) {
        details.push_back(detail);

        return this;
    }

    ResponseBuilder *ResponseBuilderImpl::setFinished(Response::Finished finished) {
        this->finished = finished;

        return this;
    }

    ResponseBuilder *ResponseBuilderImpl::setExecution(Response::Execution execution) {
        this->execution = execution;

        return this;
    }

    ResponseBuilder *
    ResponseBuilderImpl::setResponseDeliveryListener(
            std::shared_ptr<ResponseDeliveryListener> responseDeliveryListener_) {
        responseDeliveryListener = responseDeliveryListener_;

        return this;
    }

    ResponseBuilder *ResponseBuilderImpl::setIgnoreSleep() {
        ignoreSleep = true;

        return this;
    }

    std::unique_ptr<Response> ResponseBuilderImpl::build() {
        auto response = new ResponseImpl();
        response->details = details;
        response->execution = execution;
        response->finished = finished;
        response->ignoreSleep = ignoreSleep;
        response->responseDeliveryListener = responseDeliveryListener;

        return std::unique_ptr<Response>(response);
    }


    bool ConfigResponseImpl::isIgnoredSleep() {
        return ignoreSleep;
    }

    std::map<std::string, std::string> ConfigResponseImpl::getData() {
        return data;
    }

    ConfigResponseBuilder *ConfigResponseBuilderImpl::setIgnoreSleep() {
        ignoreSleep = true;

        return this;
    }

    ConfigResponseBuilder *ConfigResponseBuilderImpl::addData(const std::string &key, const std::string &value) {
        data.insert({key, value});

        return this;
    }

    std::unique_ptr<ConfigResponse> ConfigResponseBuilderImpl::build() {
        auto configResponse = new ConfigResponseImpl();
        configResponse->data = data;
        configResponse->ignoreSleep = ignoreSleep;
        return std::unique_ptr<ConfigResponse>(configResponse);
    }

}