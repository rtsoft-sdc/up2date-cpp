#ifndef __WIN_HTTP_CLI_IMPL
#define __WIN_HTTP_CLI_IMPL

#include <Windows.h>
#include <winhttp.h>


#ifdef __cplusplus
extern "C" {
#endif

struct request_config {
    LPCSTR szScheme;
    LPCSTR szHost;
    LPCSTR szPath;
    LPCSTR szMethod;

    LPCSTR szHeaders;
    DWORD dwHeadersSize;

    LPVOID pvBody;
    DWORD dwBodySize;

    BOOL bVerifyServerCrt;

    BOOL (*pfRetCodeHandler)(INT code, LPVOID ctx);
    LPVOID pvRetCodeHandlerCtx;

    BOOL (*pfContentReceiver)(LPCH data, DWORD dataRead, LPVOID ctx);
    LPVOID pvContentReceiverCtx;
};

struct mtls_keypair {
    LPCSTR cszCrt;
    DWORD dwCrtSize;

    LPCSTR cszKey;
    DWORD dwKeySize;
};


DWORD do_http_request(const struct request_config *);
DWORD do_http_request_mtls(const struct request_config *, const struct mtls_keypair*);

#ifdef __cplusplus
}
#endif
#endif