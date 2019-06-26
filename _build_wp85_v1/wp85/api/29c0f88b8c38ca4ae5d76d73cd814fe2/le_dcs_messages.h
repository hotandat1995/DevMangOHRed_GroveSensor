/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef LE_DCS_MESSAGES_H_INCLUDE_GUARD
#define LE_DCS_MESSAGES_H_INCLUDE_GUARD


#include "le_dcs_common.h"

#define _MAX_MSG_SIZE IFGEN_LE_DCS_MSG_SIZE

// Define the message type for communicating between client and server
typedef struct __attribute__((packed))
{
    uint32_t id;
    uint8_t buffer[_MAX_MSG_SIZE];
}
_Message_t;

#define _MSGID_le_dcs_AddEventHandler 0
#define _MSGID_le_dcs_RemoveEventHandler 1
#define _MSGID_le_dcs_GetTechnology 2
#define _MSGID_le_dcs_GetState 3
#define _MSGID_le_dcs_Start 4
#define _MSGID_le_dcs_Stop 5
#define _MSGID_le_dcs_GetReference 6
#define _MSGID_le_dcs_GetChannels 7


// Define type-safe pack/unpack functions for all enums, including included types

static inline bool le_dcs_PackTechnology
(
    uint8_t **bufferPtr,
    le_dcs_Technology_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_dcs_UnpackTechnology
(
    uint8_t **bufferPtr,
    le_dcs_Technology_t* valuePtr
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

static inline bool le_dcs_PackState
(
    uint8_t **bufferPtr,
    le_dcs_State_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_dcs_UnpackState
(
    uint8_t **bufferPtr,
    le_dcs_State_t* valuePtr
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

static inline bool le_dcs_PackEvent
(
    uint8_t **bufferPtr,
    le_dcs_Event_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_dcs_UnpackEvent
(
    uint8_t **bufferPtr,
    le_dcs_Event_t* valuePtr
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


static inline bool le_dcs_PackChannelInfo
(
    uint8_t **bufferPtr,
    const le_dcs_ChannelInfo_t *valuePtr
)
{
    __attribute__((unused))
    bool subResult, result = true;

    LE_ASSERT(valuePtr);
    subResult = le_pack_PackReference( bufferPtr,
                                                 valuePtr->ref );
    result = result && subResult;
    subResult = le_pack_PackString( bufferPtr,
                                    valuePtr->name, 32);
    result = result && subResult;
    subResult = le_dcs_PackTechnology( bufferPtr,
                                                 valuePtr->technology );
    result = result && subResult;
    subResult = le_dcs_PackState( bufferPtr,
                                                 valuePtr->state );
    result = result && subResult;

    return result;
}

static inline bool le_dcs_UnpackChannelInfo
(
    uint8_t **bufferPtr,
    le_dcs_ChannelInfo_t *valuePtr
)
{
    bool result = true;
    if (result)
    {
        result = le_pack_UnpackReference(bufferPtr,
                                                   &valuePtr->ref );
    }
    if (result)
    {
        result = le_pack_UnpackString(bufferPtr,
                                      valuePtr->name,
                                      sizeof(valuePtr->name),
                                      32);
    }
    if (result)
    {
        result = le_dcs_UnpackTechnology(bufferPtr,
                                                   &valuePtr->technology );
    }
    if (result)
    {
        result = le_dcs_UnpackState(bufferPtr,
                                                   &valuePtr->state );
    }
    return result;
}

#endif // LE_DCS_MESSAGES_H_INCLUDE_GUARD