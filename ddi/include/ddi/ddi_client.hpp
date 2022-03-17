#pragma once

#include <string>
#include <memory>

#include "hawkbit_event_handler.hpp"

namespace ddi {
    // Main class that used for communication with hawkBit
    class Client {
    public:

        // infinity loop
        virtual void run() = 0;

        virtual ~Client() = default;
    };

    class AuthRestoreHandler {
    public:

        virtual void setTLS(const std::string &crt, const std::string &key) = 0;

        // should be used full url which contains controllerId
        virtual void setEndpoint(const std::string &endpoint) = 0;
        // default value for tenant will be inherited for all child classes
        virtual void setEndpoint(std::string &hawkbitEndpoint,
                                 const std::string &controllerId, const std::string &tenant = "default") = 0;

        virtual void setDeviceToken(const std::string &) = 0;

        virtual void setGatewayToken(const std::string &) = 0;

        virtual ~AuthRestoreHandler() = default;
    };

    class AuthErrorHandler {
    public:
        virtual void onAuthError(std::unique_ptr<AuthRestoreHandler>) = 0;

        virtual ~AuthErrorHandler() = default;
    };

    class DDIClientBuilder {
    public:
        static std::unique_ptr<DDIClientBuilder> newInstance();

        virtual DDIClientBuilder *setDefaultPollingTimeout(int pollingTimeout_) = 0;

        virtual DDIClientBuilder *setEventHandler(std::shared_ptr<EventHandler> handler) = 0;

        virtual DDIClientBuilder *addHeader(const std::string &, const std::string &) = 0;

        virtual DDIClientBuilder *setGatewayToken(const std::string &) = 0;

        virtual DDIClientBuilder *setDeviceToken(const std::string &) = 0;

        virtual DDIClientBuilder *notVerifyServerCertificate() = 0;

        virtual DDIClientBuilder *setHawkbitEndpoint(const std::string &) = 0;

        virtual DDIClientBuilder *setAuthErrorHandler(std::shared_ptr<AuthErrorHandler>) = 0;

        virtual DDIClientBuilder *setTLS(const std::string &, const std::string &) = 0;

        // all child classes will have the same default tenant value
        virtual DDIClientBuilder *setHawkbitEndpoint(const std::string &endpoint,
                                                     const std::string &controllerId_,
                                                     const std::string &tenant_ = "default") = 0;

        virtual std::unique_ptr<Client> build() = 0;

        virtual ~DDIClientBuilder() = default;
    };

}
