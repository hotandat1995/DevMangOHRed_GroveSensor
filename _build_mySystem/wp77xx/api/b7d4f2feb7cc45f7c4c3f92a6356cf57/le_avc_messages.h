/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef LE_AVC_MESSAGES_H_INCLUDE_GUARD
#define LE_AVC_MESSAGES_H_INCLUDE_GUARD


#include "le_avc_common.h"

#define _MAX_MSG_SIZE IFGEN_LE_AVC_MSG_SIZE

// Define the message type for communicating between client and server
typedef struct __attribute__((packed))
{
    uint32_t id;
    uint8_t buffer[_MAX_MSG_SIZE];
}
_Message_t;

#define _MSGID_le_avc_AddStatusEventHandler 0
#define _MSGID_le_avc_RemoveStatusEventHandler 1
#define _MSGID_le_avc_AddSessionRequestEventHandler 2
#define _MSGID_le_avc_RemoveSessionRequestEventHandler 3
#define _MSGID_le_avc_AddCommInfoHandler 4
#define _MSGID_le_avc_RemoveCommInfoHandler 5
#define _MSGID_le_avc_StartSession 6
#define _MSGID_le_avc_StopSession 7
#define _MSGID_le_avc_CheckRoute 8
#define _MSGID_le_avc_DeferConnect 9
#define _MSGID_le_avc_AcceptDownload 10
#define _MSGID_le_avc_DeferDownload 11
#define _MSGID_le_avc_AcceptInstall 12
#define _MSGID_le_avc_DeferInstall 13
#define _MSGID_le_avc_AcceptUninstall 14
#define _MSGID_le_avc_DeferUninstall 15
#define _MSGID_le_avc_AcceptReboot 16
#define _MSGID_le_avc_DeferReboot 17
#define _MSGID_le_avc_GetUpdateType 18
#define _MSGID_le_avc_GetAppUpdateName 19
#define _MSGID_le_avc_BlockInstall 20
#define _MSGID_le_avc_UnblockInstall 21
#define _MSGID_le_avc_GetErrorCode 22
#define _MSGID_le_avc_GetSessionType 23
#define _MSGID_le_avc_GetHttpStatus 24
#define _MSGID_le_avc_GetPollingTimer 25
#define _MSGID_le_avc_GetRetryTimers 26
#define _MSGID_le_avc_GetApnConfig 27
#define _MSGID_le_avc_SetApnConfig 28
#define _MSGID_le_avc_SetPollingTimer 29
#define _MSGID_le_avc_SetRetryTimers 30
#define _MSGID_le_avc_GetCredentialStatus 31
#define _MSGID_le_avc_SetUserAgreement 32
#define _MSGID_le_avc_GetUserAgreement 33
#define _MSGID_le_avc_ReadLwm2mResource 34


// Define type-safe pack/unpack functions for all enums, including included types

static inline bool le_avc_PackStatus
(
    uint8_t **bufferPtr,
    le_avc_Status_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_avc_UnpackStatus
(
    uint8_t **bufferPtr,
    le_avc_Status_t* valuePtr
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

static inline bool le_avc_PackUserAgreement
(
    uint8_t **bufferPtr,
    le_avc_UserAgreement_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_avc_UnpackUserAgreement
(
    uint8_t **bufferPtr,
    le_avc_UserAgreement_t* valuePtr
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

static inline bool le_avc_PackSessionRequest
(
    uint8_t **bufferPtr,
    le_avc_SessionRequest_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_avc_UnpackSessionRequest
(
    uint8_t **bufferPtr,
    le_avc_SessionRequest_t* valuePtr
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

static inline bool le_avc_PackUpdateType
(
    uint8_t **bufferPtr,
    le_avc_UpdateType_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_avc_UnpackUpdateType
(
    uint8_t **bufferPtr,
    le_avc_UpdateType_t* valuePtr
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

static inline bool le_avc_PackErrorCode
(
    uint8_t **bufferPtr,
    le_avc_ErrorCode_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_avc_UnpackErrorCode
(
    uint8_t **bufferPtr,
    le_avc_ErrorCode_t* valuePtr
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

static inline bool le_avc_PackSessionType
(
    uint8_t **bufferPtr,
    le_avc_SessionType_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_avc_UnpackSessionType
(
    uint8_t **bufferPtr,
    le_avc_SessionType_t* valuePtr
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

static inline bool le_avc_PackCredentialStatus
(
    uint8_t **bufferPtr,
    le_avc_CredentialStatus_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_avc_UnpackCredentialStatus
(
    uint8_t **bufferPtr,
    le_avc_CredentialStatus_t* valuePtr
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


#endif // LE_AVC_MESSAGES_H_INCLUDE_GUARD