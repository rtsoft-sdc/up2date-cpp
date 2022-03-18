#pragma once

#include "ddi/hawkbit_response.hpp"
#include "ddi/ddi_client.hpp"

/*
 * Description:
 *  DDI module contains all required functionality for easy business logic development
 *
 *  The main case of this project is to simplify hawkBit communication model.
 *  This module provides event handle callback model (spectator pattern). All you need is to
 *   implement EventHandler interface and register one in client.
 *
 *  Client will call one of presented methods based on action that was received from hawkBit.
 *
 * Communication model scheme:
 *  For library user the communication model looks like:
 *
 *  hawkBit -- (action) --> ddi
 *                           +    -->   EventHandler.(action)
 *                                           [your logic]
 *                                           return result
 *  hawkBit <--(response) -- +    <--              +
 *
 *  Also AuthErrorHandler, ResponseDeliveryListener can be implemented.
 *  This interfaces provides API for client behaviour customisation. You can get more information about interfaces in
 *   module documentation.
 *
 */