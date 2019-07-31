
/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */
#ifndef LE_RSIM_COMMON_H_INCLUDE_GUARD
#define LE_RSIM_COMMON_H_INCLUDE_GUARD


#include "legato.h"

#define IFGEN_LE_RSIM_PROTOCOL_ID "b09e1d24fc5dc8036c1bf572dbd40b21"
#define IFGEN_LE_RSIM_MSG_SIZE 292



//--------------------------------------------------------------------------------------------------
/**
 * Remote SIM maximal message size
 *
 * @note
 *  The maximal message size of 276 bytes is based on the longest possible SAP message.
 *  This message is a @c TRANSFER_APDU_RESP composed of a 4-byte SAP header, a 8-byte ResultCode,
 *  a 4-byte ResponseAPDU header, 256 bytes of APDU data, 2 status bytes and 2 padding bytes.
 */
//--------------------------------------------------------------------------------------------------
#define LE_RSIM_MAX_MSG_SIZE 276

//--------------------------------------------------------------------------------------------------
/**
 ** Remote SIM minimal message size
 **
 ** @note
 **  This minimal length is set to ensure that the communication with the remote SIM server
 **  can be effective.
 */
//--------------------------------------------------------------------------------------------------
#define LE_RSIM_MIN_MSG_SIZE 200

//--------------------------------------------------------------------------------------------------
/**
 * Reference type used by Add/Remove functions for EVENT 'le_rsim_Message'
 */
//--------------------------------------------------------------------------------------------------
typedef struct le_rsim_MessageHandler* le_rsim_MessageHandlerRef_t;


//--------------------------------------------------------------------------------------------------
/**
 * Handler for message notification
 */
//--------------------------------------------------------------------------------------------------
typedef void (*le_rsim_MessageHandlerFunc_t)
(
        const uint8_t* messagePtr,
        ///< Message to notify
        size_t messageSize,
        ///<
        void* contextPtr
        ///<
);

//--------------------------------------------------------------------------------------------------
/**
 * Handler for message sending result
 */
//--------------------------------------------------------------------------------------------------
typedef void (*le_rsim_CallbackHandlerFunc_t)
(
        uint8_t messageId,
        ///< Message identifier
        le_result_t result,
        ///< Sending result
        void* contextPtr
        ///<
);


//--------------------------------------------------------------------------------------------------
/**
 * Get if this client bound locally.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED bool ifgen_le_rsim_HasLocalBinding
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Init data that is common across all threads
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED void ifgen_le_rsim_InitCommonData
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Perform common initialization and open a session
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_rsim_OpenSession
(
    le_msg_SessionRef_t _ifgen_sessionRef,
    bool isBlocking
);

//--------------------------------------------------------------------------------------------------
/**
 * Add handler function for EVENT 'le_rsim_Message'
 *
 * This event is used to notify and send a message
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_rsim_MessageHandlerRef_t ifgen_le_rsim_AddMessageHandler
(
    le_msg_SessionRef_t _ifgen_sessionRef,
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
LE_SHARED void ifgen_le_rsim_RemoveMessageHandler
(
    le_msg_SessionRef_t _ifgen_sessionRef,
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
LE_SHARED le_result_t ifgen_le_rsim_SendMessage
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        const uint8_t* messagePtr,
        ///< [IN] Message to send
        size_t messageSize,
        ///< [IN]
        le_rsim_CallbackHandlerFunc_t callbackPtr,
        ///< [IN] Callback for sending result
        void* contextPtr
        ///< [IN]
);

#endif // LE_RSIM_COMMON_H_INCLUDE_GUARD