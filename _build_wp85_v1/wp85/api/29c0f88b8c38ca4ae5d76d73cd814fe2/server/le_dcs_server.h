

/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef LE_DCS_INTERFACE_H_INCLUDE_GUARD
#define LE_DCS_INTERFACE_H_INCLUDE_GUARD


#include "legato.h"

// Internal includes for this interface
#include "le_dcs_common.h"
//--------------------------------------------------------------------------------------------------
/**
 * Get the server service reference
 */
//--------------------------------------------------------------------------------------------------
le_msg_ServiceRef_t le_dcs_GetServiceRef
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the client session reference for the current message
 */
//--------------------------------------------------------------------------------------------------
le_msg_SessionRef_t le_dcs_GetClientSessionRef
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * Initialize the server and advertise the service.
 */
//--------------------------------------------------------------------------------------------------
void le_dcs_AdvertiseService
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Reference returned by Request function and used by Release function
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 * Reference to a data channel
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 * Technologies.
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 * Channel states.
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 * Channel events.
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 * Handler for channel state changes
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 * Reference type used by Add/Remove functions for EVENT 'le_dcs_Event'
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 * This is the structure with info about each available channel
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 * Handler for the receiving the results of a channel list query
 */
//--------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------
/**
 * Add handler function for EVENT 'le_dcs_Event'
 *
 * This event provides information on channel events
 */
//--------------------------------------------------------------------------------------------------
le_dcs_EventHandlerRef_t le_dcs_AddEventHandler
(
    le_dcs_ChannelRef_t channelRef,
        ///< [IN] The channel for which the event is
    le_dcs_EventHandlerFunc_t handlerPtr,
        ///< [IN]
    void* contextPtr
        ///< [IN]
);



//--------------------------------------------------------------------------------------------------
/**
 * Remove handler function for EVENT 'le_dcs_Event'
 */
//--------------------------------------------------------------------------------------------------
void le_dcs_RemoveEventHandler
(
    le_dcs_EventHandlerRef_t handlerRef
        ///< [IN]
);



//--------------------------------------------------------------------------------------------------
/**
 * Query for a channel's technology type
 *
 * @return
 *      - The given channel's technology type as an enumberator from @ref le_dcs_Technology_t
 */
//--------------------------------------------------------------------------------------------------
le_dcs_Technology_t le_dcs_GetTechnology
(
    le_dcs_ChannelRef_t channelRef
        ///< [IN] channel which technology type is to be queried
);



//--------------------------------------------------------------------------------------------------
/**
 * Query for a channel's state
 *
 * @return
 *      - The given channel's state in the 2nd argument and associated network interface
 *        in 'name'
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_dcs_GetState
(
    le_dcs_ChannelRef_t channelRef,
        ///< [IN] channel which status is to be queried
    le_dcs_State_t* statePtr,
        ///< [OUT] channel state returned as output
    char* interfaceName,
        ///< [OUT] channel's network interface name
    size_t interfaceNameSize
        ///< [IN]
);



//--------------------------------------------------------------------------------------------------
/**
 * Request by an app to start a data channel
 *
 * @return
 *      - Object reference to the request (to be used later for releasing the channel)
 *      - NULL if it has failed to process the request
 */
//--------------------------------------------------------------------------------------------------
le_dcs_ReqObjRef_t le_dcs_Start
(
    le_dcs_ChannelRef_t channelRef
        ///< [IN] channel requested to be started
);



//--------------------------------------------------------------------------------------------------
/**
 * Stop for an app its previously started data channel
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_dcs_Stop
(
    le_dcs_ReqObjRef_t reqRef
        ///< [IN] the start request's reference earlier returned
);



//--------------------------------------------------------------------------------------------------
/**
 * Get the object reference of the channel given by the name and its technology type in the first
 * and second arguments as input
 *
 * @return
 *     - The retrieved channel reference is returned in the function's return value upon success.
 *     - Upon failure, 0 is returned back
 */
//--------------------------------------------------------------------------------------------------
le_dcs_ChannelRef_t le_dcs_GetReference
(
    const char* LE_NONNULL name,
        ///< [IN] name of channel which reference is to be retrieved
    le_dcs_Technology_t technology
        ///< [IN] technology of the channel given by its name above
);



//--------------------------------------------------------------------------------------------------
/**
 * Trigger a query for the list of all available data channels that will be returned asynchronously
 * via the ChannelQueryHandler callback notification
 */
//--------------------------------------------------------------------------------------------------
void le_dcs_GetChannels
(
    le_dcs_GetChannelsHandlerFunc_t handlerPtr,
        ///< [IN] requester's handler for receiving results
    void* contextPtr
        ///< [IN]
);


#endif // LE_DCS_INTERFACE_H_INCLUDE_GUARD