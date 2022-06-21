#pragma once

#include <utility>

#include "ritms_dps.hpp"
#include "uriparse.hpp"
#include "httpclient.hpp"


namespace ritms {
    namespace dps {

        class mTLSKeyPair_impl : public mTLSKeyPair {
            std::string crt, key;

        public:
            mTLSKeyPair_impl(std::string crt_, std::string key_) : crt(std::move(crt_)), key(std::move(key_)) {};

            std::string getKey() override;
            std::string getCrt() override;
        };



        class ProvisioningData_impl : public ProvisioningData {
            std::unique_ptr<mTLSKeyPair_impl> keyPair;
            std::string up2DateEndpoint;

        public:

            ProvisioningData_impl(std::unique_ptr<mTLSKeyPair_impl> kp,
                                  std::string up2DateEndpoint_)
                                  : keyPair(std::move(kp)), up2DateEndpoint(std::move(up2DateEndpoint_)) {};

            std::unique_ptr<mTLSKeyPair> getKeyPair() override;
            std::string getUp2DateEndpoint() override;
        };


        class ProvisioningClient_impl : public ProvisioningClient {
            std::string crt;
            uri::URI provisioningURI;
            httpclient::Headers provisioningHeaders;

        public:
            std::string formatCertificateUpdatePayload();
            std::unique_ptr<ProvisioningData> doProvisioning() override;

            friend class CloudProvisioningClientBuilder_impl;
        };

        class CloudProvisioningClientBuilder_impl : public CloudProvisioningClientBuilder {
            std::string crt;
            uri::URI provisioningURI;
            httpclient::Headers provisioningHeaders;

        public:

            CloudProvisioningClientBuilder *setAuthCrt(const std::string &crt) override;

            CloudProvisioningClientBuilder *setEndpoint(const std::string &endpoint) override;

            CloudProvisioningClientBuilder *
            addHeader(const std::string &key, const std::string &val) override;

            std::unique_ptr<ProvisioningClient> build() override;
        };


    }
}