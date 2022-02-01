#pragma once

#include "rapidjson/document.h"
#include "uriparse.hpp"


namespace hawkbit {

    uri::URI parseHrefObject(const rapidjson::Value &hrefObject);
}