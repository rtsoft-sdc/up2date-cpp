#pragma once
#include "..\ddi\include\ddi.hpp"
#include "dllexport.h"

namespace HkbClient {
	extern "C" {
		WDLL_EXPORT int __stdcall TestFunction1(int, int);
		WDLL_EXPORT int __stdcall TestFunction2(const char* s);

		WDLL_EXPORT void __stdcall RunClient(const char* clientCertificatePath, const char* provisioningEndpoint, const char* xApigToken);
	}
}
