/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef LE_COAP_MESSAGES_H_INCLUDE_GUARD
#define LE_COAP_MESSAGES_H_INCLUDE_GUARD


#include "le_coap_common.h"

#define _MAX_MSG_SIZE IFGEN_LE_COAP_MSG_SIZE

// Define the message type for communicating between client and server
typedef struct __attribute__((packed))
{
    uint32_t id;
    uint8_t buffer[_MAX_MSG_SIZE];
}
_Message_t;

#define _MSGID_le_coap_SendResponse 0
#define _MSGID_le_coap_AddMessageEventHandler 1
#define _MSGID_le_coap_RemoveMessageEventHandler 2
#define _MSGID_le_coap_Push 3


// Define type-safe pack/unpack functions for all enums, including included types

static inline bool le_coap_PackCode
(
    uint8_t **bufferPtr,
    le_coap_Code_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_coap_UnpackCode
(
    uint8_t **bufferPtr,
    le_coap_Code_t* valuePtr
)
{
    bool result;
    uint32_t value = 0;
    result = le_pack_UnpackUint32(bufferPtr, &value);
    if (result)
    {
        *valuePtr = value;
    }
    return result;
}

static inline bool le_coap_PackStreamStatus
(
    uint8_t **bufferPtr,
    le_coap_StreamStatus_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_coap_UnpackStreamStatus
(
    uint8_t **bufferPtr,
    le_coap_StreamStatus_t* valuePtr
)
{
    bool result;
    uint32_t value = 0;
    result = le_pack_UnpackUint32(bufferPtr, &value);
    if (result)
    {
        *valuePtr = value;
    }
    return result;
}

static inline bool le_coap_PackPushStatus
(
    uint8_t **bufferPtr,
    le_coap_PushStatus_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_coap_UnpackPushStatus
(
    uint8_t **bufferPtr,
    le_coap_PushStatus_t* valuePtr
)
{
    bool result;
    uint32_t value = 0;
    result = le_pack_UnpackUint32(bufferPtr, &value);
    if (result)
    {
        *valuePtr = value;
    }
    return result;
}

// Define pack/unpack functions for all structures, including included types


#endif // LE_COAP_MESSAGES_H_INCLUDE_GUARD