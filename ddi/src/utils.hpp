#pragma once

#include "rapidjson/document.h"
#include "uriparse.hpp"


namespace ddi {

    uri::URI parseHrefObject(const rapidjson::Value &hrefObject);
}