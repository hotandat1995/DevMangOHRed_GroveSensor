
/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */
#ifndef LE_COAP_COMMON_H_INCLUDE_GUARD
#define LE_COAP_COMMON_H_INCLUDE_GUARD


#include "legato.h"

#define IFGEN_LE_COAP_PROTOCOL_ID "62302f8c5709ff41e77a52d69f1e1a4c"
#define IFGEN_LE_COAP_MSG_SIZE 1323



//--------------------------------------------------------------------------------------------------
/**
 * Maximum length of CoAP URI
 */
//--------------------------------------------------------------------------------------------------
#define LE_COAP_MAX_URI_LENGTH 256

//--------------------------------------------------------------------------------------------------
/**
 * Maximum length of CoAP Token
 */
//--------------------------------------------------------------------------------------------------
#define LE_COAP_MAX_TOKEN_LENGTH 8

//--------------------------------------------------------------------------------------------------
/**
 * Maximum length of CoAP Payload
 */
//--------------------------------------------------------------------------------------------------
#define LE_COAP_MAX_PAYLOAD 1024

//--------------------------------------------------------------------------------------------------
/**
 * Maximum number of bytes of CoAP URI
 */
//--------------------------------------------------------------------------------------------------
#define LE_COAP_MAX_URI_NUM_BYTES 257

//--------------------------------------------------------------------------------------------------
/**
 * Maximum number of bytes of CoAP Token
 */
//--------------------------------------------------------------------------------------------------
#define LE_COAP_MAX_TOKEN_NUM_BYTES 9

//--------------------------------------------------------------------------------------------------
/**
 * Maximum number of bytes of CoAP Payload
 */
//--------------------------------------------------------------------------------------------------
#define LE_COAP_MAX_PAYLOAD_NUM_BYTES 1025

//--------------------------------------------------------------------------------------------------
/**
 * CoAP method and response codes as defined in RFC 7252
 */
//--------------------------------------------------------------------------------------------------
typedef enum
{
    LE_COAP_CODE_EMPTY_MESSAGE = 0,
        ///<
    LE_COAP_CODE_GET = 1,
        ///<
    LE_COAP_CODE_POST = 2,
        ///<
    LE_COAP_CODE_PUT = 3,
        ///<
    LE_COAP_CODE_DELETE = 4,
        ///<
    LE_COAP_CODE_201_CREATED = 65,
        ///<
    LE_COAP_CODE_202_DELETED = 66,
        ///<
    LE_COAP_CODE_203_VALID = 67,
        ///<
    LE_COAP_CODE_204_CHANGED = 68,
        ///<
    LE_COAP_CODE_205_CONTENT = 69,
        ///<
    LE_COAP_CODE_231_CONTINUE = 95,
        ///<
    LE_COAP_CODE_400_BAD_REQUEST = 128,
        ///<
    LE_COAP_CODE_401_UNAUTHORIZED = 129,
        ///<
    LE_COAP_CODE_402_BAD_OPTION = 130,
        ///<
    LE_COAP_CODE_403_FORBIDDEN = 131,
        ///<
    LE_COAP_CODE_404_NOT_FOUND = 132,
        ///<
    LE_COAP_CODE_405_METHOD_NOT_ALLOWED = 133,
        ///<
    LE_COAP_CODE_406_NOT_ACCEPTABLE = 134,
        ///<
    LE_COAP_CODE_408_REQUEST_ENTITY_INCOMPLETE = 136,
        ///<
    LE_COAP_CODE_412_PRECONDITION_FAILED = 140,
        ///<
    LE_COAP_CODE_413_REQUEST_ENTITY_TOO_LARGE = 141,
        ///<
    LE_COAP_CODE_415_UNSUPPORTED_CONTENT_FORMAT = 143,
        ///<
    LE_COAP_CODE_500_INTERNAL_SERVER_ERROR = 160,
        ///<
    LE_COAP_CODE_501_NOT_IMPLEMENTED = 161,
        ///<
    LE_COAP_CODE_502_BAD_GATEWAY = 162,
        ///<
    LE_COAP_CODE_503_SERVICE_UNAVAILABLE = 163,
        ///<
    LE_COAP_CODE_504_GATEWAY_TIMEOUT = 164,
        ///<
    LE_COAP_CODE_505_PROXYING_NOT_SUPPORTED = 165,
        ///<
    LE_COAP_CODE_NO_RESPONSE = 255
        ///<
}
le_coap_Code_t;


//--------------------------------------------------------------------------------------------------
/**
 * CoAP stream status (write request block-2 & read response block-1).
 *
 * Note: This enum is used for receiving CoAP messages and also starting a transmit stream from
 * the device. TX_STREAM_START/ TX_STREAM_END will never be received on incoming CoAP messages but
 * can be used to indicate status of outgoing stream using le_coap_SendResponse() api.
 */
//--------------------------------------------------------------------------------------------------
typedef enum
{
    LE_COAP_STREAM_NONE = 0,
        ///< Payload is less than 1KB.
    LE_COAP_RX_STREAM_START = 1,
        ///< Beginning of a new CoAP receive stream
    LE_COAP_RX_STREAM_IN_PROGRESS = 2,
        ///< Incoming CoAP Stream is in progress
    LE_COAP_RX_STREAM_END = 3,
        ///< Incoming CoAP Stream completed successfully
    LE_COAP_RX_STREAM_ERROR = 4,
        ///< Error in receiving incoming stream
    LE_COAP_TX_STREAM_START = 5,
        ///< Starting a transmit Stream
    LE_COAP_TX_STREAM_IN_PROGRESS = 6,
        ///< Continue streaming
    LE_COAP_TX_STREAM_END = 7,
        ///< All blocks sent out successfully
    LE_COAP_TX_STREAM_ERROR = 8,
        ///< Error in sending stream
    LE_COAP_STREAM_INVALID = 9
        ///< Invalid stream status
}
le_coap_StreamStatus_t;


