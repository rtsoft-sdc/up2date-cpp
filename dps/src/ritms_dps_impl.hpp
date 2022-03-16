#pragma once

#include "ritms_dps.hpp"
#include "uriparse.hpp"
#include "httplib.h"


namespace ritms {
    namespace dps {

        class mTLSKeyPair_impl : public mTLSKeyPair {
            std::string crt, key;

        public:
            mTLSKeyPair_impl(const std::string &crt_, const std::string &key_) : crt(crt_), key(key_) {};

            std::string getKey() override;
            std::string getCrt() override;
        };


        class Up2DatePayload_impl : public Up2DatePayload {
            std::string endpoint;

        public:
            // as this endpoint required, set endpoint in constructor
            Up2DatePayload_impl(const std::string& endpoint_) : endpoint(endpoint_) {};

            bool hasUp2DateEndpoint() override;
            std::string getUp2DateEndpoint() override;
        };


        class ProvisioningData_impl : public ProvisioningData {
            std::unique_ptr<mTLSKeyPair_impl> keyPair;
            std::unique_ptr<Up2DatePayload_impl> up2DatePayload;

        public:

            ProvisioningData_impl(std::unique_ptr<mTLSKeyPair_impl> kp,
                                  std::unique_ptr<Up2DatePayload_impl> up)
                                  : keyPair(std::move(kp)), up2DatePayload(std::move(up)) {};

            std::unique_ptr<mTLSKeyPair> getKeyPair() override;
            std::unique_ptr<Up2DatePayload> getUp2DatePayload() override;
        };


        class ProvisioningClient_impl : public ProvisioningClient {
            std::string crt;
            uri::URI provisioningURI;
            httplib::Headers provisioningHeaders;

        public:
            std::string formatCertificateUpdatePayload();
            std::unique_ptr<ProvisioningData> doProvisioning() override;

            friend class CloudProvisioningClientBuilder_impl;
        };

        class CloudProvisioningClientBuilder_impl : public CloudProvisioningClientBuilder {
            std::string crt;
            uri::URI provisioningURI;
            httplib::Headers provisioningHeaders;

        public:

            CloudProvisioningClientBuilder *setAuthCrt(const std::string &crt) override;

            CloudProvisioningClientBuilder *setProvisioningEndpoint(const std::string &endpoint) override;

            CloudProvisioningClientBuilder *
            addAdditionalProvisioningHeader(const std::string &key, const std::string &val) override;

            std::unique_ptr<ProvisioningClient> build() override;
        };


    }
}