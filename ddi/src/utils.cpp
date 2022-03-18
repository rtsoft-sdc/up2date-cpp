#include "utils.hpp"
#include "ddi/hawkbit_exceptions.hpp"

namespace ddi {
    uri::URI parseHrefObject(const rapidjson::Value &hrefObject) {
        try {
            return uri::URI::fromString(hrefObject["href"].GetString());
        } catch (std::exception &) {
            throw unexpected_payload();
        }
    }

    std::string
    hawkbitEndpointFrom(const std::string &endpoint, const std::string &controllerId_, const std::string &tenant_) {
        auto hawkbitEndpoint = uri::URI::fromString(endpoint);
        return hawkbitEndpoint.getScheme() + "://" + hawkbitEndpoint.getAuthority() + "/" + tenant_ +
               "/controller/v1/" +
               controllerId_;
    }
}