#include <stdexcept>
#include "ddi/ddi_client.hpp"
#include "ddi_client_impl.hpp"
#include "uriparse.hpp"
#include "utils.hpp"

namespace ddi {
    std::unique_ptr<DDIClientBuilder> DDIClientBuilder::newInstance() {
        return std::unique_ptr<DDIClientBuilder>(new DefaultClientBuilderImpl());
    }

    DDIClientBuilder *DefaultClientBuilderImpl::setEventHandler(std::shared_ptr<EventHandler> handler_) {
        handler = handler_;

        return this;
    }

    DDIClientBuilder *DefaultClientBuilderImpl::setHawkbitEndpoint(const std::string &endpoint) {
        this->hawkbitUri = endpoint;

        return this;
    }

    DDIClientBuilder *DefaultClientBuilderImpl::setHawkbitEndpoint(const std::string &endpoint,
                                                                   const std::string &controllerId,
                                                                   const std::string &tenant) {

        return  setHawkbitEndpoint(hawkbitEndpointFrom(endpoint, controllerId, tenant));
    }

    DDIClientBuilder *DefaultClientBuilderImpl::setDefaultPollingTimeout(int pollingTimeout_) {
        pollingTimeout = pollingTimeout_;

        return this;
    }

    DDIClientBuilder *DefaultClientBuilderImpl::addHeader(const std::string &k, const std::string &v) {
        defaultHeaders.insert({k, v});

        return this;
    }

    DDIClientBuilder *DefaultClientBuilderImpl::setGatewayToken(const std::string &token_) {
        if (authVariant != AuthorizeVariants::NOT_SET) {
            throw std::runtime_error("Another authority type is already set");
        }

        token = token_;
        authVariant = AuthorizeVariants::GATEWAY_TOKEN;

        return this;
    }

    DDIClientBuilder *DefaultClientBuilderImpl::setDeviceToken(const std::string &token_) {
        if (authVariant != AuthorizeVariants::NOT_SET) {
            throw std::runtime_error("Another authority type is already set");
        }

        token = token_;
        authVariant = AuthorizeVariants::DEVICE_TOKEN;

        return this;
    }

    DDIClientBuilder *DefaultClientBuilderImpl::setTLS(const std::string &crt_, const std::string &key_) {
        if (authVariant != AuthorizeVariants::NOT_SET) {
            throw std::runtime_error("Another authority type is already set");
        }
        crt = crt_;
        key = key_;
        authVariant = AuthorizeVariants::M_TLS_KEYPAIR;

        return this;
    }

    DDIClientBuilder *DefaultClientBuilderImpl::notVerifyServerCertificate() {
        verifyServerCertificate = false;

        return this;
    }


    DDIClientBuilder *DefaultClientBuilderImpl::setAuthErrorHandler(std::shared_ptr<AuthErrorHandler> e) {
        authErrorHandler = e;

        return this;
    }

    std::unique_ptr<Client> DefaultClientBuilderImpl::build() {
        auto cli = new HawkbitCommunicationClient();
        auto cliPtr = std::unique_ptr<Client>(cli);

        if (!hawkbitUri.empty())
            cli->setEndpoint(hawkbitUri);

        cli->defaultSleepTime = pollingTimeout;
        cli->currentSleepTime = pollingTimeout;
        cli->handler = handler;
        cli->defaultHeaders = defaultHeaders;
        cli->serverCertificateVerify = verifyServerCertificate;
        cli->authErrorHandler = authErrorHandler;
        cli->isRunning = false;

        if (authVariant == AuthorizeVariants::M_TLS_KEYPAIR) {
            cli->setTLS(crt, key);
        } else if (authVariant == AuthorizeVariants::GATEWAY_TOKEN) {
            cli->setGatewayToken(token);
        } else if (authVariant == AuthorizeVariants::DEVICE_TOKEN) {
            cli->setDeviceToken(token);
        }

        return cliPtr;
    }

}