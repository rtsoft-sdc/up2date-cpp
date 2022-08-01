#include "ritms_token_exchanger_impl.hpp"

#define OTT_FIELD_NAME "oneTimeToken"
#define CERTIFICATE_FIELD_NAME "crt"

#define RAPIDJSON_HAS_STDSTRING 1

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "httpclient.hpp"
#include "ritms_exceptions.hpp"

namespace ritms {
    namespace dps {
        std::unique_ptr<TokenExchanger>
                TokenExchangerFactory::buildCloudExchanger(
                        const std::string& url,
                        const std::string& token) {

            auto exchangeUrl = uri::URI::fromString(url);

            return std::make_unique<TokenExchanger_impl>(exchangeUrl,token);
        }


        std::string TokenExchanger_impl::exchange() {
            rapidjson::Document document;
            document.SetObject();
            document.AddMember(OTT_FIELD_NAME, this->ott,
                               document.GetAllocator());

            rapidjson::StringBuffer buf;
            rapidjson::Writer<rapidjson::StringBuffer> writer(buf);
            document.Accept(writer);

            httpclient::Headers headers;
            auto resp = httpclient::Client(this->url.getScheme() +
                                           "://" + this->url.getAuthority())
                    .Post(this->url.getPath().c_str(), headers,
                          buf.GetString(), "application/json");
            if (resp.error() != httpclient::Error::Success) {
                throw httplib_error((int)resp.error());
            }
            if (resp->status != HTTP_OK && resp->status != HTTP_OK) {
                throw provisioning_error(resp->status);
            }

            rapidjson::Document responseDocument;
            responseDocument.Parse<0>(resp->body.c_str());

            if (responseDocument.HasParseError()
                    || !responseDocument.HasMember(CERTIFICATE_FIELD_NAME)) {
                throw up2date_cloud_error("Bad payload received from DPS");
            }

            return responseDocument[CERTIFICATE_FIELD_NAME].GetString();
        }
    }
}