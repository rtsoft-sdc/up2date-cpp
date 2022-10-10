#pragma once

#include <string>
#include <memory>

#include "uriparse.hpp"
#include "ddi/hawkbit_event_handler.hpp"
#include "ddi/hawkbit_exceptions.hpp"
#include "actions_impl.hpp"
#include "ddi/ddi_client.hpp"
#include "httpclient.hpp"

namespace ddi {

    class PollingData_;

    class HawkbitCommunicationClient : public DownloadProvider, public Client, public AuthRestoreHandler {
    protected:
        uri::URI hawkbitURI;

        httpclient::Headers defaultHeaders;

        std::shared_ptr<EventHandler> handler;
        std::shared_ptr<AuthErrorHandler> authErrorHandler;

        int defaultSleepTime;
        int currentSleepTime;

        bool ignoreSleep;

        bool serverCertificateVerify = true;

        struct {
            std::string crt;
            std::string key;
            bool isSet = false;
        } mTLSKeypair;

        // starting hawkbit communication logic.
        //  returns execute time in ms
        void doPoll();

        // call user-defined handler and send config data to hawkBit
        void followConfigData(uri::URI &);

        // call user-defined handler to process cancelAction and send response to hawkBit
        void followCancelAction(uri::URI &);

        // call user-defined handler to process deploymentBase and send response to hawkBit
        void followDeploymentBase(uri::URI &);

        // all requests should go via retryHandler
        httpclient::Result wrappedRequest(uri::URI, const std::function<httpclient::Result(httpclient::Client &)> &);

        httpclient::Result retryHandler(uri::URI, const std::function<httpclient::Result(httpclient::Client &)> &);

        // creates httpClient with predefined params
        httpclient::Client newHttpClient(uri::URI &) const;

    public:

        [[noreturn]] void run() override;

        void downloadTo(uri::URI uri, const std::string &path) override;

        std::string getBody(uri::URI uri) override;

        void downloadWithReceiver(uri::URI uri, std::function<bool(const char *, size_t)> function) override;

        void setTLS(const std::string &crt, const std::string &key) override;

        void setEndpoint(const std::string &endpoint) override;

        void setEndpoint(std::string &hawkbitEndpoint,
                         const std::string &controllerId, const std::string &tenant = "default") override;

        void setDeviceToken(const std::string &string) override;

        void setGatewayToken(const std::string &string) override;

        friend class DefaultClientBuilderImpl;
    };

    class DefaultClientBuilderImpl : public DDIClientBuilder {
        std::string hawkbitUri;

        int pollingTimeout = 30000;

        httpclient::Headers defaultHeaders;

        std::shared_ptr<AuthErrorHandler> authErrorHandler;
        std::shared_ptr<EventHandler> handler;

        std::string token;
        std::string crt, key;

        enum AuthorizeVariants {
            NOT_SET,
            GATEWAY_TOKEN,
            DEVICE_TOKEN,
            M_TLS_KEYPAIR
        };

        bool verifyServerCertificate = true;

        AuthorizeVariants authVariant = AuthorizeVariants::NOT_SET;

    public:
        DDIClientBuilder *setHawkbitEndpoint(const std::string &) override;

        DDIClientBuilder *setDefaultPollingTimeout(int pollingTimeout_) override;

        DDIClientBuilder *setEventHandler(std::shared_ptr<EventHandler> handler) override;

        DDIClientBuilder *addHeader(const std::string &, const std::string &) override;

        DDIClientBuilder *setGatewayToken(const std::string &) override;

        DDIClientBuilder *setDeviceToken(const std::string &) override;

        DDIClientBuilder *notVerifyServerCertificate() override;


        DDIClientBuilder *setTLS(const std::string &crt, const std::string &key) override;

        DDIClientBuilder *setAuthErrorHandler(std::shared_ptr<AuthErrorHandler>) override;

        DDIClientBuilder *setHawkbitEndpoint(const std::string &endpoint,
                                             const std::string &controllerId_, const std::string &tenant_ = "default") override;

        std::unique_ptr<Client> build() override;
    };

}