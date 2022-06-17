#pragma once
#include "..\ddi\include\ddi.hpp"
#include "dllexport.h"

namespace HkbClient {

    typedef char* (*callback_function)(void);

	extern "C" {
		WDLL_EXPORT int TestFunction1(int, int);
		WDLL_EXPORT int TestFunction2(const char* s);

		//WDLL_EXPORT void __stdcall RunClient(const char* clientCertificatePath, const char* provisioningEndpoint, const char* xApigToken);
		WDLL_EXPORT void RunClient(const char* clientCertificatePath, const char* provisioningEndpoint, const char* xApigToken, callback_function callback);
	}
}
