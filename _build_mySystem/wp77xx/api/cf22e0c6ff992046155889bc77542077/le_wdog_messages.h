/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef LE_WDOG_MESSAGES_H_INCLUDE_GUARD
#define LE_WDOG_MESSAGES_H_INCLUDE_GUARD


#include "le_wdog_common.h"

#define _MAX_MSG_SIZE IFGEN_LE_WDOG_MSG_SIZE

// Define the message type for communicating between client and server
typedef struct __attribute__((packed))
{
    uint32_t id;
    uint8_t buffer[_MAX_MSG_SIZE];
}
_Message_t;

#define _MSGID_le_wdog_Kick 0
#define _MSGID_le_wdog_Timeout 1
#define _MSGID_le_wdog_GetWatchdogTimeout 2
#define _MSGID_le_wdog_GetMaxWatchdogTimeout 3


// Define type-safe pack/unpack functions for all enums, including included types

// Define pack/unpack functions for all structures, including included types


#endif // LE_WDOG_MESSAGES_H_INCLUDE_GUARD