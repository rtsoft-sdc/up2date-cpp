#pragma once
#include "httpclient.hpp"
#include "httplib.h"

namespace httpclient {

    class HttpLibClientImpl : public ClientImpl {
        httplib::Client cli;
    public:
        HttpLibClientImpl(httplib::Client cli_) : cli(std::move(cli_)) {};

        Result Get(const char *path, const Headers &headers) override;

        Result Get(const char *path, const Headers &headers, HttpClientResponseHandler response_handler,
                   HttpClientContentReceiver content_receiver) override;

        Result
        Post(const char *path, const Headers &headers, const std::string &body, const char *content_type) override;

        Result
        Put(const char *path, const Headers &headers, const std::string &body, const char *content_type) override;

        void enable_server_certificate_verification(bool b) override;
    };

}
