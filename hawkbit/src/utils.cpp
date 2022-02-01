#include "utils.hpp"
#include "hawkbit/exceptions.hpp"

namespace hawkbit {
    uri::URI parseHrefObject(const rapidjson::Value &hrefObject) {
        try {
            return uri::URI::fromString(hrefObject["href"].GetString());
        } catch (std::exception) {
            throw unexpected_payload();
        }
    }
}