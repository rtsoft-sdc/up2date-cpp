#include "provisioning_client_impl.hpp"
#include "httplib.h"

#ifndef RAPIDJSON_HAS_STDSTRING
#define RAPIDJSON_HAS_STDSTRING 1
#endif

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

namespace hawkbit {

    void ProvisioningClientImpl::run() {
        updateCredentials();
        HawkbitCommunicationClient::run();
    }

    httplib::Result ProvisioningClientImpl::retryHandler(uri::URI uri1, const std::function<httplib::Result(
            httplib::Client &)> &function) {
        // if certificates become invalid we try to renew it
        try {
            return HawkbitCommunicationClient::retryHandler(uri1, function);
        } catch (unauthorized_exception&) {
            updateCredentials();
        }
        // if getting exception again not handling it
        return HawkbitCommunicationClient::retryHandler(uri1, function);
    }

    std::string formatCertificateUpdatePayload(const std::string& crt) {
        rapidjson::Document document;
        document.SetObject();
        document.AddMember("tq", crt, document.GetAllocator());

        rapidjson::StringBuffer buf;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buf);
        document.Accept(writer);

        return buf.GetString();
    }

    void ProvisioningClientImpl::updateCredentials() {
        auto resp = httplib::Client(provisioningURI.getScheme() +
                "://" + provisioningURI.getAuthority())
                        .Post(provisioningURI.getPath().c_str(), provisioningHeaders,
                              formatCertificateUpdatePayload(crt), "application/json");
        if (resp.error() != httplib::Error::Success) {
            throw http_lib_error((int)resp.error());
        }
        if (resp->status != HTTP_OK && resp->status != HTTP_CREATED) {
            throw http_unexpected_code_exception(resp->status, HTTP_OK);
        }

        rapidjson::Document document;
        document.Parse<0>(resp->body.c_str());

        if (document.HasParseError() || !document.HasMember("crt")
                || !document.HasMember("endpoint") || !document.HasMember("key"))
            throw unexpected_payload();

        provisioningPayload.crt = document["crt"].GetString();
        provisioningPayload.key = document["key"].GetString();
        hawkbitURI = uri::URI::fromString(document["endpoint"].GetString());
    }

    httplib::Client ProvisioningClientImpl::newHttpClient(uri::URI &uri1) {
        BIO *bio_crt = BIO_new(BIO_s_mem());
        BIO_puts(bio_crt, provisioningPayload.crt.c_str());
        X509 *certificate = PEM_read_bio_X509(bio_crt, nullptr, nullptr, nullptr);
        BIO_free(bio_crt);

        BIO *bio_key = BIO_new(BIO_s_mem());
        BIO_puts(bio_key, provisioningPayload.key.c_str());
        EVP_PKEY *key = PEM_read_bio_PrivateKey(bio_key, nullptr, nullptr, nullptr);
        BIO_free(bio_key);


        return httplib::Client(uri1.getScheme() + "://" + uri1.getAuthority(), certificate, key);
    }

}