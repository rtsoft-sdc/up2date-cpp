#include "httpclient_impl.hpp"

namespace httpclient {

    Result toHttpLibResult(httplib::Result r) {
        Error e = r.error();
        return {
            (e == Error::Success) ? std::make_unique<Response>(r->status, r->body) : nullptr,
            e
        };
    }

    httplib::Headers toHttpLibHeaders(const Headers& headers) {

        return {headers.begin(), headers.end()};
    }

    Result HttpLibClientImpl::Get(const char *path, const Headers &headers) {

        return toHttpLibResult(cli.Get(path, toHttpLibHeaders(headers)));
    }

    Result HttpLibClientImpl::Get(const char *path, const Headers &headers, HttpClientResponseHandler response_handler,
                                  HttpClientContentReceiver content_receiver) {

        return toHttpLibResult(cli.Get(path, toHttpLibHeaders(headers),
                                       [&response_handler](const httplib::Response &r) {
            return response_handler(Response(r.status, ""));
        }, content_receiver));
    }

    Result HttpLibClientImpl::Post(const char *path, const Headers &headers, const std::string &body,
                                   const char *content_type) {
        return toHttpLibResult(cli.Post(path, toHttpLibHeaders(headers), body, content_type));
    }

    Result HttpLibClientImpl::Put(const char *path, const Headers &headers, const std::string &body,
                                  const char *content_type) {

        return toHttpLibResult(cli.Put(path, toHttpLibHeaders(headers), body, content_type));
    }

    void HttpLibClientImpl::enable_server_certificate_verification(bool b) {

        this->cli.enable_server_certificate_verification(b);
    }

    Client::Client(const std::string& endpoint) {

        client_ = std::make_unique<HttpLibClientImpl>(httplib::Client(endpoint));
    }

    Client::Client(std::string endpoint, std::unique_ptr<mTLSKeyPair> kp) {
        BIO *bio_crt = BIO_new(BIO_s_mem());
        BIO_puts(bio_crt, kp->crt.c_str());
        X509 *certificate = PEM_read_bio_X509(bio_crt, nullptr, nullptr, nullptr);
        BIO_free(bio_crt);

        BIO *bio_key = BIO_new(BIO_s_mem());
        BIO_puts(bio_key, kp->key.c_str());
        EVP_PKEY *key = PEM_read_bio_PrivateKey(bio_key, nullptr, nullptr, nullptr);
        BIO_free(bio_key);
        client_ = std::make_unique<HttpLibClientImpl>(
                httplib::Client(endpoint, certificate, key));
    }
}
