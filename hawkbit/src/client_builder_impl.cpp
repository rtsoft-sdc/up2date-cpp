#include <utility>

#include "hawkbit/hawkbit_client.hpp"
#include "provisioning_client_impl.hpp"

namespace hawkbit {
    std::unique_ptr<ProvisioningClientBuilder> ProvisioningClientBuilder::newInstance() {
        return std::unique_ptr<ProvisioningClientBuilder>(new ProvisioningClientBuilderImpl());
    }

    ProvisioningClientBuilder *ProvisioningClientBuilderImpl::setCrt(const std::string& root_) {
        crt = root_;

        return this;
    }

    ProvisioningClientBuilder *ProvisioningClientBuilderImpl::setProvisioningEndpoint(const std::string& provisioningEndpoint_) {
        provisioningURI = uri::URI::fromString(provisioningEndpoint_);

        return this;
    }

    ProvisioningClientBuilder *ProvisioningClientBuilderImpl::setDefaultPollingTimeout(int pollingTimeout_) {
        pollingTimeout = pollingTimeout_;

        return this;
    }

    ProvisioningClientBuilder *ProvisioningClientBuilderImpl::setEventHandler(std::shared_ptr<EventHandler> handler_) {
        handler = handler_;

        return this;
    }

    ProvisioningClientBuilder *ProvisioningClientBuilderImpl::addHeader(const std::string &key, const std::string &value) {
        defaultHeaders.insert({key, value});

        return this;
    }

    ProvisioningClientBuilder *
    ProvisioningClientBuilderImpl::addProvisioningHeader(const std::string &key, const std::string &value) {
        provisioningHeaders.insert({key, value});

        return this;
    }

    std::unique_ptr<Client> ProvisioningClientBuilderImpl::build() {
        auto cli = new ProvisioningClientImpl();
        auto cliPtr = std::unique_ptr<Client>(cli);
        cli->defaultHeaders = defaultHeaders;
        cli->provisioningHeaders = provisioningHeaders;
        cli->provisioningURI = provisioningURI;
        cli->defaultSleepTime = pollingTimeout;
        cli->currentSleepTime = pollingTimeout;
        cli->crt = crt;
        cli->handler = handler;

        return cliPtr;
    }

}