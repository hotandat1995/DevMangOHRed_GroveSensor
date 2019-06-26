/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef LE_CELLNET_MESSAGES_H_INCLUDE_GUARD
#define LE_CELLNET_MESSAGES_H_INCLUDE_GUARD


#include "le_cellnet_common.h"

#define _MAX_MSG_SIZE IFGEN_LE_CELLNET_MSG_SIZE

// Define the message type for communicating between client and server
typedef struct __attribute__((packed))
{
    uint32_t id;
    uint8_t buffer[_MAX_MSG_SIZE];
}
_Message_t;

#define _MSGID_le_cellnet_AddStateEventHandler 0
#define _MSGID_le_cellnet_RemoveStateEventHandler 1
#define _MSGID_le_cellnet_Request 2
#define _MSGID_le_cellnet_Release 3
#define _MSGID_le_cellnet_SetSimPinCode 4
#define _MSGID_le_cellnet_GetSimPinCode 5
#define _MSGID_le_cellnet_GetNetworkState 6


// Define type-safe pack/unpack functions for all enums, including included types

static inline bool le_cellnet_PackState
(
    uint8_t **bufferPtr,
    le_cellnet_State_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_cellnet_UnpackState
(
    uint8_t **bufferPtr,
    le_cellnet_State_t* valuePtr
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

static inline bool le_sim_PackStates
(
    uint8_t **bufferPtr,
    le_sim_States_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_sim_UnpackStates
(
    uint8_t **bufferPtr,
    le_sim_States_t* valuePtr
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

static inline bool le_sim_PackId
(
    uint8_t **bufferPtr,
    le_sim_Id_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_sim_UnpackId
(
    uint8_t **bufferPtr,
    le_sim_Id_t* valuePtr
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

static inline bool le_sim_PackManufacturer
(
    uint8_t **bufferPtr,
    le_sim_Manufacturer_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_sim_UnpackManufacturer
(
    uint8_t **bufferPtr,
    le_sim_Manufacturer_t* valuePtr
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

static inline bool le_sim_PackCommand
(
    uint8_t **bufferPtr,
    le_sim_Command_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_sim_UnpackCommand
(
    uint8_t **bufferPtr,
    le_sim_Command_t* valuePtr
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

static inline bool le_sim_PackStkEvent
(
    uint8_t **bufferPtr,
    le_sim_StkEvent_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_sim_UnpackStkEvent
(
    uint8_t **bufferPtr,
    le_sim_StkEvent_t* valuePtr
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

static inline bool le_sim_PackStkRefreshMode
(
    uint8_t **bufferPtr,
    le_sim_StkRefreshMode_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_sim_UnpackStkRefreshMode
(
    uint8_t **bufferPtr,
    le_sim_StkRefreshMode_t* valuePtr
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

static inline bool le_sim_PackStkRefreshStage
(
    uint8_t **bufferPtr,
    le_sim_StkRefreshStage_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_sim_UnpackStkRefreshStage
(
    uint8_t **bufferPtr,
    le_sim_StkRefreshStage_t* valuePtr
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

static inline bool le_mdmDefs_PackIpVersion
(
    uint8_t **bufferPtr,
    le_mdmDefs_IpVersion_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_mdmDefs_UnpackIpVersion
(
    uint8_t **bufferPtr,
    le_mdmDefs_IpVersion_t* valuePtr
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


#endif // LE_CELLNET_MESSAGES_H_INCLUDE_GUARD