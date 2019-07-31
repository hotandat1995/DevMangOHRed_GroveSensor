

/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef LE_VOICECALL_INTERFACE_H_INCLUDE_GUARD
#define LE_VOICECALL_INTERFACE_H_INCLUDE_GUARD


#include "legato.h"

// Interface specific includes
#include "le_mdmDefs_server.h"
#include "le_audio_server.h"

// Internal includes for this interface
#include "le_voicecall_common.h"
//--------------------------------------------------------------------------------------------------
/**
 * Get the server service reference
 */
//--------------------------------------------------------------------------------------------------
le_msg_ServiceRef_t le_voicecall_GetServiceRef
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the client session reference for the current message
 */
//--------------------------------------------------------------------------------------------------
le_msg_SessionRef_t le_voicecall_GetClientSessionRef
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * Initialize the server and advertise the service.
 */
//--------------------------------------------------------------------------------------------------
void le_voicecall_AdvertiseService
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Reference returned by Start function and used by End function
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 * Voice call establishment states.
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 * Voice call termination reason.
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 * Handler for voice call state changes.
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 * Reference type used by Add/Remove functions for EVENT 'le_voicecall_State'
 */
//--------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------
/**
 * Add handler function for EVENT 'le_voicecall_State'
 *
 * This event provides information on voice call state changes
 *
 */
//--------------------------------------------------------------------------------------------------
le_voicecall_StateHandlerRef_t le_voicecall_AddStateHandler
(
    le_voicecall_StateHandlerFunc_t handlerPtr,
        ///< [IN]
    void* contextPtr
        ///< [IN]
);



//--------------------------------------------------------------------------------------------------
/**
 * Remove handler function for EVENT 'le_voicecall_State'
 */
//--------------------------------------------------------------------------------------------------
void le_voicecall_RemoveStateHandler
(
    le_voicecall_StateHandlerRef_t handlerRef
        ///< [IN]
);



//--------------------------------------------------------------------------------------------------
/**
 * Start a voice call.
 *
 * @return
 *      - Reference to the voice call (to be used later for releasing the voice call)
 *      - NULL if the voice call could not be processed
 */
//--------------------------------------------------------------------------------------------------
le_voicecall_CallRef_t le_voicecall_Start
(
    const char* LE_NONNULL DestinationID
        ///< [IN] Destination identifier for the voice
        ///< call establishment.
);



//--------------------------------------------------------------------------------------------------
/**
 * Release a voice call.
 *
 * @return
 *      - LE_OK if the end of voice call can be processed.
 *      - LE_NOT_FOUND if the voice call object reference is not found.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_voicecall_End
(
    le_voicecall_CallRef_t reference
        ///< [IN] Voice call object reference to hang-up.
);



//--------------------------------------------------------------------------------------------------
/**
 * Delete voice call object reference create by le_voicecall_Start() or an incoming voice call.
 *
 * @return
 *      - LE_OK if the delete of voice call can be processed.
 *      - LE_FAULT if the voice call is not terminated.
 *      - LE_NOT_FOUND if the voice call object reference is not found.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_voicecall_Delete
(
    le_voicecall_CallRef_t reference
        ///< [IN] Voice call object reference to delete.
);



//--------------------------------------------------------------------------------------------------
/**
 * Answer to incoming voice call.
 *
 * @return
 *      - LE_OK if the incoming voice call can be answered
 *      - LE_NOT_FOUND if the incoming voice call object reference is not found.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_voicecall_Answer
(
    le_voicecall_CallRef_t reference
        ///< [IN] Incoming voice call object reference to answer.
);



//--------------------------------------------------------------------------------------------------
/**
 * Get the termination reason of a voice call reference.
 *
 * @return
 *      - LE_OK if the termination reason is got
 *      - LE_NOT_FOUND if the incoming voice call object reference is not found.
 *      - LE_FAULT if the voice call is not terminated.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_voicecall_GetTerminationReason
(
    le_voicecall_CallRef_t reference,
        ///< [IN] Voice call object reference to read from.
    le_voicecall_TerminationReason_t* reasonPtr
        ///< [OUT] Termination reason of the voice call.
);



//--------------------------------------------------------------------------------------------------
/**
 * Called to get the transmitted audio stream. All audio generated on this
 * end of the call is sent on this stream.
 *
 * @return Transmitted audio stream reference.
 */
//--------------------------------------------------------------------------------------------------
le_audio_StreamRef_t le_voicecall_GetTxAudioStream
(
    le_voicecall_CallRef_t reference
        ///< [IN] Voice call object reference to read from.
);



//--------------------------------------------------------------------------------------------------
/**
 * Called to get the received audio stream. All audio received from the
 * other end of the call is received on this stream.
 *
 * @return Received audio stream reference.
 */
//--------------------------------------------------------------------------------------------------
le_audio_StreamRef_t le_voicecall_GetRxAudioStream
(
    le_voicecall_CallRef_t reference
        ///< [IN] Voice call object reference to read from.
);


#endif // LE_VOICECALL_INTERFACE_H_INCLUDE_GUARD