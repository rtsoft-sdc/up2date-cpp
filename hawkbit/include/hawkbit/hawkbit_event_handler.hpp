#pragma once

#include <memory>

#include "hawkbit_response.hpp"
#include "hawkbit_actions.hpp"

namespace hawkbit {

    // user defined event handler for hawkBit client
    class EventHandler {
    public:

        // there`is no meta from hawkBit for configRequest
        virtual std::unique_ptr<ConfigResponse> onConfigRequest() = 0;

        virtual std::unique_ptr<Response> onDeploymentAction(std::unique_ptr<DeploymentBase>) = 0;

        virtual std::unique_ptr<Response> onCancelAction(std::unique_ptr<CancelAction>) = 0;

        virtual void onNoActions() = 0;

        virtual ~EventHandler() = default;
    };

}