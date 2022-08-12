#ifndef __WIN_HTTP_CLI_IMPL
#define __WIN_HTTP_CLI_IMPL

#include <Windows.h>
#include <WinInet.h>


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

    BOOL verifyServerCrt;

    BOOL (*retCodeHandler)(INT code, LPVOID ctx);
    LPVOID retCodeHandlerCtx;

    BOOL (*contentReceiver)(LPCH data, DWORD dataRead, LPVOID ctx);
    LPVOID contentReceiverCtx;
};

struct mtls_keypair {
    LPSTR crt;
    DWORD crtSize;

    LPSTR key;
    DWORD keySize;
};


DWORD do_http_request(const struct request_config *);
DWORD do_http_request_mtls(const struct request_config *, const struct mtls_keypair*);

#ifdef __cplusplus
}
#endif
#endif