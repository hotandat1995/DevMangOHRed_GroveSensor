/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef LE_UPDATE_MESSAGES_H_INCLUDE_GUARD
#define LE_UPDATE_MESSAGES_H_INCLUDE_GUARD


#include "le_update_common.h"

#define _MAX_MSG_SIZE IFGEN_LE_UPDATE_MSG_SIZE

// Define the message type for communicating between client and server
typedef struct __attribute__((packed))
{
    uint32_t id;
    uint8_t buffer[_MAX_MSG_SIZE];
}
_Message_t;

#define _MSGID_le_update_AddProgressHandler 0
#define _MSGID_le_update_RemoveProgressHandler 1
#define _MSGID_le_update_Start 2
#define _MSGID_le_update_Install 3
#define _MSGID_le_update_End 4
#define _MSGID_le_update_GetErrorCode 5
#define _MSGID_le_update_GetCurrentSysIndex 6
#define _MSGID_le_update_GetSystemHash 7
#define _MSGID_le_update_GetPreviousSystemIndex 8


// Define type-safe pack/unpack functions for all enums, including included types

static inline bool le_update_PackState
(
    uint8_t **bufferPtr,
    le_update_State_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_update_UnpackState
(
    uint8_t **bufferPtr,
    le_update_State_t* valuePtr
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

static inline bool le_update_PackErrorCode
(
    uint8_t **bufferPtr,
    le_update_ErrorCode_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_update_UnpackErrorCode
(
    uint8_t **bufferPtr,
    le_update_ErrorCode_t* valuePtr
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


#endif // LE_UPDATE_MESSAGES_H_INCLUDE_GUARD