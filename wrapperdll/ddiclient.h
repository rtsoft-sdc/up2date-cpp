#pragma once
#include "..\ddi\include\ddi.hpp"
#include "dllexport.h"
#include "CallbackDispatcher.hpp"

namespace HkbClient {

	extern "C" {
	    WDLL_EXPORT CallbackDispatcher* CreateDispatcher(
			ConfigRequestCallbackFunction configRequest, 
			DeploymentActionCallbackFunction deploymentAction, 
			CancelActionCallbackFunction cancelAction);
		WDLL_EXPORT void AddConfigAttribute(ddi::ConfigResponseBuilder* responseBuilder, const char* key, const char* value);
		WDLL_EXPORT void DownloadArtifact(ddi::Artifact* artifact, const char* location);
		WDLL_EXPORT void RunClient(const char* clientCertificatePath, const char* provisioningEndpoint, const char* xApigToken, CallbackDispatcher* dispatcher);
	    WDLL_EXPORT void ReleaseDispatcher(CallbackDispatcher* dispatcher);
	}
}
