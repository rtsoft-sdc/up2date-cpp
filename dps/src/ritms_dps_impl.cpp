#include "ritms_dps_impl.hpp"

namespace ritms {
    namespace dps {

        // mTLSKeyPair_impl --------------------------------------------------------------------------------

        std::string mTLSKeyPair_impl::getKey() {
            return key;
        }

        std::string mTLSKeyPair_impl::getCrt() {
            return crt;
        }

        // ProvisioningData_impl ---------------------------------------------------------------------------

        std::unique_ptr<mTLSKeyPair> ProvisioningData_impl::getKeyPair() {

            return std::unique_ptr<mTLSKeyPair>(
                    new mTLSKeyPair_impl(*this->keyPair)
            );
        }

        std::string ProvisioningData_impl::getUp2DateEndpoint() {
            return up2DateEndpoint;
        }


        // CloudProvisioningClientBuilder_impl -------------------------------------------------------------

        std::unique_ptr<CloudProvisioningClientBuilder> CloudProvisioningClientBuilder::newInstance() {

            return std::unique_ptr<CloudProvisioningClientBuilder>(new CloudProvisioningClientBuilder_impl());
        }

        CloudProvisioningClientBuilder *CloudProvisioningClientBuilder_impl::setAuthCrt(const std::string &crt_) {
            this->crt = crt_;

            return this;
        }

        CloudProvisioningClientBuilder *
        CloudProvisioningClientBuilder_impl::setEndpoint(const std::string &endpoint) {
            this->provisioningURI = uri::URI::fromString(endpoint);

            return this;
        }

        CloudProvisioningClientBuilder *
        CloudProvisioningClientBuilder_impl::addHeader(const std::string &key,
                                                       const std::string &val) {
            this->provisioningHeaders.insert({key, val});

            return this;
        }

        std::unique_ptr<ProvisioningClient> CloudProvisioningClientBuilder_impl::build() {
            auto provisioningClient = new ProvisioningClient_impl();
            auto cli = std::unique_ptr<ProvisioningClient>(provisioningClient);

            provisioningClient->provisioningHeaders = provisioningHeaders;
            provisioningClient->crt = crt;
            provisioningClient->provisioningURI = provisioningURI;

            return cli;
        }
    }
}