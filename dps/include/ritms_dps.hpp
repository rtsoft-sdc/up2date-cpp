#pragma once

#include <string>
#include <memory>

namespace ritms {
    namespace dps {

        /** \page dpsModule  dps module description
        * This module used for provisioning in UP2Date services
        * \link ritms::dps::ProvisioningClient::doProvisioning You can begin from here \endlink
        */

        ///\brief Contains payload required to authorize in UP2Date services
        class mTLSKeyPair {
        public:
            ///\brief Get private key.
            virtual std::string getKey() = 0;

            ///\brief Get public key.
            virtual std::string getCrt() = 0;

            virtual ~mTLSKeyPair() = default;
        };

        ///\brief Contains data received from UP2DATE DPS service
        class ProvisioningData {
        public:

            ///\brief Get mTLSKeyPair.
            /// Used for authentication in up2date services.
            virtual std::unique_ptr<mTLSKeyPair> getKeyPair() = 0;

            ///\brief UP2DATE service endpoint (for client tenant)
            virtual std::string getUp2DateEndpoint() = 0;

            virtual ~ProvisioningData() = default;
        };

        ///\brief Make provisioning in UP2DATE service.
        class ProvisioningClient {
        public:

            ///\brief Do provisioning.
            virtual std::unique_ptr<ProvisioningData> doProvisioning() = 0;

            virtual ~ProvisioningClient() = default;
        };

        ///\brief Configure and build ritms::dps::ProvisioningClient.
        class CloudProvisioningClientBuilder {
        public:

            ///\brief Get ritms::dps::CloudProvisioningClientBuilder instance.
            static std::unique_ptr<CloudProvisioningClientBuilder> newInstance();

            ///\brief  Set client certificate (required).
            virtual CloudProvisioningClientBuilder *setAuthCrt(const std::string &crt) = 0;

            ///\brief  Set provisioning endpoint (required).
            virtual CloudProvisioningClientBuilder *setEndpoint(const std::string &endpoint) = 0;

            ///\brief  Set additional headers for provisioning client.
            virtual CloudProvisioningClientBuilder *addHeader(const std::string &, const std::string &) = 0;

            ///\brief Get ritms::dps::ProvisioningClient instance.
            virtual std::unique_ptr<ProvisioningClient> build() = 0;
        };

    }
}