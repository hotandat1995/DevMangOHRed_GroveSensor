/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef LE_PORT_MESSAGES_H_INCLUDE_GUARD
#define LE_PORT_MESSAGES_H_INCLUDE_GUARD


#include "le_port_common.h"

#define _MAX_MSG_SIZE IFGEN_LE_PORT_MSG_SIZE

// Define the message type for communicating between client and server
typedef struct __attribute__((packed))
{
    uint32_t id;
    uint8_t buffer[_MAX_MSG_SIZE];
}
_Message_t;

#define _MSGID_le_port_Request 0
#define _MSGID_le_port_SetDataMode 1
#define _MSGID_le_port_SetCommandMode 2
#define _MSGID_le_port_Release 3
#define _MSGID_le_port_GetPortReference 4


// Define type-safe pack/unpack functions for all enums, including included types

static inline bool le_atServer_PackType
(
    uint8_t **bufferPtr,
    le_atServer_Type_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_atServer_UnpackType
(
    uint8_t **bufferPtr,
    le_atServer_Type_t* valuePtr
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

static inline bool le_atServer_PackFinalRsp
(
    uint8_t **bufferPtr,
    le_atServer_FinalRsp_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_atServer_UnpackFinalRsp
(
    uint8_t **bufferPtr,
    le_atServer_FinalRsp_t* valuePtr
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

static inline bool le_atServer_PackAvailableDevice
(
    uint8_t **bufferPtr,
    le_atServer_AvailableDevice_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_atServer_UnpackAvailableDevice
(
    uint8_t **bufferPtr,
    le_atServer_AvailableDevice_t* valuePtr
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


#endif // LE_PORT_MESSAGES_H_INCLUDE_GUARD