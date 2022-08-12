#include "win_http.h"
#include <string.h>
#include <stdio.h>


#define DEFAULT_DOWNLOAD_BUF_SIZE 2048


static DWORD do_http_request_ctx(const struct request_config * config, PCCERT_CONTEXT ctx){
    DWORD code = 0;
    HINTERNET hSession = NULL;
    HINTERNET hConnect = NULL;
    HINTERNET hRequest = NULL;
    LPCH szHost = NULL;

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


    INT iPort;
    BOOL bIsHttps = 0;

    DWORD dwHostSubLength;

    DWORD dwHostSize = strlen(config->szHost);
    LPCSTR szPortSub = strstr(config->szHost, ":");

    if (szPortSub == NULL) {
        if (strcmp("https", config->szScheme) == 0) {
            iPort = INTERNET_DEFAULT_HTTPS_PORT;
            bIsHttps = TRUE;
        } else {
            iPort = INTERNET_DEFAULT_HTTP_PORT;
        }
        dwHostSubLength = dwHostSize;

    } else {
        iPort = atoi(szPortSub + 1);
        dwHostSubLength = dwHostSize - strlen(szPortSub);
    }

    szHost = (LPCH)LocalAlloc(0, (dwHostSubLength + 1) * sizeof(CHAR));
    memcpy(szHost, config->szHost, dwHostSubLength);
    szHost[dwHostSubLength] = '\0';

    hConnect = InternetConnectA(
            hSession,
            szHost,
            iPort, // THIS
            NULL,
            NULL,
            INTERNET_SERVICE_HTTP,
            0,
            0);

    if (!hConnect) {
        code = GetLastError();
        goto clean;
    }

    DWORD flags = INTERNET_FLAG_RELOAD;
    if (bIsHttps){
        flags |= INTERNET_FLAG_SECURE ;
        if (!config->bVerifyServerCrt) {
            flags |= INTERNET_FLAG_IGNORE_CERT_CN_INVALID | INTERNET_FLAG_IGNORE_CERT_CN_INVALID;
        }
    }

    hRequest = HttpOpenRequestA(
            hConnect,
            config->szMethod,
            config->szPath,
            NULL,
            "",
            NULL,
            flags, // THIS
            0);

    if (ctx) {
        if (!InternetSetOptionA(hRequest, INTERNET_OPTION_CLIENT_CERT_CONTEXT,
                                (LPVOID)ctx, sizeof(CERT_CONTEXT))) {
            code = GetLastError();
            goto clean;
        }
    }

    if(!HttpSendRequestA(hRequest, config->szHeaders, config->dwHeadersSize,
                    config->pvBody, config->dwBodySize)) {

        code = GetLastError();
        goto clean;
    }

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

    if (!config->pfRetCodeHandler(statusCode, config->pvRetCodeHandlerCtx)) {
        code = ERROR_CANCELLED;
        goto clean;
    }

    DWORD dwFileSize = DEFAULT_DOWNLOAD_BUF_SIZE;
    LPCH buffer = (LPCH)LocalAlloc(0, dwFileSize * sizeof(CHAR));

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

        if (!config->pfContentReceiver(
                buffer, dwBytesRead, config->pvContentReceiverCtx)) {
            code = ERROR_CANCELLED;
            break;
        }
    }


    LocalFree(buffer);
    clean:
    if (szHost) LocalFree(szHost);
    if (hRequest) InternetCloseHandle(hRequest);
    if (hConnect) InternetCloseHandle(hConnect);
    if (hSession) InternetCloseHandle(hSession);
    return code;
}


DWORD do_http_request(const struct request_config* config) {

    return do_http_request_ctx(config, NULL);
}

#define HANDLE_ERROR(f) {if (!(f)){ \
                dwRetCode = GetLastError(); \
                goto clear;    \
}}

