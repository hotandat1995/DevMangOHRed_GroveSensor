
/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */
#ifndef LE_DCS_COMMON_H_INCLUDE_GUARD
#define LE_DCS_COMMON_H_INCLUDE_GUARD


#include "legato.h"

#define IFGEN_LE_DCS_PROTOCOL_ID "b99da6a1f07d6a6525ee41fd78fb48a5"
#define IFGEN_LE_DCS_MSG_SIZE 3184



//--------------------------------------------------------------------------------------------------
/**
 * Interface name string length.
 */
//--------------------------------------------------------------------------------------------------
#define LE_DCS_INTERFACE_NAME_MAX_LEN 100

//--------------------------------------------------------------------------------------------------
/**
 * Channel name string length.
 */
//--------------------------------------------------------------------------------------------------
#define LE_DCS_CHANNEL_NAME_MAX_LEN 32

//--------------------------------------------------------------------------------------------------
/**
 * IP addr string's max length
 */
//--------------------------------------------------------------------------------------------------
#define LE_DCS_IPADDR_MAX_LEN 46

//--------------------------------------------------------------------------------------------------
/**
 * Max # of channel list entries for a query: CHANNEL_LIST_QUERY_MAX is for per-technology, while
 * CHANNEL_LIST_ENTRY_MAX is for all channels of all supported technologies. Currently, the former
 * one's value is set to the max value supported by cellular. The latter is set to 2 times of that.
 * When more technologies are supported, this latter one's value needs to be raised accordingly.
 */
//--------------------------------------------------------------------------------------------------
#define LE_DCS_CHANNEL_LIST_QUERY_MAX 36

//--------------------------------------------------------------------------------------------------
/**
 */
//--------------------------------------------------------------------------------------------------
#define LE_DCS_CHANNEL_LIST_ENTRY_MAX 72

//--------------------------------------------------------------------------------------------------
/**
 * Reference returned by Request function and used by Release function
 */
//--------------------------------------------------------------------------------------------------
typedef struct le_dcs_ReqObj* le_dcs_ReqObjRef_t;


//--------------------------------------------------------------------------------------------------
/**
 * Reference to a data channel
 */
//--------------------------------------------------------------------------------------------------
typedef struct le_dcs_Channel* le_dcs_ChannelRef_t;


//--------------------------------------------------------------------------------------------------
/**
 * Technologies.
 */
//--------------------------------------------------------------------------------------------------
typedef enum
{
    LE_DCS_TECH_UNKNOWN = 0,
        ///<
    LE_DCS_TECH_WIFI = 1,
        ///< Wi-Fi
    LE_DCS_TECH_CELLULAR = 2,
        ///< Cellular
    LE_DCS_TECH_MAX = 3
        ///< Unknown state.
}
le_dcs_Technology_t;


//--------------------------------------------------------------------------------------------------
/**
 * Channel states.
 */
//--------------------------------------------------------------------------------------------------
typedef enum
{
    LE_DCS_STATE_DOWN = 0,
        ///<
    LE_DCS_STATE_UP = 1
        ///<
}
le_dcs_State_t;


//--------------------------------------------------------------------------------------------------
/**
 * Channel events.
 */
//--------------------------------------------------------------------------------------------------
typedef enum
{
    LE_DCS_EVENT_DOWN = 0,
        ///<
    LE_DCS_EVENT_UP = 1
        ///<
}
le_dcs_Event_t;


//--------------------------------------------------------------------------------------------------
/**
 * Reference type used by Add/Remove functions for EVENT 'le_dcs_Event'
 */
//--------------------------------------------------------------------------------------------------
typedef struct le_dcs_EventHandler* le_dcs_EventHandlerRef_t;


//--------------------------------------------------------------------------------------------------
/**
 * This is the structure with info about each available channel
 */
//--------------------------------------------------------------------------------------------------
typedef struct
{
    le_dcs_ChannelRef_t ref;
    char name[32 + 1];
    le_dcs_Technology_t technology;
    le_dcs_State_t state;
}
le_dcs_ChannelInfo_t;


//--------------------------------------------------------------------------------------------------
/**
 * Handler for channel state changes
 */
//--------------------------------------------------------------------------------------------------
typedef void (*le_dcs_EventHandlerFunc_t)
(
        le_dcs_ChannelRef_t channelRef,
        ///< The channel for which the event is
        le_dcs_Event_t event,
        ///< Event up or down
        int32_t code,
        ///< Additional event code, like error code
        void* contextPtr
        ///<
);

//--------------------------------------------------------------------------------------------------
/**
 * Handler for the receiving the results of a channel list query
 */
//--------------------------------------------------------------------------------------------------
typedef void (*le_dcs_GetChannelsHandlerFunc_t)
(
        le_result_t result,
        ///< Result of the channel list query
        const le_dcs_ChannelInfo_t* channelListPtr,
        ///< List of channels returned
        size_t channelListSize,
        ///<
        void* contextPtr
        ///<
);


//--------------------------------------------------------------------------------------------------
/**
 * Get if this client bound locally.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED bool ifgen_le_dcs_HasLocalBinding
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Init data that is common across all threads
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED void ifgen_le_dcs_InitCommonData
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Perform common initialization and open a session
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_dcs_OpenSession
(
    le_msg_SessionRef_t _ifgen_sessionRef,
    bool isBlocking
);

//--------------------------------------------------------------------------------------------------
/**
 * Add handler function for EVENT 'le_dcs_Event'
 *
 * This event provides information on channel events
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_dcs_EventHandlerRef_t ifgen_le_dcs_AddEventHandler
(
    le_msg_SessionRef_t _ifgen_sessionRef,
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
LE_SHARED void ifgen_le_dcs_RemoveEventHandler
(
    le_msg_SessionRef_t _ifgen_sessionRef,
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
LE_SHARED le_dcs_Technology_t ifgen_le_dcs_GetTechnology
(
    le_msg_SessionRef_t _ifgen_sessionRef,
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
LE_SHARED le_result_t ifgen_le_dcs_GetState
(
    le_msg_SessionRef_t _ifgen_sessionRef,
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
LE_SHARED le_dcs_ReqObjRef_t ifgen_le_dcs_Start
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_dcs_ChannelRef_t channelRef
        ///< [IN] channel requested to be started
);

//--------------------------------------------------------------------------------------------------
/**
 * Stop for an app its previously started data channel
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_dcs_Stop
(
    le_msg_SessionRef_t _ifgen_sessionRef,
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
LE_SHARED le_dcs_ChannelRef_t ifgen_le_dcs_GetReference
(
    le_msg_SessionRef_t _ifgen_sessionRef,
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
LE_SHARED void ifgen_le_dcs_GetChannels
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_dcs_GetChannelsHandlerFunc_t handlerPtr,
        ///< [IN] requester's handler for receiving results
        void* contextPtr
        ///< [IN]
);

#endif // LE_DCS_COMMON_H_INCLUDE_GUARD