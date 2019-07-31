/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef LE_IPS_MESSAGES_H_INCLUDE_GUARD
#define LE_IPS_MESSAGES_H_INCLUDE_GUARD


#include "le_ips_common.h"

#define _MAX_MSG_SIZE IFGEN_LE_IPS_MSG_SIZE

// Define the message type for communicating between client and server
typedef struct __attribute__((packed))
{
    uint32_t id;
    uint8_t buffer[_MAX_MSG_SIZE];
}
_Message_t;

#define _MSGID_le_ips_AddThresholdEventHandler 0
#define _MSGID_le_ips_RemoveThresholdEventHandler 1
#define _MSGID_le_ips_GetInputVoltage 2
#define _MSGID_le_ips_SetVoltageThresholds 3
#define _MSGID_le_ips_GetVoltageThresholds 4
#define _MSGID_le_ips_GetPowerSource 5
#define _MSGID_le_ips_GetBatteryLevel 6
#define _MSGID_le_ips_SetBatteryLevel 7


// Define type-safe pack/unpack functions for all enums, including included types

static inline bool le_ips_PackThresholdStatus
(
    uint8_t **bufferPtr,
    le_ips_ThresholdStatus_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_ips_UnpackThresholdStatus
(
    uint8_t **bufferPtr,
    le_ips_ThresholdStatus_t* valuePtr
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

static inline bool le_ips_PackPowerSource
(
    uint8_t **bufferPtr,
    le_ips_PowerSource_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_ips_UnpackPowerSource
(
    uint8_t **bufferPtr,
    le_ips_PowerSource_t* valuePtr
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


#endif // LE_IPS_MESSAGES_H_INCLUDE_GUARD