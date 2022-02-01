#pragma once

#include <iostream>

#include "hawkbit/hawkbit_event_handler.hpp"
#include "httplib.h"
#include "hawkbit_client_impl.hpp"

namespace hawkbit {
    class ProvisioningClientImpl : public HawkbitCommunicationClient {
        std::string crt;
        uri::URI provisioningURI;
        httplib::Headers provisioningHeaders;

        struct provisioningPayload {
            std::string crt;
            std::string key;
        } provisioningPayload;

        httplib::Result
            retryHandler(uri::URI uri1, const std::function<httplib::Result(httplib::Client &)> &function) override;

        void updateCredentials();

        httplib::Client newHttpClient(uri::URI &uri1) override;

        friend class ProvisioningClientBuilderImpl;
    public:
        void run() override;

    };

    class ProvisioningClientBuilderImpl: public ClientBuilder {
        std::string crt;
        uri::URI provisioningURI;
        int pollingTimeout = 30000;
        std::shared_ptr<EventHandler> handler;
        httplib::Headers defaultHeaders;
        httplib::Headers provisioningHeaders;


    public:
        ClientBuilder *setCrt(const std::string&root_) override;

        ClientBuilder *setProvisioningEndpoint(const std::string& provisioningEndpoint_) override;

        ClientBuilder *setDefaultPollingTimeout(int pollingTimeout_) override;

        ClientBuilder *setEventHandler(std::shared_ptr<EventHandler> handler) override;

        ClientBuilder *addHeader(const std::string &string, const std::string &string1) override;

        std::unique_ptr<Client> build() override;

        ClientBuilder *addProvisioningHeader(const std::string &string, const std::string &string1) override;

    };
}