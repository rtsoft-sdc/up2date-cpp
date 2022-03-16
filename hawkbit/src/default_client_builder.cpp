#include "hawkbit/hawkbit_client.hpp"
#include "hawkbit_client_impl.hpp"
#include "uriparse.hpp"

namespace hawkbit {
    std::unique_ptr<DefaultClientBuilder> DefaultClientBuilder::newInstance() {
        return std::unique_ptr<DefaultClientBuilder>(new DefaultClientBuilderImpl());
    }

    DefaultClientBuilder *DefaultClientBuilderImpl::setEventHandler(std::shared_ptr<EventHandler> handler_) {
        handler = handler_;

        return this;
    }

    DefaultClientBuilder *DefaultClientBuilderImpl::setHawkbitEndpoint(const std::string &endpoint) {
        this->hawkbitUri = uri::URI::fromString(endpoint);

        return this;
    }

    DefaultClientBuilder *DefaultClientBuilderImpl::setDefaultPollingTimeout(int pollingTimeout_) {
        pollingTimeout = pollingTimeout_;

        return this;
    }

    DefaultClientBuilder *DefaultClientBuilderImpl::addHeader(const std::string &k, const std::string &v) {
        defaultHeaders.insert({k, v});

        return this;
    }

    DefaultClientBuilder *DefaultClientBuilderImpl::setGatewayToken(const std::string &token) {
        if (currentVariant != AuthorizeVariants::NOT_SET) {
            throw std::runtime_error("Another authority type is already set");
        }

        addHeader("Authorization", "GatewayToken " + token);
        currentVariant = AuthorizeVariants::GATEWAY_TOKEN;

        return this;
    }

    DefaultClientBuilder *DefaultClientBuilderImpl::setDeviceToken(const std::string &token) {
        if (currentVariant != AuthorizeVariants::NOT_SET) {
            throw std::runtime_error("Another authority type is already set");
        }

        addHeader("Authorization", "TargetToken " + token);
        currentVariant = AuthorizeVariants::DEVICE_TOKEN;

        return this;
    }

    DefaultClientBuilder *DefaultClientBuilderImpl::notVerifyServerCertificate() {
        verifyServerCertificate = false;

        return this;
    }

    DefaultClientBuilder *DefaultClientBuilderImpl::setTenant(const std::string &tenant_) {
        tenant = tenant_;

        return this;
    }

    DefaultClientBuilder *DefaultClientBuilderImpl::setControllerId(const std::string &controllerId_) {
        controllerId = controllerId_;

        return this;
    }

    std::unique_ptr<Client> DefaultClientBuilderImpl::build() {
        auto cli = new HawkbitCommunicationClient();
        auto cliPtr = std::unique_ptr<Client>(cli);

        auto requestUri =
                hawkbitUri.getScheme() + "://" + hawkbitUri.getAuthority() + "/" + tenant + "/controller/v1/" +
                controllerId;

        cli->hawkbitURI = uri::URI::fromString(requestUri);
        cli->defaultSleepTime = pollingTimeout;
        cli->currentSleepTime = pollingTimeout;
        cli->handler = handler;
        cli->defaultHeaders = defaultHeaders;
        cli->defaultHeaders = defaultHeaders;
        cli->serverCertificateVerify = verifyServerCertificate;

        return cliPtr;
    }
}