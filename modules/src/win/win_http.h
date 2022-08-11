#ifndef __WIN_HTTP_CLI_IMPL
#define __WIN_HTTP_CLI_IMPL

#include <windows.h>
#include <wininet.h>


#pragma comment (lib, "Wininet.lib")

#ifdef __cplusplus
extern "C" {
#endif

struct request_config {
    LPCSTR scheme;
    LPCSTR host;
    LPCSTR path;
    LPCSTR method;

    LPCSTR headers;
    DWORD headersSize;

    LPVOID body;
    DWORD bodySize;

    BOOL (*retCodeHandler)(INT code, LPVOID ctx);

    LPVOID retCodeHandlerCtx;

    BOOL (*contentReceiver)(LPCH data, DWORD dataRead, LPVOID ctx);

    LPVOID contentReceiverCtx;
};


DWORD do_http_request(const struct request_config *);

#ifdef __cplusplus
}
#endif
#endif