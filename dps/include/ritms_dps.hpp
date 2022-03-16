#pragma once

#include <string>
#include <memory>

namespace ritms {
    namespace dps {

        // mTLSKeyPair contains payload required to authorize in up2date services
        class mTLSKeyPair {
        public:
            // get private key
            virtual std::string getKey() = 0;

            // get public key
            virtual std::string getCrt() = 0;

            virtual ~mTLSKeyPair() = default;
        };

        // payload needed for up2date service functionality
        class Up2DatePayload {
        public:

            //  up2date service endpoint (for client tenant)
            virtual bool hasUp2DateEndpoint() = 0;
            virtual std::string getUp2DateEndpoint() = 0;

            virtual ~Up2DatePayload() = default;
        };

        // ProvisioningData contains data received from up2date DPS service
        class ProvisioningData {
        public:

            // get mTLSKeyPair should be presented in provisioning data
            virtual std::unique_ptr<mTLSKeyPair> getKeyPair() = 0;

            // can get Up2DatePayload class if exists
            virtual std::unique_ptr<Up2DatePayload> getUp2DatePayload() = 0;

            virtual ~ProvisioningData() = default;
        };

        // ProvisioningClient uses for provisioning in ritms service
        class ProvisioningClient {
        public:

            virtual std::unique_ptr<ProvisioningData> doProvisioning() = 0;

            virtual ~ProvisioningClient() = default;
        };

        // ProvisioningClient builder for default cloud provisioning
        class CloudProvisioningClientBuilder {
        public:

            static std::unique_ptr<CloudProvisioningClientBuilder> newInstance();

            // set client certificate (required)
            virtual CloudProvisioningClientBuilder *setAuthCrt(const std::string &crt) = 0;

            // set provisioning endpoint (required)
            virtual CloudProvisioningClientBuilder *setProvisioningEndpoint(const std::string &endpoint) = 0;

            // can set additional headers for provisioning client
            virtual CloudProvisioningClientBuilder *addAdditionalProvisioningHeader(const std::string &, const std::string &) = 0;

            virtual std::unique_ptr<ProvisioningClient> build() = 0;
        };

    }
}