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

    class AuthRestoreHandler {
    public:

        virtual void setTLS(const std::string &crt, const std::string &key) = 0;

        // should be used full url which contains controllerId
        virtual void setTargetEndpoint(std::string &endpoint) = 0;

        virtual void setDeviceToken(const std::string &) = 0;

        virtual void setGatewayToken(const std::string &) = 0;

        virtual ~AuthRestoreHandler() = default;
    };

    class AuthErrorHandler {
    public:
        virtual void onAuthError(std::unique_ptr<AuthRestoreHandler>) = 0;

        virtual ~AuthErrorHandler() = default;
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

        virtual DefaultClientBuilder *setAuthErrorHandler(std::shared_ptr<AuthErrorHandler>) = 0;

        virtual DefaultClientBuilder *setTLS(const std::string &, const std::string &) = 0;

        // all child classes will have the same default tenant value
        virtual DefaultClientBuilder *setHawkbitEndpoint(const std::string &endpoint,
                                                         const std::string &controllerId_,
                                                         const std::string &tenant_ = "default") = 0;

        virtual std::unique_ptr<Client> build() = 0;

        virtual ~DefaultClientBuilder() = default;
    };

}
