/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef LE_FRAMEWORK_MESSAGES_H_INCLUDE_GUARD
#define LE_FRAMEWORK_MESSAGES_H_INCLUDE_GUARD


#include "le_framework_common.h"

#define _MAX_MSG_SIZE IFGEN_LE_FRAMEWORK_MSG_SIZE

// Define the message type for communicating between client and server
typedef struct __attribute__((packed))
{
    uint32_t id;
    uint8_t buffer[_MAX_MSG_SIZE];
}
_Message_t;

#define _MSGID_le_framework_Stop 0
#define _MSGID_le_framework_Restart 1
#define _MSGID_le_framework_IsStopping 2
#define _MSGID_le_framework_IsReadOnly 3
#define _MSGID_le_framework_NotifyExpectedReboot 4


// Define type-safe pack/unpack functions for all enums, including included types

// Define pack/unpack functions for all structures, including included types


#endif // LE_FRAMEWORK_MESSAGES_H_INCLUDE_GUARD