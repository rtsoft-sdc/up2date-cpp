#pragma once

#include "rapidjson/document.h"
#include "uriparse.hpp"


namespace ddi {

    uri::URI parseHrefObject(const rapidjson::Value &hrefObject);

    std::string hawkbitEndpointFrom(const std::string &endpoint, const std::string &controllerId_, const std::string &tenant_);
}