#include "win_http.h"
#include "converters.h"
#include <string.h>
#include <stdio.h>


static DWORD do_http_request_ctx(const struct request_config *config, PCCERT_CONTEXT ctx){
    DWORD code = 0;

    HINTERNET hSession = NULL;
    HINTERNET hConnect = NULL;
    HINTERNET hRequest = NULL;

    LPWSTR lpwHostAddress = NULL;
    BOOL bIsHttps;
    INT iPort;

    LPWSTR lpwMethod = NULL;
    LPWSTR lpwPath = NULL;

    LPWSTR lpwHeaders = NULL;

    hSession = WinHttpOpen(
            L"UP2Date Cli/1.0", // User-Agent
            WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
            WINHTTP_NO_PROXY_NAME,
            WINHTTP_NO_PROXY_BYPASS,
            0);

    if (!hSession) {
        code = GetLastError();
        goto clean;
    }

    extractDataFromHost(config->szHost, config->szScheme,
                        &lpwHostAddress,
                        &iPort,
                        &bIsHttps);


    hConnect = WinHttpConnect(hSession,
                              lpwHostAddress,
                              iPort,
                              0);

    if (!hConnect) {
        code = GetLastError();
        goto clean;
    }

    lpwMethod = convertToWCHAR(config->szMethod, NULL);
    lpwPath = convertToWCHAR(config->szPath, NULL);

    DWORD flags = WINHTTP_FLAG_REFRESH;
    if (bIsHttps){
        flags |= WINHTTP_FLAG_SECURE;
    }

    const wchar_t *att[] = { L"*/*", NULL };
    hRequest = WinHttpOpenRequest(
            hConnect,
            lpwMethod,
            lpwPath,
            NULL,
            WINHTTP_NO_REFERER,
            att,
            flags);

    if (!hRequest) {
        code = GetLastError();
        goto clean;
    }

    if (bIsHttps) {
        if (!WinHttpSetOption(hRequest, WINHTTP_OPTION_CLIENT_CERT_CONTEXT,
                              (ctx) ? (LPVOID) ctx : WINHTTP_NO_CLIENT_CERT_CONTEXT,
                              (ctx) ? sizeof(CERT_CONTEXT) : 0)) {
            code = GetLastError();
            goto clean;
        }
    }

    if (!config->bVerifyServerCrt) {
        DWORD dwFlags = SECURITY_FLAG_IGNORE_UNKNOWN_CA |
                SECURITY_FLAG_IGNORE_CERT_WRONG_USAGE |
                SECURITY_FLAG_IGNORE_CERT_CN_INVALID |
                SECURITY_FLAG_IGNORE_CERT_DATE_INVALID;

        if (!WinHttpSetOption(hRequest, WINHTTP_OPTION_SECURITY_FLAGS,
                              &dwFlags, sizeof(dwFlags))) {
            code = GetLastError();
            goto clean;
        }
    }

    int headersLength = 0;
    lpwHeaders = convertToWCHAR(config->szHeaders, &headersLength);

    if(!WinHttpSendRequest(
            hRequest,
            lpwHeaders,
            -1L,
            config->pvBody,
            config->dwBodySize,
            config->dwBodySize,
            0)) {

        code = GetLastError();
        goto clean;
    }

    if (!WinHttpReceiveResponse( hRequest, NULL)) {
        code = GetLastError();
        goto clean;
    }

    DWORD dwStatusCode = 0;
    DWORD dwSize = sizeof(dwStatusCode);

    if (!WinHttpQueryHeaders(hRequest,
                            WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                            WINHTTP_HEADER_NAME_BY_INDEX,
                            &dwStatusCode, &dwSize, WINHTTP_NO_HEADER_INDEX)) {

        code = ERROR_API_UNAVAILABLE;
        goto clean;
    }

    if (!config->pfRetCodeHandler((INT)dwStatusCode, config->pvRetCodeHandlerCtx)) {
        code = ERROR_CANCELLED;
        goto clean;
    }

    while(1) {
        // Check for available data.
        DWORD dwReqSize = 0;
        DWORD dwActSize = 0;
        BOOL bError = FALSE;

        if (!WinHttpQueryDataAvailable( hRequest, &dwReqSize)) {
            code = GetLastError();
            break;
        }

        if (dwReqSize == 0) break;
        LPCH buffer = (LPCH)LocalAlloc(0, (dwReqSize+1) * sizeof(CHAR));

        ZeroMemory(buffer, dwReqSize+1);

        if (WinHttpReadData( hRequest, (LPVOID)buffer,
                              dwSize, &dwActSize)){
            if (!config->pfContentReceiver(
                    buffer, dwActSize, config->pvContentReceiverCtx)) {
                code = ERROR_CANCELLED;
                bError = TRUE;
            }
        } else {
            code = GetLastError();
            bError = TRUE;
        }

        LocalFree(buffer);
        if (bError)
            break;
    }

    clean:
    if (lpwHostAddress) LocalFree(lpwHostAddress);
    if (lpwMethod) LocalFree(lpwMethod);
    if (lpwPath) LocalFree(lpwPath);
    if (lpwHeaders) LocalFree(lpwHeaders);

    if (hRequest) WinHttpCloseHandle(hRequest);
    if (hConnect) WinHttpCloseHandle(hConnect);
    if (hSession) WinHttpCloseHandle(hSession);
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
    PCCERT_CONTEXT ctx_ = NULL;
    PCCERT_CONTEXT ctx__ = NULL;
    PCCERT_CONTEXT  pCertContext = NULL;
    HCRYPTPROV hProv = 0;
    HCRYPTKEY hKey = 0;
    DWORD pdwSkip = 0;

    // open local user store
    hMemStore = CertOpenStore(
            CERT_STORE_PROV_SYSTEM,   // the memory provider type
            0,
            0,                     // use the default HCRYPTPROV
            CERT_SYSTEM_STORE_CURRENT_USER, // accept the default dwFlags
            L"RTS_Store"                      // pvPara is not used
    );
    HANDLE_ERROR(hMemStore);

    // clear certificate store (remove old unused certs)
    PCCERT_CONTEXT  pDupCertContext = NULL;
    while((pCertContext = CertEnumCertificatesInStore(
            hMemStore,
            pCertContext)))
    {
        pDupCertContext = CertDuplicateCertificateContext(
                pCertContext);

        HANDLE_ERROR(CertDeleteCertificateFromStore(pDupCertContext))
        pCertContext = CertEnumCertificatesInStore(
                hMemStore,
                pCertContext);
    }

    // read certificate
    HANDLE_ERROR(CryptStringToBinaryA(kp->cszCrt, kp->dwCrtSize, CRYPT_STRING_BASE64HEADER,
                                      NULL, &dwBufferLen, &pdwSkip, NULL));

    pbDecryptedData = LocalAlloc(0, dwBufferLen);

    HANDLE_ERROR(CryptStringToBinaryA(kp->cszCrt, kp->dwCrtSize, CRYPT_STRING_BASE64HEADER,
                                      pbDecryptedData, &dwBufferLen, NULL, NULL));

    ctx_ = CertCreateCertificateContext(X509_ASN_ENCODING, 
                                        pbDecryptedData,dwBufferLen);
    HANDLE_ERROR(ctx_);

    LPCSTR lpcKeyName = "RITMSOnlinePKEY";
    LPWSTR lpwKeyName = L"RITMSOnlinePKEY";

    if(CryptAcquireContextA(
            &hProv,
            lpcKeyName,
            NULL,
            PROV_RSA_FULL,
            0) == 0) {

        DWORD dwLastError = GetLastError();
        if (dwLastError != NTE_BAD_KEYSET) {
            dwRetCode = dwLastError;
            goto clear;
        }

        HANDLE_ERROR(CryptAcquireContextA(
                &hProv,
                lpcKeyName,
                NULL,
                PROV_RSA_FULL,
                CRYPT_NEWKEYSET))
    } else {
        // recreate container to remove old keys...
        CryptReleaseContext(hProv, 0);

        HANDLE_ERROR(CryptAcquireContextA(
                &hProv,
                lpcKeyName,
                NULL,
                PROV_RSA_FULL,
                CRYPT_DELETEKEYSET))

        HANDLE_ERROR(CryptAcquireContextA(
                &hProv,
                lpcKeyName,
                NULL,
                PROV_RSA_FULL,
                CRYPT_NEWKEYSET))
    }

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

    HANDLE_ERROR(CryptImportKey(hProv, pbKeyBlob, cbKeyBlob, 0,
                                CRYPT_EXPORTABLE, &hKey))


    CRYPT_KEY_PROV_INFO kpInfo;
    ZeroMemory( & kpInfo, sizeof(kpInfo) );
    kpInfo.dwProvType = PROV_RSA_FULL;
    kpInfo.pwszContainerName = lpwKeyName;
    kpInfo.dwKeySpec = AT_KEYEXCHANGE;
    kpInfo.dwFlags = CERT_SET_KEY_PROV_HANDLE_PROP_ID;

    HANDLE_ERROR(CertSetCertificateContextProperty(ctx_,
                                                   CERT_KEY_PROV_INFO_PROP_ID, 0, &kpInfo));


    // load additional chain certificates
    LPCSTR pos = kp->cszCrt;
    while ((pos=strstr(pos + pdwSkip + 1, "-----BEGIN CERTIFICATE-----"))) {
        LocalFree(pbDecryptedData);

        HANDLE_ERROR(CryptStringToBinaryA(pos, strlen(pos), CRYPT_STRING_BASE64HEADER,
                                          NULL, &dwBufferLen, &pdwSkip, NULL));

        pbDecryptedData = LocalAlloc(0, dwBufferLen);

        HANDLE_ERROR(CryptStringToBinaryA(pos, strlen(pos), CRYPT_STRING_BASE64HEADER,
                                          pbDecryptedData, &dwBufferLen, NULL, NULL));

        ctx__ = CertCreateCertificateContext(X509_ASN_ENCODING,
                                            pbDecryptedData,dwBufferLen);
        HANDLE_ERROR(ctx__);
        HANDLE_ERROR(CertAddCertificateContextToStore(hMemStore,ctx__,
                                                      CERT_STORE_ADD_REPLACE_EXISTING, NULL));
        CertFreeCertificateContext(ctx__);
        ctx__ = NULL;
    }


    HANDLE_ERROR(CertAddCertificateContextToStore(hMemStore,ctx_,
                                                  CERT_STORE_ADD_REPLACE_EXISTING, NULL));

    dwRetCode = do_http_request_ctx(config, ctx_);

    clear:
    if (pbDecryptedData) LocalFree(pbDecryptedData);
    if (pbKeyBlob) LocalFree (pbKeyBlob);
    if (pCertContext) CertFreeCertificateContext(pCertContext);
    if (ctx_) CertFreeCertificateContext(ctx_);
    if (ctx__) CertFreeCertificateContext(ctx__);
    if (hKey) CryptDestroyKey(hKey);
    if (hProv) CryptReleaseContext(hProv, 0);

    if (hMemStore) CertCloseStore(hMemStore, CERT_CLOSE_STORE_FORCE_FLAG);

    return dwRetCode;
}

DWORD do_http_request_mtls(const struct request_config * config, const struct mtls_keypair* kp) {
    if (kp) {
        return do_http_request_mtls_(config, kp);
    }

    return do_http_request(config);
}