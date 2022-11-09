#ifndef __WIN_CONVERTERS_H
#define __WIN_CONVERTERS_H

#include <Windows.h>
#include <winhttp.h>

#ifdef __cplusplus
extern "C" {
#endif


LPWSTR convertToWCHAR(LPCSTR lpzStr, int *nChars);
void extractDataFromHost(LPCSTR host, LPCSTR scheme, LPWSTR *lpwHost, INT *port, BOOL *isHttps);

#ifdef __cplusplus
}
#endif
#endif