#include <exception>
#include <thread>
#include "pch.h"
#include "ddiclient.h"
#include "..\ddi\include\ddi.hpp"
#include "..\dps\include\ritms_dps.hpp"
#include "CallbackDispatcher.hpp"
#include "DPSInfoReloadHandler.hpp"

using namespace ritms::dps;
using namespace ddi;

namespace HkbClient {

	int TestFunction1(int a1, int a2)	{
		return a1 + a2;
	}

	int TestFunction2(const char* s)	{
		return std::string(s).length();
	}

    // void callback(void) {
    // }

    //void __stdcall RunClient(const char* clientCertificatePath, const char* provisioningEndpoint, const char* xApigToken) {
    void RunClient(const char* clientCertificatePath, const char* provisioningEndpoint, const char* xApigToken, callback_function callback) {
        std::ifstream t((std::string(clientCertificatePath)));
        if (!t.is_open()) {
            std::cout << "File " << clientCertificatePath << " not exists" << std::endl;
            throw std::runtime_error(std::string("fail: cannot open file :").append(clientCertificatePath));
        }
        std::string crt((std::istreambuf_iterator<char>(t)),
                        std::istreambuf_iterator<char>());

        auto dpsBuilder = CloudProvisioningClientBuilder::newInstance();
        auto dpsClient = dpsBuilder->setEndpoint(provisioningEndpoint)
                ->setAuthCrt(crt)
                ->addHeader("X-Apig-AppCode", std::string(xApigToken))
                ->build();

        auto authErrorHandler = std::shared_ptr<AuthErrorHandler>(new DPSInfoReloadHandler(std::move(dpsClient)));


        auto builder = DDIClientBuilder::newInstance();
        builder->setAuthErrorHandler(authErrorHandler)
            ->setEventHandler(std::shared_ptr<EventHandler>(new CallbackDispatcher(callback)))
            ->build()
            ->run();

    }

}