static DWORD do_http_request_mtls_(const struct request_config * config, const struct mtls_keypair* kp) {
    DWORD dwRetCode = 0;
    DWORD dwBufferLen = 0;
    LPBYTE pbDecryptedData = NULL;
    DWORD cbKeyBlob = 0;
    LPBYTE pbKeyBlob = NULL;
    HCERTSTORE hMemStore = NULL;
    PCCERT_CONTEXT ctx = NULL, ctx_ = NULL;
    HCRYPTPROV hProv = NULL;
    HCRYPTKEY hKey = NULL;

    HANDLE_ERROR(CryptStringToBinaryA(kp->cszCrt, kp->dwCrtSize, CRYPT_STRING_BASE64HEADER,
                                      NULL, &dwBufferLen, NULL, NULL));


    pbDecryptedData = LocalAlloc(0, dwBufferLen);

    HANDLE_ERROR(CryptStringToBinaryA(kp->cszCrt, kp->dwCrtSize, CRYPT_STRING_BASE64HEADER,
                              pbDecryptedData, &dwBufferLen, NULL, NULL));

    hMemStore = CertOpenStore(
            CERT_STORE_PROV_MEMORY,   // the memory provider type
            0,
            NULL,                     // use the default HCRYPTPROV
            0,                        // accept the default dwFlags
            NULL                      // pvPara is not used
    );

    HANDLE_ERROR(hMemStore);

    ctx_ = CertCreateCertificateContext(X509_ASN_ENCODING, 
                                        pbDecryptedData,dwBufferLen);
    HANDLE_ERROR(ctx_);


    HANDLE_ERROR(CryptAcquireContextA(&hProv,
                              NULL, NULL,
                              PROV_RSA_SCHANNEL,
                              CRYPT_VERIFYCONTEXT ));

    HANDLE_ERROR(CryptStringToBinaryA(kp->cszKey, kp->dwKeySize, CRYPT_STRING_BASE64HEADER,
                              NULL, &dwBufferLen, NULL, NULL));

    LocalFree(pbDecryptedData);
    pbDecryptedData = LocalAlloc(0, dwBufferLen);

    HANDLE_ERROR(CryptStringToBinaryA(kp->cszKey, kp->dwKeySize, CRYPT_STRING_BASE64HEADER,
                              pbDecryptedData, &dwBufferLen, NULL, NULL));

    HANDLE_ERROR(CryptDecodeObjectEx(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
                             PKCS_RSA_PRIVATE_KEY, pbDecryptedData, dwBufferLen, 0,
                             NULL, NULL, &cbKeyBlob));

    pbKeyBlob = (LPBYTE) LocalAlloc(0, cbKeyBlob);

    HANDLE_ERROR(CryptDecodeObjectEx(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, PKCS_RSA_PRIVATE_KEY,
                             pbDecryptedData, dwBufferLen, 0,
                             NULL, pbKeyBlob, &cbKeyBlob));

    HANDLE_ERROR(CryptImportKey(hProv, pbKeyBlob, cbKeyBlob, NULL,
                                CRYPT_EXPORTABLE, &hKey))


    HANDLE_ERROR(CertSetCertificateContextProperty(ctx_,
                                                   CERT_KEY_PROV_HANDLE_PROP_ID, 0, hProv));


    CRYPT_KEY_PROV_INFO info;
    info.dwProvType = PROV_RSA_SCHANNEL;
    info.pwszContainerName = NULL;
    info.pwszProvName = NULL;
    info.dwKeySpec = AT_KEYEXCHANGE | AT_SIGNATURE;
    info.cProvParam = 0;
    info.rgProvParam = 0;

    HANDLE_ERROR(CertSetCertificateContextProperty(ctx_,
                                                   CERT_KEY_PROV_INFO_PROP_ID, 0, &info));


    HANDLE_ERROR(CertAddCertificateContextToStore(hMemStore,ctx_,
                                          CERT_STORE_ADD_NEW, &ctx));



    dwRetCode = do_http_request_ctx(config, ctx);

    clear:
    if (pbDecryptedData) LocalFree(pbDecryptedData);
    if (pbKeyBlob) LocalFree (pbKeyBlob);

    // TODO: check if needed (returned const pointer)
    if (ctx) CertFreeCertificateContext(ctx);
    if (ctx_) CertFreeCertificateContext(ctx_);

    if (hMemStore) CertCloseStore(hMemStore, CERT_CLOSE_STORE_FORCE_FLAG);

    if (hKey) CryptDestroyKey(hKey);
    if (hProv) CryptReleaseContext(hProv, 0);

    return dwRetCode;
}

DWORD do_http_request_mtls(const struct request_config * config, const struct mtls_keypair* kp) {
    if (kp) {
        return do_http_request_mtls_(config, kp);
    }

    return do_http_request(config);
}