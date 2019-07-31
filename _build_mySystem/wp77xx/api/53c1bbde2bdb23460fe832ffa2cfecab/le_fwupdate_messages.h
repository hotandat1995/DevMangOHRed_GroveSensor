/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef LE_FWUPDATE_MESSAGES_H_INCLUDE_GUARD
#define LE_FWUPDATE_MESSAGES_H_INCLUDE_GUARD


#include "le_fwupdate_common.h"

#define _MAX_MSG_SIZE IFGEN_LE_FWUPDATE_MSG_SIZE

// Define the message type for communicating between client and server
typedef struct __attribute__((packed))
{
    uint32_t id;
    uint8_t buffer[_MAX_MSG_SIZE];
}
_Message_t;

#define _MSGID_le_fwupdate_Download 0
#define _MSGID_le_fwupdate_InitDownload 1
#define _MSGID_le_fwupdate_GetResumePosition 2
#define _MSGID_le_fwupdate_GetUpdateStatus 3
#define _MSGID_le_fwupdate_GetFirmwareVersion 4
#define _MSGID_le_fwupdate_GetBootloaderVersion 5
#define _MSGID_le_fwupdate_GetAppBootloaderVersion 6
#define _MSGID_le_fwupdate_IsSystemMarkedGood 7
#define _MSGID_le_fwupdate_Install 8
#define _MSGID_le_fwupdate_MarkGood 9
#define _MSGID_le_fwupdate_InstallAndMarkGood 10


// Define type-safe pack/unpack functions for all enums, including included types

static inline bool le_fwupdate_PackUpdateStatus
(
    uint8_t **bufferPtr,
    le_fwupdate_UpdateStatus_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_fwupdate_UnpackUpdateStatus
(
    uint8_t **bufferPtr,
    le_fwupdate_UpdateStatus_t* valuePtr
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


#endif // LE_FWUPDATE_MESSAGES_H_INCLUDE_GUARD