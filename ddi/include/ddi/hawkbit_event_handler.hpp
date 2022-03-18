#pragma once

#include <memory>

#include "hawkbit_response.hpp"
#include "hawkbit_actions.hpp"

namespace ddi {

    // User defined event handler for hawkBit actions
    class EventHandler {
    public:

        // Called when hawkBit request current config from client.
        virtual std::unique_ptr<ConfigResponse> onConfigRequest() = 0;

        // Called when hawkBit request to start (queue) update.
        virtual std::unique_ptr<Response> onDeploymentAction(std::unique_ptr<DeploymentBase>) = 0;

        // Called when hawkBit request to stop current update.
        virtual std::unique_ptr<Response> onCancelAction(std::unique_ptr<CancelAction>) = 0;

        // Called if no actions requested (bcs client requested hwb every n seconds)
        virtual void onNoActions() = 0;

        virtual ~EventHandler() = default;
    };

}