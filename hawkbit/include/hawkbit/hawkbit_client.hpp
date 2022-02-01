#pragma once

#include <string>
#include <memory>

#include "hawkbit_event_handler.hpp"

namespace hawkbit {
    // Main class that used for communication with hawkBit
    class Client {
    public:

        // infinity loop
        virtual void run() = 0;

        virtual ~Client() = default;
    };

    class ClientBuilder {
    public:

        static std::unique_ptr<ClientBuilder> newInstance();

        virtual ClientBuilder *setCrt(const std::string&) = 0;

        virtual ClientBuilder *setProvisioningEndpoint(const std::string&) = 0;

        virtual ClientBuilder *setDefaultPollingTimeout(int pollingTimeout_) = 0;

        virtual ClientBuilder *setEventHandler(std::shared_ptr<EventHandler> handler) = 0;

        virtual ClientBuilder *addHeader(const std::string &, const std::string &) = 0;

        virtual ClientBuilder *addProvisioningHeader(const std::string &, const std::string &) = 0;

        virtual std::unique_ptr<Client> build() = 0;

        virtual ~ClientBuilder() = default;
    };


}
