#pragma once

#include <string>
#include <memory>

#include "hawkbit_event_handler.hpp"

namespace ddi {

    // Main communication interface
    class Client {
    public:

        // Start client calling this method.
        // This method realized as infinite, blocking loop
        virtual void run() = 0;

        virtual ~Client() = default;
    };

    // This class provides authorization restore interface.
    // Client will pass it as argument in AuthErrorHandler (if it was implemented and registered).
    // Note: if set one of auth types, other will be removed. For example if set TLS auth model
    //  all information required for GatewayToken or TargetToken auth will be removed.
    class AuthRestoreHandler {
    public:
        // Set mTLS keypair.
        virtual void setTLS(const std::string &crt, const std::string &key) = 0;

        // Set hawkBit endpoint. If used with 1 arg, full ur; should be specified
        //  (ex: https://.../<tenant>/.../<controllerId)
        // If used with 3[2] arguments full endpoint will be built automatically.
        //  (ex: setEndpoint("https://my.hwb.n/", "my-controller") will register endpoint:
        //   https://my.hwb.n/default/controller/v1/my-controller
        // Note: if you use function with 3 args, path of the first argument will be ignored!!
        //  (ex:  setEndpoint("https://my.hwb.n/this/will/be/igonred", "my-controller") will register endpoint:
        //   https://my.hwb.n/default/controller/v1/my-controller
        virtual void setEndpoint(const std::string &endpoint) = 0;
        // default value for tenant will be inherited for all child classes
        virtual void setEndpoint(std::string &hawkbitEndpoint,
                                 const std::string &controllerId, const std::string &tenant = "default") = 0;

        // Set deviceToken. (Security Token) [https://www.eclipse.org/hawkbit/concepts/authentication/]
        virtual void setDeviceToken(const std::string &) = 0;

        // Set gatewayToken.
        virtual void setGatewayToken(const std::string &) = 0;

        virtual ~AuthRestoreHandler() = default;
    };

    // This interface should be implemented and registered in client if required to make user-define HTTP_UNAUTHORIZED
    //  error code handling.
    class AuthErrorHandler {
    public:

        // Calls by client only in 2 cases:
        //   1) ONCE at client startup if endpoint specified
        //   2) if get HTTP_UNAUTHORIZED(401) error code from hawkBit
        //  Note: in second after onAuthError was called, client will retry to do the same request
        //    with new auth params, but if server respond error again, exception will be thrown
        //    (in some cases you can catch it with ResponseDeliveryListener. Read more in interface
        //    description).
        virtual void onAuthError(std::unique_ptr<AuthRestoreHandler>) = 0;

        virtual ~AuthErrorHandler() = default;
    };

    // Builder used for build and configure ddi::Client
    class DDIClientBuilder {
    public:
        // Should be used to get builder instance
        static std::unique_ptr<DDIClientBuilder> newInstance();

        // Set pollingTimeout (timeout before next request to hawkBit). It used when cannot get this value from server.
        //  Default this value is ignored.
        virtual DDIClientBuilder *setDefaultPollingTimeout(int pollingTimeout_) = 0;

        // Set user-implemented EventHandler (business logic). Required.
        virtual DDIClientBuilder *setEventHandler(std::shared_ptr<EventHandler> handler) = 0;

        // Add header to request. May be needed for proxy or etc. By default, not required.
        virtual DDIClientBuilder *addHeader(const std::string &, const std::string &) = 0;

        // Authorization block. Should be set only one of three given variants otherwise will get exception.
        // + -------------------------------------------------------------------------
        // set gatewayToken.
        virtual DDIClientBuilder *setGatewayToken(const std::string &) = 0;

        // Set deviceToken. (Security Token) [https://www.eclipse.org/hawkbit/concepts/authentication/]
        virtual DDIClientBuilder *setDeviceToken(const std::string &) = 0;

        // Set mTLS authorization cert and key.
        virtual DDIClientBuilder *setTLS(const std::string &, const std::string &) = 0;
        // + -------------------------------------------------------------------------

        // Not verify server certificate. USE ONLY FOR DEBUG (if your test server has self-signed certificate).
        //  Note: ignored if TLS authorization type is set.
        virtual DDIClientBuilder *notVerifyServerCertificate() = 0;

        // Set hawkBit endpoint. If used with 1 arg, full ur; should be specified
        //  (ex: https://.../<tenant>/.../<controllerId)
        // If used with 3[2] arguments full endpoint will be built automatically.
        //  (ex: setEndpoint("https://my.hwb.n/", "my-controller") will register endpoint:
        //   https://my.hwb.n/default/controller/v1/my-controller
        // Note: if you use function with 3 args, path of the first argument will be ignored!!
        //  (ex:  setEndpoint("https://my.hwb.n/this/will/be/igonred", "my-controller") will register endpoint:
        //   https://my.hwb.n/default/controller/v1/my-controller
        virtual DDIClientBuilder *setHawkbitEndpoint(const std::string &) = 0;
        // all child classes will have the same default tenant value
        virtual DDIClientBuilder *setHawkbitEndpoint(const std::string &endpoint,
                                                     const std::string &controllerId_,
                                                     const std::string &tenant_ = "default") = 0;

        // Register AuthErrorHandler.
        virtual DDIClientBuilder *setAuthErrorHandler(std::shared_ptr<AuthErrorHandler>) = 0;

        // Build ddi::Client instance.
        virtual std::unique_ptr<Client> build() = 0;

        virtual ~DDIClientBuilder() = default;
    };

}
