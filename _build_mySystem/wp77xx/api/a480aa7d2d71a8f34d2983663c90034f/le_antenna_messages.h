/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef LE_ANTENNA_MESSAGES_H_INCLUDE_GUARD
#define LE_ANTENNA_MESSAGES_H_INCLUDE_GUARD


#include "le_antenna_common.h"

#define _MAX_MSG_SIZE IFGEN_LE_ANTENNA_MSG_SIZE

// Define the message type for communicating between client and server
typedef struct __attribute__((packed))
{
    uint32_t id;
    uint8_t buffer[_MAX_MSG_SIZE];
}
_Message_t;

#define _MSGID_le_antenna_Request 0
#define _MSGID_le_antenna_GetType 1
#define _MSGID_le_antenna_SetShortLimit 2
#define _MSGID_le_antenna_GetShortLimit 3
#define _MSGID_le_antenna_SetOpenLimit 4
#define _MSGID_le_antenna_GetOpenLimit 5
#define _MSGID_le_antenna_AddStatusEventHandler 6
#define _MSGID_le_antenna_RemoveStatusEventHandler 7
#define _MSGID_le_antenna_GetStatus 8
#define _MSGID_le_antenna_SetExternalAdc 9
#define _MSGID_le_antenna_GetExternalAdc 10


// Define type-safe pack/unpack functions for all enums, including included types

static inline bool le_antenna_PackType
(
    uint8_t **bufferPtr,
    le_antenna_Type_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_antenna_UnpackType
(
    uint8_t **bufferPtr,
    le_antenna_Type_t* valuePtr
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

static inline bool le_antenna_PackStatus
(
    uint8_t **bufferPtr,
    le_antenna_Status_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_antenna_UnpackStatus
(
    uint8_t **bufferPtr,
    le_antenna_Status_t* valuePtr
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


#endif // LE_ANTENNA_MESSAGES_H_INCLUDE_GUARD