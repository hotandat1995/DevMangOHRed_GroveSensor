
/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */
#ifndef LE_VOICECALL_COMMON_H_INCLUDE_GUARD
#define LE_VOICECALL_COMMON_H_INCLUDE_GUARD


#include "legato.h"

// Interface specific includes
#include "le_mdmDefs_common.h"
#include "le_audio_common.h"

#define IFGEN_LE_VOICECALL_PROTOCOL_ID "181e5028027647a1335449aaac136e5c"
#define IFGEN_LE_VOICECALL_MSG_SIZE 37



//--------------------------------------------------------------------------------------------------
/**
 * Reference returned by Start function and used by End function
 */
//--------------------------------------------------------------------------------------------------
typedef struct le_voicecall_Call* le_voicecall_CallRef_t;


//--------------------------------------------------------------------------------------------------
/**
 * Voice call establishment states.
 */
//--------------------------------------------------------------------------------------------------
typedef enum
{
    LE_VOICECALL_EVENT_ALERTING = 0,
        ///< Voice call establishment in progress.
        ///< Far end is now alerting its user (outgoing call).
    LE_VOICECALL_EVENT_CONNECTED = 1,
        ///< Call has been established, and is media is active.
    LE_VOICECALL_EVENT_TERMINATED = 2,
        ///< Call has terminated.
    LE_VOICECALL_EVENT_OFFLINE = 3,
        ///< NO Service available to try establish a voice call.
    LE_VOICECALL_EVENT_BUSY = 4,
        ///< Remote party (callee) is busy.
    LE_VOICECALL_EVENT_RESOURCE_BUSY = 5,
        ///< All local connection resources (lines/channels) are in use.
    LE_VOICECALL_EVENT_CALL_END_FAILED = 6,
        ///< Call ending failed.
    LE_VOICECALL_EVENT_CALL_ANSWER_FAILED = 7,
        ///< Call answering failed.
    LE_VOICECALL_EVENT_INCOMING = 8
        ///< Incoming voice call in progress.
}
le_voicecall_Event_t;


//--------------------------------------------------------------------------------------------------
/**
 * Voice call termination reason.
 */
//--------------------------------------------------------------------------------------------------
typedef enum
{
    LE_VOICECALL_TERM_NETWORK_FAIL = 0,
        ///< Network could not complete the call.
    LE_VOICECALL_TERM_BAD_ADDRESS = 1,
        ///< Remote address could not be resolved.
    LE_VOICECALL_TERM_BUSY = 2,
        ///< Caller is currently busy and cannot take the call.
    LE_VOICECALL_TERM_LOCAL_ENDED = 3,
        ///< Local party ended the call.
    LE_VOICECALL_TERM_REMOTE_ENDED = 4,
        ///< Remote party ended the call.
    LE_VOICECALL_TERM_UNDEFINED = 5
        ///< Undefined reason.
}
le_voicecall_TerminationReason_t;


//--------------------------------------------------------------------------------------------------
/**
 * Reference type used by Add/Remove functions for EVENT 'le_voicecall_State'
 */
//--------------------------------------------------------------------------------------------------
typedef struct le_voicecall_StateHandler* le_voicecall_StateHandlerRef_t;


//--------------------------------------------------------------------------------------------------
/**
 * Handler for voice call state changes.
 */
//--------------------------------------------------------------------------------------------------
typedef void (*le_voicecall_StateHandlerFunc_t)
(
        le_voicecall_CallRef_t reference,
        ///< Event voice call object reference.
        const char* LE_NONNULL identifier,
        ///< Identifier of the remote party
        ///< associated with the call.
        le_voicecall_Event_t event,
        ///< Voice call event.
        void* contextPtr
        ///<
);


//--------------------------------------------------------------------------------------------------
/**
 * Get if this client bound locally.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED bool ifgen_le_voicecall_HasLocalBinding
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Init data that is common across all threads
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED void ifgen_le_voicecall_InitCommonData
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Perform common initialization and open a session
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_voicecall_OpenSession
(
    le_msg_SessionRef_t _ifgen_sessionRef,
    bool isBlocking
);

//--------------------------------------------------------------------------------------------------
/**
 * Add handler function for EVENT 'le_voicecall_State'
 *
 * This event provides information on voice call state changes
 *
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_voicecall_StateHandlerRef_t ifgen_le_voicecall_AddStateHandler
(
    le_msg_SessionRef_t _ifgen_sessionRef,
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
LE_SHARED void ifgen_le_voicecall_RemoveStateHandler
(
    le_msg_SessionRef_t _ifgen_sessionRef,
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
LE_SHARED le_voicecall_CallRef_t ifgen_le_voicecall_Start
(
    le_msg_SessionRef_t _ifgen_sessionRef,
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
LE_SHARED le_result_t ifgen_le_voicecall_End
(
    le_msg_SessionRef_t _ifgen_sessionRef,
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
LE_SHARED le_result_t ifgen_le_voicecall_Delete
(
    le_msg_SessionRef_t _ifgen_sessionRef,
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
LE_SHARED le_result_t ifgen_le_voicecall_Answer
(
    le_msg_SessionRef_t _ifgen_sessionRef,
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
LE_SHARED le_result_t ifgen_le_voicecall_GetTerminationReason
(
    le_msg_SessionRef_t _ifgen_sessionRef,
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
LE_SHARED le_audio_StreamRef_t ifgen_le_voicecall_GetTxAudioStream
(
    le_msg_SessionRef_t _ifgen_sessionRef,
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
LE_SHARED le_audio_StreamRef_t ifgen_le_voicecall_GetRxAudioStream
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_voicecall_CallRef_t reference
        ///< [IN] Voice call object reference to read from.
);

#endif // LE_VOICECALL_COMMON_H_INCLUDE_GUARD