//--------------------------------------------------------------------------------------------------
/**
 * Push status
 */
//--------------------------------------------------------------------------------------------------
typedef enum
{
    LE_COAP_PUSH_IDLE = 0,
        ///<
    LE_COAP_PUSH_START = 1,
        ///<
    LE_COAP_PUSH_IN_PROGRESS = 2,
        ///<
    LE_COAP_PUSH_SUCCESS = 3,
        ///<
    LE_COAP_PUSH_FAILED = 4,
        ///<
    LE_COAP_PUSH_TERMINATE = 5
        ///<
}
le_coap_PushStatus_t;


//--------------------------------------------------------------------------------------------------
/**
 * Reference type used by Add/Remove functions for EVENT 'le_coap_MessageEvent'
 */
//--------------------------------------------------------------------------------------------------
typedef struct le_coap_MessageEventHandler* le_coap_MessageEventHandlerRef_t;


//--------------------------------------------------------------------------------------------------
/**
 * Handler for receiving incoming CoAP messages
 *
 * @note
 *     Refer to CoAP Message Format https://tools.ietf.org/html/rfc7252#section-2.1 to know how to
 *     use Message ID and Token in a CoAP message.
 *
 *     Content type of payload is dependent on the server implementation. CoAP content formats
 *     Registry https://tools.ietf.org/html/rfc7252#section-12.3 describes how a content format is
 *     assigned. The CoAP Content Formats sub-registry is at
 *     https://www.iana.org/assignments/core-parameters/core-parameters.xhtml#content-formats.
 *     A server might support a different format other than the ones described in the sub registry.
 */
//--------------------------------------------------------------------------------------------------
typedef void (*le_coap_MessageHandlerFunc_t)
(
        le_coap_Code_t code,
        ///< CoAP Code
        le_coap_StreamStatus_t stream,
        ///< CoAP stream status
        uint16_t messageId,
        ///< Message id in CoAP message header
        uint16_t contentType,
        ///< Content type of the payload
        const char* LE_NONNULL uri,
        ///< URI of resource
        const uint8_t* tokenPtr,
        ///< Token in CoAP message header
        size_t tokenSize,
        ///<
        const uint8_t* payloadPtr,
        ///< CoAP payload
        size_t payloadSize,
        ///<
        void* contextPtr
        ///<
);

//--------------------------------------------------------------------------------------------------
/**
 * Handler for receiving response to push operation.
 */
//--------------------------------------------------------------------------------------------------
typedef void (*le_coap_PushHandlerFunc_t)
(
        le_coap_PushStatus_t status,
        ///< push status
        void* contextPtr
        ///<
);


//--------------------------------------------------------------------------------------------------
/**
 * Get if this client bound locally.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED bool ifgen_le_coap_HasLocalBinding
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Init data that is common across all threads
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED void ifgen_le_coap_InitCommonData
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Perform common initialization and open a session
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_coap_OpenSession
(
    le_msg_SessionRef_t _ifgen_sessionRef,
    bool isBlocking
);

//--------------------------------------------------------------------------------------------------
/**
 * Sends asynchronous CoAP response to server.
 *
 * @return
 *      - LE_OK on success.
 *      - LE_FAULT if failed.
 *
 * Note: This API will return success if it successful in sending the message down the stack.
 * Retransmission will be handled at CoAP layer.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_coap_SendResponse
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        uint16_t messageId,
        ///< [IN] Message id in CoAP message header
        const uint8_t* tokenPtr,
        ///< [IN] Token in CoAP message header
        size_t tokenSize,
        ///< [IN]
        uint16_t contentType,
        ///< [IN] Content type of the payload
        le_coap_Code_t responseCode,
        ///< [IN] Result of CoAP operation
        le_coap_StreamStatus_t streamStatus,
        ///< [IN] Status of transmit stream
        const uint8_t* payloadPtr,
        ///< [IN] CoAP payload
        size_t payloadSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Add handler function for EVENT 'le_coap_MessageEvent'
 *
 * This event sends the incoming CoAP message that are not handled by LwM2M to application.
 *
 * Note: It the content-type of incoming message is not recognizable at CoAP layer, those messages
 * are sent to application for processing.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_coap_MessageEventHandlerRef_t ifgen_le_coap_AddMessageEventHandler
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_coap_MessageHandlerFunc_t handlerPtr,
        ///< [IN]
        void* contextPtr
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Remove handler function for EVENT 'le_coap_MessageEvent'
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED void ifgen_le_coap_RemoveMessageEventHandler
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_coap_MessageEventHandlerRef_t handlerRef
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * This function sends unsolicited CoAP push messages to the server. Responses to push will be
 * received by message handler function.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_coap_Push
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        const char* LE_NONNULL uri,
        ///< [IN] URI where push should end
        const uint8_t* tokenPtr,
        ///< [IN] Token in CoAP message header
        size_t tokenSize,
        ///< [IN]
        uint16_t contentType,
        ///< [IN] Content type of the payload
        le_coap_StreamStatus_t streamStatus,
        ///< [IN] Status of transmit stream
        const uint8_t* payloadPtr,
        ///< [IN] CoAP Payload
        size_t payloadSize,
        ///< [IN]
        le_coap_PushHandlerFunc_t pushHandlerRefPtr,
        ///< [IN] Handler for receiving response
        void* contextPtr
        ///< [IN]
);

#endif // LE_COAP_COMMON_H_INCLUDE_GUARD