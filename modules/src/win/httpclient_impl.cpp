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

        config.scheme = scheme.c_str();
        config.path = path;

        config.host = host.c_str();

        config.headers = headersEncoded.c_str();
        config.headersSize = headersEncoded.size();

        config.body = (LPVOID) body;
        config.bodySize = bodySize;

        config.retCodeHandler = retCodeHandler;
        config.retCodeHandlerCtx = retCodeHandlerCtx;

        config.contentReceiver = contentReceiver;
        config.contentReceiverCtx = contentReceiverCtx;

        config.method = method;

        return (int) do_http_request(&config);
    }

    Result WINHttpLibClientImpl::Get(const char *path, const Headers &headers) {

        int statusCode = 0;
        std::string resp = "";


        int res = doHttpWinRequest(
                "GET", path, headers, &simpleStatusCodeReader, &statusCode,
                &stringDataReader, &resp, NULL, 0
        );

        if (res != 0) {
            std::cerr << "Error HTTP request with WIN API " << res << std::endl;
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
                &cbDataReader, &content_receiver, NULL, 0
        );

        if (res != 0) {
            std::cerr << "Error HTTP request with WIN API " << res << std::endl;
        }

        return {
                std::make_unique<Response>(handler.code, ""),
                (res == 0) ? Error::Success : Error::Unknown
        };
    }

    Result WINHttpLibClientImpl::Post(const char *path, const Headers &headers, const std::string &body,
                                      const char *content_type) {

        // TODO: implement
        return {
                std::make_unique<Response>(0, ""),
                Error::Success
        };
    }

    Result WINHttpLibClientImpl::Put(const char *path, const Headers &headers, const std::string &body,
                                     const char *content_type) {

        // TODO: implement
        return {
                std::make_unique<Response>(0, ""),
                Error::Success
        };
    }

    void WINHttpLibClientImpl::enable_server_certificate_verification(bool b) {
        // TODO: implement
    }

    WINHttpLibClientImpl::WINHttpLibClientImpl(const std::string &endpoint) {
        baseAddress = uri::URI::fromString(endpoint);
    }

    Client::Client(const std::string &endpoint) {
        client_ = std::make_unique<WINHttpLibClientImpl>(endpoint);
    }

    Client::Client(std::string endpoint, std::unique_ptr<mTLSKeyPair> kp) {
        client_ = std::make_unique<WINHttpLibClientImpl>(endpoint);
    }
}
