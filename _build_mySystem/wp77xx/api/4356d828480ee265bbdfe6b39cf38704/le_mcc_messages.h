/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef LE_MCC_MESSAGES_H_INCLUDE_GUARD
#define LE_MCC_MESSAGES_H_INCLUDE_GUARD


#include "le_mcc_common.h"

#define _MAX_MSG_SIZE IFGEN_LE_MCC_MSG_SIZE

// Define the message type for communicating between client and server
typedef struct __attribute__((packed))
{
    uint32_t id;
    uint8_t buffer[_MAX_MSG_SIZE];
}
_Message_t;

#define _MSGID_le_mcc_Create 0
#define _MSGID_le_mcc_Delete 1
#define _MSGID_le_mcc_Start 2
#define _MSGID_le_mcc_IsConnected 3
#define _MSGID_le_mcc_GetRemoteTel 4
#define _MSGID_le_mcc_GetTerminationReason 5
#define _MSGID_le_mcc_GetPlatformSpecificTerminationCode 6
#define _MSGID_le_mcc_Answer 7
#define _MSGID_le_mcc_HangUp 8
#define _MSGID_le_mcc_HangUpAll 9
#define _MSGID_le_mcc_GetCallerIdRestrict 10
#define _MSGID_le_mcc_SetCallerIdRestrict 11
#define _MSGID_le_mcc_AddCallEventHandler 12
#define _MSGID_le_mcc_RemoveCallEventHandler 13
#define _MSGID_le_mcc_SetCallWaitingService 14
#define _MSGID_le_mcc_GetCallWaitingService 15
#define _MSGID_le_mcc_ActivateCall 16
#define _MSGID_le_mcc_SetAmrWbCapability 17
#define _MSGID_le_mcc_GetAmrWbCapability 18


// Define type-safe pack/unpack functions for all enums, including included types

static inline bool le_mcc_PackEvent
(
    uint8_t **bufferPtr,
    le_mcc_Event_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_mcc_UnpackEvent
(
    uint8_t **bufferPtr,
    le_mcc_Event_t* valuePtr
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

static inline bool le_mcc_PackTerminationReason
(
    uint8_t **bufferPtr,
    le_mcc_TerminationReason_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_mcc_UnpackTerminationReason
(
    uint8_t **bufferPtr,
    le_mcc_TerminationReason_t* valuePtr
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


#endif // LE_MCC_MESSAGES_H_INCLUDE_GUARD