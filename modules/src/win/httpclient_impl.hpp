#pragma once

#include "httpclient.hpp"
#include "uriparse.hpp"
#include "win_http.h"

namespace httpclient {

    class WINHttpLibClientImpl : public ClientImpl {
        uri::URI baseAddress;

        int doHttpWinRequest(const char* method,
                             const char *path,
                             const Headers &headers,
                             BOOL (*retCodeHandler)(INT code, LPVOID ctx),
                             LPVOID retCodeHandlerCtx,
                             BOOL (*contentReceiver)(LPCH data, DWORD dataRead, LPVOID ctx),
                             LPVOID contentReceiverCtx,
                             const char *body, DWORD bodySize);

    public:
        explicit WINHttpLibClientImpl(const std::string &endpoint);

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
