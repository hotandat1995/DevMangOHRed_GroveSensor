/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef IO_MESSAGES_H_INCLUDE_GUARD
#define IO_MESSAGES_H_INCLUDE_GUARD


#include "io_common.h"

#define _MAX_MSG_SIZE IFGEN_IO_MSG_SIZE

// Define the message type for communicating between client and server
typedef struct __attribute__((packed))
{
    uint32_t id;
    uint8_t buffer[_MAX_MSG_SIZE];
}
_Message_t;

#define _MSGID_io_CreateInput 0
#define _MSGID_io_SetJsonExample 1
#define _MSGID_io_CreateOutput 2
#define _MSGID_io_DeleteResource 3
#define _MSGID_io_PushTrigger 4
#define _MSGID_io_PushBoolean 5
#define _MSGID_io_PushNumeric 6
#define _MSGID_io_PushString 7
#define _MSGID_io_PushJson 8
#define _MSGID_io_AddTriggerPushHandler 9
#define _MSGID_io_RemoveTriggerPushHandler 10
#define _MSGID_io_AddBooleanPushHandler 11
#define _MSGID_io_RemoveBooleanPushHandler 12
#define _MSGID_io_AddNumericPushHandler 13
#define _MSGID_io_RemoveNumericPushHandler 14
#define _MSGID_io_AddStringPushHandler 15
#define _MSGID_io_RemoveStringPushHandler 16
#define _MSGID_io_AddJsonPushHandler 17
#define _MSGID_io_RemoveJsonPushHandler 18
#define _MSGID_io_MarkOptional 19
#define _MSGID_io_SetBooleanDefault 20
#define _MSGID_io_SetNumericDefault 21
#define _MSGID_io_SetStringDefault 22
#define _MSGID_io_SetJsonDefault 23
#define _MSGID_io_GetTimestamp 24
#define _MSGID_io_GetBoolean 25
#define _MSGID_io_GetNumeric 26
#define _MSGID_io_GetString 27
#define _MSGID_io_GetJson 28
#define _MSGID_io_AddUpdateStartEndHandler 29
#define _MSGID_io_RemoveUpdateStartEndHandler 30


// Define type-safe pack/unpack functions for all enums, including included types

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


#endif // IO_MESSAGES_H_INCLUDE_GUARD