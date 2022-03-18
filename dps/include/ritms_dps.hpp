#pragma once

#include <string>
#include <memory>

namespace ritms {
    namespace dps {

        // mTLSKeyPair contains payload required to authorize in up2date services
        class mTLSKeyPair {
        public:
            // Get private key.
            virtual std::string getKey() = 0;

            // Get public key.
            virtual std::string getCrt() = 0;

            virtual ~mTLSKeyPair() = default;
        };

        // ProvisioningData contains data received from up2date DPS service
        class ProvisioningData {
        public:

            // Get mTLSKeyPair. Used for authentication in up2date services
            virtual std::unique_ptr<mTLSKeyPair> getKeyPair() = 0;

            //  up2date service endpoint (for client tenant)
            virtual std::string getUp2DateEndpoint() = 0;

            virtual ~ProvisioningData() = default;
        };

        // ProvisioningClient uses for provisioning in ritms service
        class ProvisioningClient {
        public:

            // Request to server, try to do provisioning.
            virtual std::unique_ptr<ProvisioningData> doProvisioning() = 0;

            virtual ~ProvisioningClient() = default;
        };

        // ProvisioningClient builder for default cloud provisioning
        class CloudProvisioningClientBuilder {
        public:

            static std::unique_ptr<CloudProvisioningClientBuilder> newInstance();

            // Set client certificate (required).
            virtual CloudProvisioningClientBuilder *setAuthCrt(const std::string &crt) = 0;

            // Set provisioning endpoint (required).
            virtual CloudProvisioningClientBuilder *setEndpoint(const std::string &endpoint) = 0;

            // Set additional headers for provisioning client.
            virtual CloudProvisioningClientBuilder *addHeader(const std::string &, const std::string &) = 0;

            virtual std::unique_ptr<ProvisioningClient> build() = 0;
        };

    }
}