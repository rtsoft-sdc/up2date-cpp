#include "win_http.h"
#include <string.h>


#define DEFAULT_DOWNLOAD_BUF_SIZE 2048

DWORD do_http_request(const struct request_config* config) {

    return do_http_request_mtls(config, NULL);
}


DWORD do_http_request_mtls(const struct request_config * config, const struct mtls_keypair* kp){
    DWORD code = 0;
    HINTERNET hSession = NULL;
    HINTERNET hConnect = NULL;
    HINTERNET hRequest = NULL;
    LPCH host = NULL;

    hSession = InternetOpenA(
            "UP2Date Cli/1.0", // User-Agent
            INTERNET_OPEN_TYPE_PRECONFIG,
            NULL,
            NULL,
            0);

    if (!hSession) {
        code = GetLastError();
        goto clean;
    }

    INT port;
    BOOL isHttps = 0;

    DWORD hostSubLength;

    DWORD hostSize = strlen(config->host);
    LPCSTR portSub = strstr(config->host, ":");

    if (portSub == NULL) {
        if (strcmp("https", config->scheme) == 0) {
            port = INTERNET_DEFAULT_HTTPS_PORT;
            isHttps = TRUE;
        } else {
            port = INTERNET_DEFAULT_HTTP_PORT;
        }
        hostSubLength = hostSize;

    } else {
        port = atoi(portSub + 1);
        hostSubLength = hostSize - strlen(portSub);
    }

    host = (LPCH)malloc((hostSubLength + 1) * sizeof(CHAR));
    memcpy(host, config->host, hostSubLength);
    host[hostSubLength] = '\0';

    hConnect = InternetConnectA(
            hSession,
            host,
            port, // THIS
            "",
            "",
            INTERNET_SERVICE_HTTP,
            0,
            0);

    if (!hConnect) {
        code = GetLastError();
        goto clean;
    }

    DWORD flags = INTERNET_FLAG_DONT_CACHE | INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_RELOAD;
    if (isHttps){
        flags |= INTERNET_FLAG_SECURE;
        if (!config->verifyServerCrt) {
            flags |= INTERNET_FLAG_IGNORE_CERT_CN_INVALID | INTERNET_FLAG_IGNORE_CERT_CN_INVALID;
        }
    }

    hRequest = HttpOpenRequestA(
            hConnect,
            config->method,
            config->path,
            NULL,
            NULL,
            NULL,
            flags, // THIS
            0);

    if(!HttpSendRequestA(hRequest, config->headers, config->headersSize,
                    config->body, config->bodySize)) {
        code = GetLastError();
        goto clean;
    };

    CHAR szStatusCode[32] = { 0 };
    DWORD dwStatusCodeSize = sizeof(szStatusCode) / sizeof(CHAR);

    INT statusCode;
    if (HttpQueryInfoA(hRequest, HTTP_QUERY_STATUS_CODE,
                      szStatusCode, &dwStatusCodeSize, NULL)) {
        statusCode = atoi(szStatusCode);
    } else {
        code = ERROR_API_UNAVAILABLE;
        goto clean;
    }

    if (!config->retCodeHandler(statusCode, config->retCodeHandlerCtx)) {
        code = ERROR_CANCELLED;
        goto clean;
    }

    DWORD dwFileSize = DEFAULT_DOWNLOAD_BUF_SIZE;
    LPCH buffer = (LPCH)malloc(dwFileSize * sizeof(CHAR));

    while (1) {
        DWORD dwBytesRead;
        BOOL bRead;

        bRead = InternetReadFile(
                hRequest,
                buffer,
                dwFileSize,
                &dwBytesRead);

        if (dwBytesRead == 0 ) break;

        if (!bRead) {
            code = GetLastError();
            break;
        }

        if (!config->contentReceiver(
                buffer, dwBytesRead, config->contentReceiverCtx)) {
            code = ERROR_CANCELLED;
            break;
        }
    }


    free(buffer);
    clean:
    if (host) free(host);
    if (hRequest) InternetCloseHandle(hRequest);
    if (hConnect) InternetCloseHandle(hConnect);
    if (hSession) InternetCloseHandle(hSession);
    return code;
}
