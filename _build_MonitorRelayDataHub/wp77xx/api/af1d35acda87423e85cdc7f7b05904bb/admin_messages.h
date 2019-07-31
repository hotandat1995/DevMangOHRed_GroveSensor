/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef ADMIN_MESSAGES_H_INCLUDE_GUARD
#define ADMIN_MESSAGES_H_INCLUDE_GUARD


#include "admin_common.h"

#define _MAX_MSG_SIZE IFGEN_ADMIN_MSG_SIZE

// Define the message type for communicating between client and server
typedef struct __attribute__((packed))
{
    uint32_t id;
    uint8_t buffer[_MAX_MSG_SIZE];
}
_Message_t;

#define _MSGID_admin_PushTrigger 0
#define _MSGID_admin_PushBoolean 1
#define _MSGID_admin_PushNumeric 2
#define _MSGID_admin_PushString 3
#define _MSGID_admin_PushJson 4
#define _MSGID_admin_AddTriggerPushHandler 5
#define _MSGID_admin_RemoveTriggerPushHandler 6
#define _MSGID_admin_AddBooleanPushHandler 7
#define _MSGID_admin_RemoveBooleanPushHandler 8
#define _MSGID_admin_AddNumericPushHandler 9
#define _MSGID_admin_RemoveNumericPushHandler 10
#define _MSGID_admin_AddStringPushHandler 11
#define _MSGID_admin_RemoveStringPushHandler 12
#define _MSGID_admin_AddJsonPushHandler 13
#define _MSGID_admin_RemoveJsonPushHandler 14
#define _MSGID_admin_SetSource 15
#define _MSGID_admin_GetSource 16
#define _MSGID_admin_RemoveSource 17
#define _MSGID_admin_CreateObs 18
#define _MSGID_admin_DeleteObs 19
#define _MSGID_admin_SetMinPeriod 20
#define _MSGID_admin_GetMinPeriod 21
#define _MSGID_admin_SetHighLimit 22
#define _MSGID_admin_GetHighLimit 23
#define _MSGID_admin_SetLowLimit 24
#define _MSGID_admin_GetLowLimit 25
#define _MSGID_admin_SetChangeBy 26
#define _MSGID_admin_GetChangeBy 27
#define _MSGID_admin_SetTransform 28
#define _MSGID_admin_GetTransform 29
#define _MSGID_admin_SetJsonExtraction 30
#define _MSGID_admin_GetJsonExtraction 31
#define _MSGID_admin_SetBufferMaxCount 32
#define _MSGID_admin_GetBufferMaxCount 33
#define _MSGID_admin_SetBufferBackupPeriod 34
#define _MSGID_admin_GetBufferBackupPeriod 35
#define _MSGID_admin_SetBooleanDefault 36
#define _MSGID_admin_SetNumericDefault 37
#define _MSGID_admin_SetStringDefault 38
#define _MSGID_admin_SetJsonDefault 39
#define _MSGID_admin_HasDefault 40
#define _MSGID_admin_GetDefaultDataType 41
#define _MSGID_admin_GetBooleanDefault 42
#define _MSGID_admin_GetNumericDefault 43
#define _MSGID_admin_GetStringDefault 44
#define _MSGID_admin_GetJsonDefault 45
#define _MSGID_admin_RemoveDefault 46
#define _MSGID_admin_SetBooleanOverride 47
#define _MSGID_admin_SetNumericOverride 48
#define _MSGID_admin_SetStringOverride 49
#define _MSGID_admin_SetJsonOverride 50
#define _MSGID_admin_HasOverride 51
#define _MSGID_admin_GetOverrideDataType 52
#define _MSGID_admin_GetBooleanOverride 53
#define _MSGID_admin_GetNumericOverride 54
#define _MSGID_admin_GetStringOverride 55
#define _MSGID_admin_GetJsonOverride 56
#define _MSGID_admin_RemoveOverride 57
#define _MSGID_admin_GetFirstChild 58
#define _MSGID_admin_GetNextSibling 59
#define _MSGID_admin_GetEntryType 60
#define _MSGID_admin_IsMandatory 61
#define _MSGID_admin_AddResourceTreeChangeHandler 62
#define _MSGID_admin_RemoveResourceTreeChangeHandler 63
#define _MSGID_admin_StartUpdate 64
#define _MSGID_admin_EndUpdate 65


// Define type-safe pack/unpack functions for all enums, including included types

static inline bool admin_PackEntryType
(
    uint8_t **bufferPtr,
    admin_EntryType_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool admin_UnpackEntryType
(
    uint8_t **bufferPtr,
    admin_EntryType_t* valuePtr
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

static inline bool admin_PackResourceOperationType
(
    uint8_t **bufferPtr,
    admin_ResourceOperationType_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool admin_UnpackResourceOperationType
(
    uint8_t **bufferPtr,
    admin_ResourceOperationType_t* valuePtr
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

static inline bool admin_PackTransformType
(
    uint8_t **bufferPtr,
    admin_TransformType_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool admin_UnpackTransformType
(
    uint8_t **bufferPtr,
    admin_TransformType_t* valuePtr
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

static inline bool io_PackDataType
(
    uint8_t **bufferPtr,
    io_DataType_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool io_UnpackDataType
(
    uint8_t **bufferPtr,
    io_DataType_t* valuePtr
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


#endif // ADMIN_MESSAGES_H_INCLUDE_GUARD