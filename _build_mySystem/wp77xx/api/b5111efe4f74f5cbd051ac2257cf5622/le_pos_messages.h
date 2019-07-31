/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef LE_POS_MESSAGES_H_INCLUDE_GUARD
#define LE_POS_MESSAGES_H_INCLUDE_GUARD


#include "le_pos_common.h"

#define _MAX_MSG_SIZE IFGEN_LE_POS_MSG_SIZE

// Define the message type for communicating between client and server
typedef struct __attribute__((packed))
{
    uint32_t id;
    uint8_t buffer[_MAX_MSG_SIZE];
}
_Message_t;

#define _MSGID_le_pos_AddMovementHandler 0
#define _MSGID_le_pos_RemoveMovementHandler 1
#define _MSGID_le_pos_Get2DLocation 2
#define _MSGID_le_pos_Get3DLocation 3
#define _MSGID_le_pos_GetTime 4
#define _MSGID_le_pos_GetDate 5
#define _MSGID_le_pos_GetMotion 6
#define _MSGID_le_pos_GetHeading 7
#define _MSGID_le_pos_GetDirection 8
#define _MSGID_le_pos_GetFixState 9
#define _MSGID_le_pos_sample_Get2DLocation 10
#define _MSGID_le_pos_sample_GetTime 11
#define _MSGID_le_pos_sample_GetDate 12
#define _MSGID_le_pos_sample_GetAltitude 13
#define _MSGID_le_pos_sample_GetHorizontalSpeed 14
#define _MSGID_le_pos_sample_GetVerticalSpeed 15
#define _MSGID_le_pos_sample_GetHeading 16
#define _MSGID_le_pos_sample_GetDirection 17
#define _MSGID_le_pos_sample_GetFixState 18
#define _MSGID_le_pos_sample_Release 19
#define _MSGID_le_pos_SetAcquisitionRate 20
#define _MSGID_le_pos_GetAcquisitionRate 21
#define _MSGID_le_pos_SetDistanceResolution 22


// Define type-safe pack/unpack functions for all enums, including included types

static inline bool le_pos_PackFixState
(
    uint8_t **bufferPtr,
    le_pos_FixState_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_pos_UnpackFixState
(
    uint8_t **bufferPtr,
    le_pos_FixState_t* valuePtr
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

static inline bool le_pos_PackResolution
(
    uint8_t **bufferPtr,
    le_pos_Resolution_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_pos_UnpackResolution
(
    uint8_t **bufferPtr,
    le_pos_Resolution_t* valuePtr
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


#endif // LE_POS_MESSAGES_H_INCLUDE_GUARD