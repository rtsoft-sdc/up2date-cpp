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

        // Up2DatePayload_impl -----------------------------------------------------------------------------

        bool Up2DatePayload_impl::hasUp2DateEndpoint() {
            return true;
        }

        std::string Up2DatePayload_impl::getUp2DateEndpoint() {
            return endpoint;
        }

        // ProvisioningData_impl ---------------------------------------------------------------------------

        std::unique_ptr<mTLSKeyPair> ProvisioningData_impl::getKeyPair() {

            return std::unique_ptr<mTLSKeyPair>(
                    new mTLSKeyPair_impl(*this->keyPair)
            );
        }

        std::unique_ptr<Up2DatePayload> ProvisioningData_impl::getUp2DatePayload() {

            return std::unique_ptr<Up2DatePayload>(
                new Up2DatePayload_impl(*this->up2DatePayload)
            );
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
        CloudProvisioningClientBuilder_impl::setProvisioningEndpoint(const std::string &endpoint) {
            this->provisioningURI = uri::URI::fromString(endpoint);

            return this;
        }

        CloudProvisioningClientBuilder *
        CloudProvisioningClientBuilder_impl::addAdditionalProvisioningHeader(const std::string &key,
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