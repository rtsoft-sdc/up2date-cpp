#pragma once
#include <string>
#include <map>
#include <memory>
#include <utility>
#include <functional>

#include "httpclient_def.h"

namespace httpclient {
    struct mTLSKeyPair {
        std::string crt;
        std::string key;

    public:
        mTLSKeyPair(std::string crt_, std::string key_) {
            crt = std::move(crt_);
            key = std::move(key_);
        }
    };

    struct Response {
        int status = -1;
        std::string body;
    public:
        Response(int status_, std::string body_): status(status_), body(std::move(body_)) {};
    };

    using Headers = std::multimap<std::string, std::string>;
    using HttpClientContentReceiver = std::function<bool(const char *data, size_t data_length)>;
    using HttpClientResponseHandler = std::function<bool(const Response &response)>;

    class Result {
    public:
        Result(std::unique_ptr<Response> &&res, Error err)
                : res_(std::move(res)), err_(err) {}

        Error error() const { return err_; }

        bool operator==(std::nullptr_t) const { return res_ == nullptr; }
        bool operator!=(std::nullptr_t) const { return res_ != nullptr; }
        const Response &value() const { return *res_; }
        Response &value() { return *res_; }
        const Response &operator*() const { return *res_; }
        Response &operator*() { return *res_; }
        const Response *operator->() const { return res_.get(); }
        Response *operator->() { return res_.get(); }

    private:
        std::unique_ptr<Response> res_;
        Error err_;
    };


    class ClientImpl {
    public:
        virtual Result Get(const char *path, const Headers &headers) = 0;

        virtual Result Get(const char *path,  const Headers &headers, HttpClientResponseHandler response_handler,
                           HttpClientContentReceiver content_receiver) = 0;

        virtual Result Post(const char *path, const Headers &headers, const std::string &body,
                    const char *content_type) = 0;
        virtual Result Put(const char *path, const Headers &headers, const std::string &body,
                   const char *content_type) = 0;

        virtual void enable_server_certificate_verification(bool) = 0;

        virtual ~ClientImpl() = default;
    };

    class Client {
        std::unique_ptr<ClientImpl> client_;
    public:
        explicit Client(const std::string& endpoint, std::unique_ptr<mTLSKeyPair>);
        explicit Client(const std::string& endpoint);

        Result Get(const char *path, const Headers &headers) {
            return client_->Get(path, headers);
        }

        Result Get(const char *path,  const Headers &headers, HttpClientResponseHandler response_handler,
                   HttpClientContentReceiver content_receiver) {
            return client_->Get(path, headers,
                                std::move(response_handler),
                                std::move(content_receiver));
        }

        Result Post(const char *path, const Headers &headers, const std::string &body,
                    const char *content_type) {
            return client_->Post(path, headers, body, content_type);
        }

        Result Put(const char *path, const Headers &headers, const std::string &body,
                    const char *content_type) {
            return client_->Put(path, headers, body, content_type);
        }

        void enable_server_certificate_verification(bool enable) {
            return client_->enable_server_certificate_verification(enable);
        }


        Client(Client &&) = default;
    };
}