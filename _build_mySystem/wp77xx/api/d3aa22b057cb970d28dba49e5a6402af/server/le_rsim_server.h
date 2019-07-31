

/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef LE_RSIM_INTERFACE_H_INCLUDE_GUARD
#define LE_RSIM_INTERFACE_H_INCLUDE_GUARD


#include "legato.h"

// Internal includes for this interface
#include "le_rsim_common.h"
//--------------------------------------------------------------------------------------------------
/**
 * Get the server service reference
 */
//--------------------------------------------------------------------------------------------------
le_msg_ServiceRef_t le_rsim_GetServiceRef
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the client session reference for the current message
 */
//--------------------------------------------------------------------------------------------------
le_msg_SessionRef_t le_rsim_GetClientSessionRef
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * Initialize the server and advertise the service.
 */
//--------------------------------------------------------------------------------------------------
void le_rsim_AdvertiseService
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Handler for message notification
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 * Handler for message sending result
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 * Reference type used by Add/Remove functions for EVENT 'le_rsim_Message'
 */
//--------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------
/**
 * Add handler function for EVENT 'le_rsim_Message'
 *
 * This event is used to notify and send a message
 */
//--------------------------------------------------------------------------------------------------
le_rsim_MessageHandlerRef_t le_rsim_AddMessageHandler
(
    le_rsim_MessageHandlerFunc_t handlerPtr,
        ///< [IN]
    void* contextPtr
        ///< [IN]
);



//--------------------------------------------------------------------------------------------------
/**
 * Remove handler function for EVENT 'le_rsim_Message'
 */
//--------------------------------------------------------------------------------------------------
void le_rsim_RemoveMessageHandler
(
    le_rsim_MessageHandlerRef_t handlerRef
        ///< [IN]
);



//--------------------------------------------------------------------------------------------------
/**
 * This function is called by the application to send a message to the Legato remote SIM service
 *
 * @return
 *  - LE_OK             Function succeeded
 *  - LE_BAD_PARAMETER  Message is too long
 *
 * @warning The sending process is asynchronous: only the message length is checked by this function
 * before returning a result. A callback function should be passed as a parameter in order to be
 * notified of the message sending result.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_rsim_SendMessage
(
    const uint8_t* messagePtr,
        ///< [IN] Message to send
    size_t messageSize,
        ///< [IN]
    le_rsim_CallbackHandlerFunc_t callbackPtr,
        ///< [IN] Callback for sending result
    void* contextPtr
        ///< [IN]
);


#endif // LE_RSIM_INTERFACE_H_INCLUDE_GUARD