#pragma once
#include "..\ddi\include\ddi.hpp"
#include "..\dps\include\ritms_dps.hpp"
#include "dllexport.h"
#include "CallbackDispatcher.hpp"

namespace HkbClient {

    typedef bool (*AuthErrorCallbackFunction)(const char* errorMessage);

	extern "C" {
	    WDLL_EXPORT CallbackDispatcher* CreateDispatcher(
			ConfigRequestCallbackFunction configRequest, 
			DeploymentActionCallbackFunction deploymentAction, 
			CancelActionCallbackFunction cancelAction);
		WDLL_EXPORT void AddConfigAttribute(ddi::ConfigResponseBuilder* responseBuilder, const char* key, const char* value);
		WDLL_EXPORT void DownloadArtifact(ddi::Artifact* artifact, const char* location);
		WDLL_EXPORT void RunClient(const char* clientCertificate, const char* provisioningEndpoint, const char* xApigToken, CallbackDispatcher* dispatcher, AuthErrorCallbackFunction authErrorAction);
	    WDLL_EXPORT void DeleteDispatcher(CallbackDispatcher* dispatcher);
	}
}
