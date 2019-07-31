/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef LE_VOICECALL_MESSAGES_H_INCLUDE_GUARD
#define LE_VOICECALL_MESSAGES_H_INCLUDE_GUARD


#include "le_voicecall_common.h"

#define _MAX_MSG_SIZE IFGEN_LE_VOICECALL_MSG_SIZE

// Define the message type for communicating between client and server
typedef struct __attribute__((packed))
{
    uint32_t id;
    uint8_t buffer[_MAX_MSG_SIZE];
}
_Message_t;

#define _MSGID_le_voicecall_AddStateHandler 0
#define _MSGID_le_voicecall_RemoveStateHandler 1
#define _MSGID_le_voicecall_Start 2
#define _MSGID_le_voicecall_End 3
#define _MSGID_le_voicecall_Delete 4
#define _MSGID_le_voicecall_Answer 5
#define _MSGID_le_voicecall_GetTerminationReason 6
#define _MSGID_le_voicecall_GetTxAudioStream 7
#define _MSGID_le_voicecall_GetRxAudioStream 8


// Define type-safe pack/unpack functions for all enums, including included types

static inline bool le_voicecall_PackEvent
(
    uint8_t **bufferPtr,
    le_voicecall_Event_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_voicecall_UnpackEvent
(
    uint8_t **bufferPtr,
    le_voicecall_Event_t* valuePtr
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

static inline bool le_voicecall_PackTerminationReason
(
    uint8_t **bufferPtr,
    le_voicecall_TerminationReason_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_voicecall_UnpackTerminationReason
(
    uint8_t **bufferPtr,
    le_voicecall_TerminationReason_t* valuePtr
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

static inline bool le_audio_PackMediaEvent
(
    uint8_t **bufferPtr,
    le_audio_MediaEvent_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_audio_UnpackMediaEvent
(
    uint8_t **bufferPtr,
    le_audio_MediaEvent_t* valuePtr
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

static inline bool le_audio_PackCompanding
(
    uint8_t **bufferPtr,
    le_audio_Companding_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_audio_UnpackCompanding
(
    uint8_t **bufferPtr,
    le_audio_Companding_t* valuePtr
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

static inline bool le_audio_PackI2SChannel
(
    uint8_t **bufferPtr,
    le_audio_I2SChannel_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_audio_UnpackI2SChannel
(
    uint8_t **bufferPtr,
    le_audio_I2SChannel_t* valuePtr
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

static inline bool le_audio_PackFormat
(
    uint8_t **bufferPtr,
    le_audio_Format_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_audio_UnpackFormat
(
    uint8_t **bufferPtr,
    le_audio_Format_t* valuePtr
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

static inline bool le_audio_PackAmrMode
(
    uint8_t **bufferPtr,
    le_audio_AmrMode_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_audio_UnpackAmrMode
(
    uint8_t **bufferPtr,
    le_audio_AmrMode_t* valuePtr
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


#endif // LE_VOICECALL_MESSAGES_H_INCLUDE_GUARD