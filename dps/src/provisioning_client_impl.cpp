#include <memory>

#include "ritms_dps_impl.hpp"
#include "ritms_exceptions.hpp"
#include "httplib.h"

#define RAPIDJSON_HAS_STDSTRING 1

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"


namespace ritms {
    namespace dps {

        std::string ProvisioningClient_impl::formatCertificateUpdatePayload() {
            rapidjson::Document document;
            document.SetObject();
            document.AddMember("tq", crt, document.GetAllocator());

            rapidjson::StringBuffer buf;
            rapidjson::Writer<rapidjson::StringBuffer> writer(buf);
            document.Accept(writer);

            return buf.GetString();
        }

        std::unique_ptr<ProvisioningData> ProvisioningClient_impl::doProvisioning() {
            auto resp = httplib::Client(provisioningURI.getScheme() +
                                        "://" + provisioningURI.getAuthority())
                    .Post(provisioningURI.getPath().c_str(), provisioningHeaders,
                          formatCertificateUpdatePayload(), "application/json");
            if (resp.error() != httplib::Error::Success) {
                throw httplib_error((int)resp.error());
            }
            if (resp->status != HTTP_OK && resp->status != HTTP_CREATED) {
                throw provisioning_error(resp->status);
            }

            rapidjson::Document document;
            document.Parse<0>(resp->body.c_str());

            if (document.HasParseError() || !document.HasMember("crt")
                || !document.HasMember("endpoint") || !document.HasMember("key"))
                throw up2date_cloud_error("Bad payload received from DPS");

            return std::unique_ptr<ProvisioningData>(
                    new ProvisioningData_impl(
                        std::make_unique<mTLSKeyPair_impl>(
                           document["crt"].GetString(), document["key"].GetString()
                        ),
                        document["endpoint"].GetString()
                    )
            );
        }

    }
}