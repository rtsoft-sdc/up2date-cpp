#pragma once

#include <string>
#include <memory>

#include "hawkbit_event_handler.hpp"

namespace ddi {

    /// \brief Main communication interface
    class Client {
    public:

        /*! Start client calling this method.
        * This method realized as infinite, blocking loop
        */
        virtual void run() = 0;

        /*! Call this method to safe exit from polling process
         *  Polling stops after full iteration done
         */
        virtual void stop() = 0;

        /*! Call this method to request client do Poll request as soon as it can
         * If client is polling now, request will be ignored.
         * Normally client should execute Poll request in 1 second
         */
        virtual void requestToPoll() = 0;

        virtual ~Client() = default;
    };

    /// \brief This class provides authorization restore interface.
    /*!
     * Client will pass it as argument in AuthErrorHandler (if it was implemented and registered).
     * Note: if set one of auth types, other will be removed. For example if set TLS auth model
     * all information required for GatewayToken or TargetToken auth will be removed.
     */
     class AuthRestoreHandler {
    public:
        /// Set mTLS keypair.
        virtual void setTLS(const std::string &crt, const std::string &key) = 0;

         ///\brief Set hawkBit endpoint.
         /*!
         * @note You should pass full url (ex: https://.../\<tenant\>/.../\<controllerId\>).
         */
        virtual void setEndpoint(const std::string &endpoint) = 0;

         ///\brief Set hawkBit endpoint.
         /*!
         * Endpoint will be built automatically.
         *  (ex: setEndpoint("https://my.hwb.n/", "my-controller") will register endpoint:
         *   https://my.hwb.n/default/controller/v1/my-controller
         * @note Path of the first argument will be ignored!!
         * (ex:  setEndpoint("https://my.hwb.n/this/will/be/igonred", "my-controller") will register endpoint:
         *   https://my.hwb.n/default/controller/v1/my-controller
         * @note Default value for tenant will be inherited for all child classes
         */
        virtual void setEndpoint(const std::string &hawkbitEndpoint,
                                 const std::string &controllerId, const std::string &tenant = "default") = 0;

        ///\brief Set deviceToken. (Security Token) <a href="https://www.eclipse.org/hawkbit/concepts/authentication/"> docs </a>
        virtual void setDeviceToken(const std::string &) = 0;

        ///\brief Set gatewayToken.
        virtual void setGatewayToken(const std::string &) = 0;

        virtual ~AuthRestoreHandler() = default;
    };

    /// \brief Interface for auth recovery
    /// This interface should be implemented and registered in client if required to make user-define HTTP_UNAUTHORIZED
    ///  error code handling.
    class AuthErrorHandler {
    public:

        /// \brief Restore entrypoint
        /*! Calls by client only in 2 cases:<br>
        *   1) ONCE at client startup if endpoint specified<br>
        *   2) if get HTTP_UNAUTHORIZED(401) error code from hawkBit<br>
        *  Note: in second after onAuthError was called, client will retry to do the same request
        *    with new auth params, but if server respond error again, exception will be thrown
        *    (in some cases you can catch it with ResponseDeliveryListener.
        */
        virtual void onAuthError(std::unique_ptr<AuthRestoreHandler>) = 0;

        virtual ~AuthErrorHandler() = default;
    };

    /// \brief Builder used for build and configure ddi::Client
    class DDIClientBuilder {
    public:
        ///\brief Get builder instance
        /// Should be used to get builder instance
        static std::unique_ptr<DDIClientBuilder> newInstance();

        ///\brief  Set pollingTimeout
        /// (timeout before next request to hawkBit). It used when cannot get this value from server.
        ///  Default this value is ignored.
        virtual DDIClientBuilder *setDefaultPollingTimeout(int pollingTimeout_) = 0;

        ///\brief  Set user-implemented ddi::EventHandler (business logic). Required.
        virtual DDIClientBuilder *setEventHandler(std::shared_ptr<EventHandler> handler) = 0;

        ///\brief  Add header to request. May be needed for proxy or etc. By default, not required.
        virtual DDIClientBuilder *addHeader(const std::string &, const std::string &) = 0;

        // Authorization block. Should be set only one of three given variants otherwise will get exception.
        // + -------------------------------------------------------------------------
        ///\brief  Set gatewayToken. Authorization with Gateway Token (<a href="https://www.eclipse.org/hawkbit/concepts/authentication/">Security</a>)
        /// @note Only one of ddi::DDIClientBuilder::setGatewayToken, ddi::DDIClientBuilder::setTLS, ddi::DDIClientBuilder::setDeviceToken
        /// should be set, or you will get an exception
        virtual DDIClientBuilder *setGatewayToken(const std::string &) = 0;

        ///\brief  Set deviceToken. Authorization with Security Token (<a href="https://www.eclipse.org/hawkbit/concepts/authentication/">Security</a>)
        /// @note Only one of ddi::DDIClientBuilder::setGatewayToken, ddi::DDIClientBuilder::setTLS, ddi::DDIClientBuilder::setDeviceToken
        /// should be set, or you will get an exception
        virtual DDIClientBuilder *setDeviceToken(const std::string &) = 0;

        ///\brief  Set mTLS authorization cert and key. Authorization with mTLS
        /// @note Only one of ddi::DDIClientBuilder::setGatewayToken, ddi::DDIClientBuilder::setTLS, ddi::DDIClientBuilder::setDeviceToken
        /// should be set, or you will get an exception
        virtual DDIClientBuilder *setTLS(const std::string &, const std::string &) = 0;
        // + -------------------------------------------------------------------------

        ///\brief Set not verify server certificate.
        /// @note USE ONLY FOR DEBUG (if your server has self-signed certificate).
        /// @note ignored if TLS authorization type is set.
        virtual DDIClientBuilder *notVerifyServerCertificate() = 0;


        ///\brief Set hawkBit endpoint.
        /*!
        * You should pass full url (ex: https://.../\<tenant\>/.../\<controllerId\>).
        */
        virtual DDIClientBuilder *setHawkbitEndpoint(const std::string &) = 0;

        ///\brief Set hawkBit endpoint.
        /*!
        * Endpoint will be built automatically.
        *  (ex: setEndpoint("https://my.hwb.n/", "my-controller") will register endpoint:
        *   https://my.hwb.n/default/controller/v1/my-controller
        * @note Path of the first argument will be ignored!!
        * (ex:  setEndpoint("https://my.hwb.n/this/will/be/igonred", "my-controller") will register endpoint:
        *   https://my.hwb.n/default/controller/v1/my-controller
        * @note Default value for tenant will be inherited for all child classes
        */
        virtual DDIClientBuilder *setHawkbitEndpoint(const std::string &endpoint,
                                                     const std::string &controllerId_,
                                                     const std::string &tenant_ = "default") = 0;

        ///\brief Register AuthErrorHandler.
        virtual DDIClientBuilder *setAuthErrorHandler(std::shared_ptr<AuthErrorHandler>) = 0;

        ///\brief Build ddi::Client instance.
        virtual std::unique_ptr<Client> build() = 0;

        virtual ~DDIClientBuilder() = default;
    };

}
