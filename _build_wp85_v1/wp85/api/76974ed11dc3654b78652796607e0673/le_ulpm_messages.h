/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef LE_ULPM_MESSAGES_H_INCLUDE_GUARD
#define LE_ULPM_MESSAGES_H_INCLUDE_GUARD


#include "le_ulpm_common.h"

#define _MAX_MSG_SIZE IFGEN_LE_ULPM_MSG_SIZE

// Define the message type for communicating between client and server
typedef struct __attribute__((packed))
{
    uint32_t id;
    uint8_t buffer[_MAX_MSG_SIZE];
}
_Message_t;

#define _MSGID_le_ulpm_BootOnGpio 0
#define _MSGID_le_ulpm_BootOnTimer 1
#define _MSGID_le_ulpm_BootOnAdc 2
#define _MSGID_le_ulpm_GetFirmwareVersion 3
#define _MSGID_le_ulpm_ShutDown 4


// Define type-safe pack/unpack functions for all enums, including included types

static inline bool le_ulpm_PackGpioState
(
    uint8_t **bufferPtr,
    le_ulpm_GpioState_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_ulpm_UnpackGpioState
(
    uint8_t **bufferPtr,
    le_ulpm_GpioState_t* valuePtr
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


#endif // LE_ULPM_MESSAGES_H_INCLUDE_GUARD