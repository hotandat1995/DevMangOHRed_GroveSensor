/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef LE_APPINFO_MESSAGES_H_INCLUDE_GUARD
#define LE_APPINFO_MESSAGES_H_INCLUDE_GUARD


#include "le_appInfo_common.h"

#define _MAX_MSG_SIZE IFGEN_LE_APPINFO_MSG_SIZE

// Define the message type for communicating between client and server
typedef struct __attribute__((packed))
{
    uint32_t id;
    uint8_t buffer[_MAX_MSG_SIZE];
}
_Message_t;

#define _MSGID_le_appInfo_GetState 0
#define _MSGID_le_appInfo_GetProcState 1
#define _MSGID_le_appInfo_GetName 2
#define _MSGID_le_appInfo_GetHash 3


// Define type-safe pack/unpack functions for all enums, including included types

static inline bool le_appInfo_PackState
(
    uint8_t **bufferPtr,
    le_appInfo_State_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_appInfo_UnpackState
(
    uint8_t **bufferPtr,
    le_appInfo_State_t* valuePtr
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

static inline bool le_appInfo_PackProcState
(
    uint8_t **bufferPtr,
    le_appInfo_ProcState_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_appInfo_UnpackProcState
(
    uint8_t **bufferPtr,
    le_appInfo_ProcState_t* valuePtr
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


#endif // LE_APPINFO_MESSAGES_H_INCLUDE_GUARD