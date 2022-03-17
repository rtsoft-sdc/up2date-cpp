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
}