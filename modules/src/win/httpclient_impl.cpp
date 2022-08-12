#include "httpclient_impl.hpp"
#include <iostream>


namespace httpclient {

    static std::string formatHeaders(const Headers &headers) {

        std::string res;
        for (const auto &header: headers) {
            res += header.first + ": " + header.second + "\r\n";
        }
        res += "\r\n";
        return res;
    }

    static BOOL stringDataReader(LPCH data, DWORD dataRead, LPVOID strCtx) {

        ((std::string *) strCtx)->append(data, dataRead);
        return TRUE;
    }

    static BOOL simpleStatusCodeReader(INT code, LPVOID intCtx) {

        *((int *) intCtx) = code;
        return TRUE;
    }


    static BOOL cbDataReader(LPCH data, DWORD dataRead, LPVOID cbCtx) {

        if ((*((HttpClientContentReceiver *)cbCtx))(data, dataRead)) {
            return TRUE;
        }

        return FALSE;
    }

    typedef struct  {
        INT code;
        HttpClientResponseHandler* rs;
    } codeHandler;

    static BOOL cbStatusCodeReader(INT code, LPVOID cbCtx) {
        Response res {
            code,
            ""
        };

        ((codeHandler *)cbCtx)->code = code;
        if ((*(((codeHandler *)cbCtx)->rs))(res)) {
            return TRUE;
        }
        return FALSE;
    }

    int WINHttpLibClientImpl::doHttpWinRequest(const char *method,
                                               const char *path,
                                               const Headers &headers,
                                               BOOL (*retCodeHandler)(INT code, LPVOID ctx),
                                               LPVOID retCodeHandlerCtx,
                                               BOOL (*contentReceiver)(LPCH data, DWORD dataRead, LPVOID ctx),
                                               LPVOID contentReceiverCtx,
                                               const char *body, DWORD bodySize) {

        std::string headersEncoded = formatHeaders(headers);
        std::string scheme = baseAddress.getScheme();
        std::string host = baseAddress.getAuthority();

        struct request_config config{};

        config.bVerifyServerCrt = (serverVerify) ? TRUE : FALSE;

        config.szScheme = scheme.c_str();
        config.szPath = path;

        config.szHost = host.c_str();

        config.szHeaders = headersEncoded.c_str();
        config.dwHeadersSize = headersEncoded.size();

        config.pvBody = (LPVOID) body;
        config.dwBodySize = bodySize;

        config.pfRetCodeHandler = retCodeHandler;
        config.pvRetCodeHandlerCtx = retCodeHandlerCtx;

        config.pfContentReceiver = contentReceiver;
        config.pvContentReceiverCtx = contentReceiverCtx;

        config.szMethod = method;


        if (kp) {
            struct mtls_keypair keypair{};
            keypair.cszKey = kp->key.c_str();
            keypair.dwKeySize = kp->key.size();
            keypair.cszCrt = kp->crt.c_str();
            keypair.dwCrtSize = kp->crt.size();

            return (int) do_http_request_mtls(&config, &keypair);
        }

        return (int) do_http_request(&config);
    }

    Result WINHttpLibClientImpl::Get(const char *path, const Headers &headers) {

        int statusCode = 0;
        std::string resp;


        int res = doHttpWinRequest(
                "GET", path, headers, &simpleStatusCodeReader, &statusCode,
                &stringDataReader, &resp, nullptr, 0
        );

        if (res != 0) {
            std::cerr << "Error Get HTTP request with WIN API " << res << std::endl;
        }

        return {
                std::make_unique<Response>(statusCode, resp),
                (res == 0) ? Error::Success : Error::Unknown
        };
    }

    Result
    WINHttpLibClientImpl::Get(const char *path, const Headers &headers, HttpClientResponseHandler response_handler,
                              HttpClientContentReceiver content_receiver) {
        codeHandler handler {};
        handler.rs = &response_handler;

        int res = doHttpWinRequest(
                "GET", path, headers, &cbStatusCodeReader, &handler,
                &cbDataReader, &content_receiver, nullptr, 0
        );

        if (res != 0) {
            std::cerr << "Error Get(cb) HTTP request with WIN API " << res << std::endl;
        }

        return {
                std::make_unique<Response>(handler.code, ""),
                (res == 0) ? Error::Success : Error::Unknown
        };
    }

    Result WINHttpLibClientImpl::Post(const char *path, const Headers &headers, const std::string &body,
                                      const char *content_type) {

        int statusCode = 0;
        std::string resp;

        Headers ctiHeaders = headers;
        ctiHeaders.insert({"Content-Type", content_type});


        int res = doHttpWinRequest(
                "POST", path, ctiHeaders, &simpleStatusCodeReader, &statusCode,
                &stringDataReader, &resp, body.c_str(), body.size()
        );

        if (res != 0) {
            std::cerr << "Error Post HTTP request with WIN API " << res << std::endl;
        }

        return {
                std::make_unique<Response>(statusCode, resp),
                (res == 0) ? Error::Success : Error::Unknown
        };
    }

    Result WINHttpLibClientImpl::Put(const char *path, const Headers &headers, const std::string &body,
                                     const char *content_type) {

        int statusCode = 0;
        std::string resp;

        Headers ctiHeaders = headers;
        ctiHeaders.insert({"Content-Type", content_type});

        int res = doHttpWinRequest(
                "PUT", path, ctiHeaders, &simpleStatusCodeReader, &statusCode,
                &stringDataReader, &resp, body.c_str(), body.size()
        );

        if (res != 0) {
            std::cerr << "Error Put HTTP request with WIN API " << res << std::endl;
        }

        return {
                std::make_unique<Response>(statusCode, resp),
                (res == 0) ? Error::Success : Error::Unknown
        };
    }

    void WINHttpLibClientImpl::enable_server_certificate_verification(bool b) {
        // TODO: test it
        serverVerify = b;
    }

    WINHttpLibClientImpl::WINHttpLibClientImpl(const std::string &endpoint, std::unique_ptr<mTLSKeyPair> kp_) {
        baseAddress = uri::URI::fromString(endpoint);
        if (kp_ != nullptr) {
            kp = std::move(kp_);
        }
    }

    Client::Client(const std::string &endpoint) {
        client_ = std::make_unique<WINHttpLibClientImpl>(endpoint, nullptr);
    }

    Client::Client(const std::string& endpoint, std::unique_ptr<mTLSKeyPair> kp_) {
        client_ = std::make_unique<WINHttpLibClientImpl>(endpoint, std::move(kp_));
    }
}
