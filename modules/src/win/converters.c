#include "converters.h"

LPWSTR convertToWCHAR(LPCSTR lpzStr, int *nChars) {
    LPWSTR lpwstr;
    int ignore = 0;
    int *ptr = (nChars == NULL) ? &ignore : nChars;

    *ptr = MultiByteToWideChar(CP_ACP, 0, lpzStr,
                               -1, NULL, 0);

    lpwstr = LocalAlloc(0, (*ptr) * sizeof(WCHAR));
    MultiByteToWideChar(CP_ACP, 0, lpzStr, -1,
                        lpwstr, *ptr);
    return lpwstr;
}


void extractDataFromHost(LPCSTR host, LPCSTR scheme, LPWSTR *lpwHost, INT *port, BOOL *isHttps) {

    DWORD dwHostSubLength;

    DWORD dwHostSize = (DWORD)strlen(host);
    LPCSTR szPortSub = strstr(host, ":");
    *isHttps = FALSE;

    if (szPortSub == NULL) {
        if (strcmp("https", scheme) == 0) {
            *port = INTERNET_DEFAULT_HTTPS_PORT;
            *isHttps = TRUE;
        } else {
            *port = INTERNET_DEFAULT_HTTP_PORT;
        }
        dwHostSubLength = dwHostSize;

    } else {
        *port = atoi(szPortSub + 1);
        dwHostSubLength = (DWORD)(dwHostSize - strlen(szPortSub));
    }

    LPSTR szHost = (LPCH)LocalAlloc(0, (dwHostSubLength + 1) * sizeof(CHAR));
    memcpy((void*)szHost, host, dwHostSubLength);
    szHost[dwHostSubLength] = '\0';

    // ignore value (null terminated string..)
    *lpwHost = convertToWCHAR(szHost, NULL);

    LocalFree(szHost);
}


