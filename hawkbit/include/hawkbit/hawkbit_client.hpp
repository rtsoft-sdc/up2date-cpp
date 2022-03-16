#pragma once

#include <string>
#include <memory>

#include "hawkbit_event_handler.hpp"

namespace hawkbit {
    // Main class that used for communication with hawkBit
    class Client {
    public:

        // infinity loop
        virtual void run() = 0;

        virtual ~Client() = default;
    };

    class ProvisioningClientBuilder {
    public:

        static std::unique_ptr<ProvisioningClientBuilder> newInstance();

        virtual ProvisioningClientBuilder *setCrt(const std::string&) = 0;

        virtual ProvisioningClientBuilder *setProvisioningEndpoint(const std::string&) = 0;

        virtual ProvisioningClientBuilder *setDefaultPollingTimeout(int pollingTimeout_) = 0;

        virtual ProvisioningClientBuilder *setEventHandler(std::shared_ptr<EventHandler> handler) = 0;

        virtual ProvisioningClientBuilder *addHeader(const std::string &, const std::string &) = 0;

        virtual ProvisioningClientBuilder *addProvisioningHeader(const std::string &, const std::string &) = 0;

        virtual std::unique_ptr<Client> build() = 0;

        virtual ~ProvisioningClientBuilder() = default;
    };

    class DefaultClientBuilder {
    public:
        static std::unique_ptr<DefaultClientBuilder> newInstance();

        virtual DefaultClientBuilder *setDefaultPollingTimeout(int pollingTimeout_) = 0;

        virtual DefaultClientBuilder *setEventHandler(std::shared_ptr<EventHandler> handler) = 0;

        virtual DefaultClientBuilder *addHeader(const std::string &, const std::string &) = 0;

        virtual DefaultClientBuilder *setGatewayToken(const std::string &) = 0;

        virtual DefaultClientBuilder *setDeviceToken(const std::string &) = 0;

        virtual DefaultClientBuilder *notVerifyServerCertificate() = 0;

        virtual DefaultClientBuilder *setHawkbitEndpoint(const std::string &) = 0;

        virtual DefaultClientBuilder *setControllerId(const std::string &) = 0;

        virtual DefaultClientBuilder *setTenant(const std::string &) = 0;

        virtual std::unique_ptr<Client> build() = 0;

        virtual ~DefaultClientBuilder() = default;
    };

}
