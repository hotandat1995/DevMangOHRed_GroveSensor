/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef LE_FLASH_MESSAGES_H_INCLUDE_GUARD
#define LE_FLASH_MESSAGES_H_INCLUDE_GUARD


#include "le_flash_common.h"

#define _MAX_MSG_SIZE IFGEN_LE_FLASH_MSG_SIZE

// Define the message type for communicating between client and server
typedef struct __attribute__((packed))
{
    uint32_t id;
    uint8_t buffer[_MAX_MSG_SIZE];
}
_Message_t;

#define _MSGID_le_flash_AddBadImageDetectionHandler 0
#define _MSGID_le_flash_RemoveBadImageDetectionHandler 1
#define _MSGID_le_flash_RequestAccess 2
#define _MSGID_le_flash_ReleaseAccess 3
#define _MSGID_le_flash_OpenMtd 4
#define _MSGID_le_flash_OpenUbi 5
#define _MSGID_le_flash_OpenUbiVolume 6
#define _MSGID_le_flash_CloseUbiVolume 7
#define _MSGID_le_flash_Close 8
#define _MSGID_le_flash_EraseBlock 9
#define _MSGID_le_flash_Read 10
#define _MSGID_le_flash_Write 11
#define _MSGID_le_flash_GetBlockInformation 12
#define _MSGID_le_flash_GetUbiVolumeInformation 13
#define _MSGID_le_flash_CreateUbi 14
#define _MSGID_le_flash_CreateUbiVolume 15
#define _MSGID_le_flash_DeleteUbiVolume 16


// Define type-safe pack/unpack functions for all enums, including included types

static inline bool le_flash_PackOpenMode
(
    uint8_t **bufferPtr,
    le_flash_OpenMode_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_flash_UnpackOpenMode
(
    uint8_t **bufferPtr,
    le_flash_OpenMode_t* valuePtr
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

static inline bool le_flash_PackUbiVolumeType
(
    uint8_t **bufferPtr,
    le_flash_UbiVolumeType_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_flash_UnpackUbiVolumeType
(
    uint8_t **bufferPtr,
    le_flash_UbiVolumeType_t* valuePtr
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


#endif // LE_FLASH_MESSAGES_H_INCLUDE_GUARD