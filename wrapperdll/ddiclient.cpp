#include "pch.h"
#include "ddiclient.h"
#include "../ddi/include/ddi.hpp"

namespace HkbClient {

	int __stdcall TestFunction1(int a1, int a2)
	{
		return a1 + a2;
	}

    void _stdcall BuildAndRun(void)
    {
        auto builder = ddi::DDIClientBuilder::newInstance();
        builder->setHawkbitEndpoint("hawkbitEndpoint", "controllerId");
        builder->setGatewayToken("gatewayToken");
        //builder->setEventHandler(std::shared_ptr<EventHandler>(new Handler()));
        builder->notVerifyServerCertificate();
        auto client = builder->build();
        client->run();
    }
}
