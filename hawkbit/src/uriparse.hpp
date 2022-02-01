#pragma once

#include <string>

namespace uri {
    class URI {
        std::string scheme;
        std::string authority;
        std::string path;
        std::string query;
        std::string fragment;

    public:
        bool hasPath();

        bool hasQuery();

        bool hasFragment();

        std::string getScheme();

        std::string getAuthority();

        std::string getPath();

        std::string getQuery();

        std::string getFragment();

        static URI fromString(const std::string&);
    };

}