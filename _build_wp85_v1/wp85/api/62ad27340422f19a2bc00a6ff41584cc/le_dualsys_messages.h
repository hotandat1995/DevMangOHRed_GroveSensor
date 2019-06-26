/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef LE_DUALSYS_MESSAGES_H_INCLUDE_GUARD
#define LE_DUALSYS_MESSAGES_H_INCLUDE_GUARD


#include "le_dualsys_common.h"

#define _MAX_MSG_SIZE IFGEN_LE_DUALSYS_MSG_SIZE

// Define the message type for communicating between client and server
typedef struct __attribute__((packed))
{
    uint32_t id;
    uint8_t buffer[_MAX_MSG_SIZE];
}
_Message_t;

#define _MSGID_le_dualsys_DisableSyncBeforeUpdate 0
#define _MSGID_le_dualsys_SetSystem 1
#define _MSGID_le_dualsys_GetCurrentSystem 2


// Define type-safe pack/unpack functions for all enums, including included types

static inline bool le_dualsys_PackSystem
(
    uint8_t **bufferPtr,
    le_dualsys_System_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_dualsys_UnpackSystem
(
    uint8_t **bufferPtr,
    le_dualsys_System_t* valuePtr
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


#endif // LE_DUALSYS_MESSAGES_H_INCLUDE_GUARD