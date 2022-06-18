#pragma once
#include "..\ddi\include\ddi.hpp"
#include "dllexport.h"
#include "CallbackDispatcher.hpp"

namespace HkbClient {

	extern "C" {
	    WDLL_EXPORT CallbackDispatcher* CreateDispatcher(callback_function callback);
	    WDLL_EXPORT void SetConfig(CallbackDispatcher* dispatcher, _KEYVALUEPAIR* keyvaluepairs, int size);
		WDLL_EXPORT void SetDownloadLocation(CallbackDispatcher* dispatcher, const char* location);
		WDLL_EXPORT void RunClient(const char* clientCertificatePath, const char* provisioningEndpoint, const char* xApigToken, CallbackDispatcher* dispatcher);
	    WDLL_EXPORT void ReleaseDispatcher(CallbackDispatcher* dispatcher);
	}
}
