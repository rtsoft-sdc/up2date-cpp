#pragma once

#include <string>
#include <memory>

#include "httplib.h"
#include "uriparse.hpp"
#include "hawkbit/hawkbit_event_handler.hpp"
#include "hawkbit/exceptions.hpp"
#include "actions_impl.hpp"
#include "hawkbit/hawkbit_client.hpp"

namespace hawkbit {

    struct PollingData_;

    class HawkbitCommunicationClient : public DownloadProvider, public Client {
    protected:
        uri::URI hawkbitURI;
        httplib::Headers defaultHeaders;
        std::shared_ptr<EventHandler> handler;
        int defaultSleepTime;
        int currentSleepTime;
        bool ignoreSleep;
        bool serverCertificateVerify = true;

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
        virtual httplib::Result retryHandler(uri::URI, const std::function<httplib::Result(httplib::Client&)> &);
        // creates httpClient with predefined params
        virtual httplib::Client newHttpClient(uri::URI &);

    public:

        [[noreturn]] virtual void run() override;

        void downloadTo(uri::URI uri, const std::string& path) override;

        std::string getBody(uri::URI uri) override;

        void downloadWithReceiver(uri::URI uri, std::function<bool(const char *, size_t)> function) override;

        friend class DefaultClientBuilderImpl;
    };

    class DefaultClientBuilderImpl: public DefaultClientBuilder{
        uri::URI hawkbitUri;
        int pollingTimeout = 30000;
        std::shared_ptr<EventHandler> handler;
        httplib::Headers defaultHeaders;
        bool verifyServerCertificate = true;
        std::string tenant = "default";
        std::string controllerId;

        enum AuthorizeVariants {
            NOT_SET,
            GATEWAY_TOKEN,
            DEVICE_TOKEN
        };

        AuthorizeVariants currentVariant = AuthorizeVariants::NOT_SET;

    public:
        DefaultClientBuilder *setHawkbitEndpoint(const std::string &) override;

        DefaultClientBuilder *setDefaultPollingTimeout(int pollingTimeout_) override;

        DefaultClientBuilder *setEventHandler(std::shared_ptr<EventHandler> handler) override;

        DefaultClientBuilder *addHeader(const std::string &, const std::string &) override;

        DefaultClientBuilder *setGatewayToken(const std::string &) override;

        DefaultClientBuilder *setDeviceToken(const std::string &) override;

        DefaultClientBuilder *notVerifyServerCertificate() override;

        DefaultClientBuilder *setTenant(const std::string &) override;

        DefaultClientBuilder *setControllerId(const std::string &) override;

        std::unique_ptr<Client> build() override;

    };

}