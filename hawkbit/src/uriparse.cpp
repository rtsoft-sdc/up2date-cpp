#include <regex>

#include "uriparse.hpp"

namespace uri {
    // URI find regex
    const std::regex uri_regex(R"(^(([^:\/?#]+):)?(//([^\/?#]*))?([^?#]*)(\?([^#]*))?(#(.*))?)", std::regex::extended);

    bool URI::hasPath() {
        return !path.empty();
    };

    bool URI::hasQuery() {
        return !query.empty();
    }

    bool URI::hasFragment() {
        return !fragment.empty();
    }

    std::string URI::getScheme() {
        return scheme;
    }

    std::string URI::getAuthority() {
        return authority;
    }

    std::string URI::getPath() {
        return path;
    }

    std::string URI::getQuery() {
        return query;
    }

    std::string URI::getFragment() {
        return fragment;
    }

    URI URI::fromString(const std::string& path) {
        std::smatch url_match;

        if (!std::regex_match(path, url_match, uri_regex)) {
            throw std::runtime_error("Malformed url.");
        }

        if (!url_match[4].matched) {
            throw std::runtime_error("No scheme or authority");
        }
        URI retUri;
        // get scheme
        retUri.scheme = url_match[2];
        // get authority
        retUri.authority = url_match[4];

        if (!url_match[5].matched) {
            return retUri;
        }
        // get path
        retUri.path = url_match[5];

        if (!url_match[7].matched) {
            return retUri;
        }
        // get query
        retUri.query = url_match[7];

        if (!url_match[9].matched) {
            return retUri;
        }
        // get fragment
        retUri.fragment = url_match[9];
        return retUri;
    }

}