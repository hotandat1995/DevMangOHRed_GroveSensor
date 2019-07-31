/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef LE_TEMP_MESSAGES_H_INCLUDE_GUARD
#define LE_TEMP_MESSAGES_H_INCLUDE_GUARD


#include "le_temp_common.h"

#define _MAX_MSG_SIZE IFGEN_LE_TEMP_MSG_SIZE

// Define the message type for communicating between client and server
typedef struct __attribute__((packed))
{
    uint32_t id;
    uint8_t buffer[_MAX_MSG_SIZE];
}
_Message_t;

#define _MSGID_le_temp_Request 0
#define _MSGID_le_temp_GetSensorName 1
#define _MSGID_le_temp_AddThresholdEventHandler 2
#define _MSGID_le_temp_RemoveThresholdEventHandler 3
#define _MSGID_le_temp_GetTemperature 4
#define _MSGID_le_temp_SetThreshold 5
#define _MSGID_le_temp_GetThreshold 6
#define _MSGID_le_temp_StartMonitoring 7


// Define type-safe pack/unpack functions for all enums, including included types

// Define pack/unpack functions for all structures, including included types


#endif // LE_TEMP_MESSAGES_H_INCLUDE_GUARD