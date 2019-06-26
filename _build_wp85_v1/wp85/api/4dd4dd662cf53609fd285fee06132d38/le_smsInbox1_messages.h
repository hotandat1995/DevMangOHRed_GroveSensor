/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef LE_SMSINBOX1_MESSAGES_H_INCLUDE_GUARD
#define LE_SMSINBOX1_MESSAGES_H_INCLUDE_GUARD


#include "le_smsInbox1_common.h"

#define _MAX_MSG_SIZE IFGEN_LE_SMSINBOX1_MSG_SIZE

// Define the message type for communicating between client and server
typedef struct __attribute__((packed))
{
    uint32_t id;
    uint8_t buffer[_MAX_MSG_SIZE];
}
_Message_t;

#define _MSGID_le_smsInbox1_Open 0
#define _MSGID_le_smsInbox1_Close 1
#define _MSGID_le_smsInbox1_AddRxMessageHandler 2
#define _MSGID_le_smsInbox1_RemoveRxMessageHandler 3
#define _MSGID_le_smsInbox1_DeleteMsg 4
#define _MSGID_le_smsInbox1_GetImsi 5
#define _MSGID_le_smsInbox1_GetFormat 6
#define _MSGID_le_smsInbox1_GetSenderTel 7
#define _MSGID_le_smsInbox1_GetTimeStamp 8
#define _MSGID_le_smsInbox1_GetMsgLen 9
#define _MSGID_le_smsInbox1_GetText 10
#define _MSGID_le_smsInbox1_GetBinary 11
#define _MSGID_le_smsInbox1_GetPdu 12
#define _MSGID_le_smsInbox1_GetFirst 13
#define _MSGID_le_smsInbox1_GetNext 14
#define _MSGID_le_smsInbox1_IsUnread 15
#define _MSGID_le_smsInbox1_MarkRead 16
#define _MSGID_le_smsInbox1_MarkUnread 17
#define _MSGID_le_smsInbox1_SetMaxMessages 18
#define _MSGID_le_smsInbox1_GetMaxMessages 19


// Define type-safe pack/unpack functions for all enums, including included types

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

static inline bool le_sim_PackStates
(
    uint8_t **bufferPtr,
    le_sim_States_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_sim_UnpackStates
(
    uint8_t **bufferPtr,
    le_sim_States_t* valuePtr
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

static inline bool le_sim_PackId
(
    uint8_t **bufferPtr,
    le_sim_Id_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_sim_UnpackId
(
    uint8_t **bufferPtr,
    le_sim_Id_t* valuePtr
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

static inline bool le_sim_PackManufacturer
(
    uint8_t **bufferPtr,
    le_sim_Manufacturer_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_sim_UnpackManufacturer
(
    uint8_t **bufferPtr,
    le_sim_Manufacturer_t* valuePtr
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

static inline bool le_sim_PackCommand
(
    uint8_t **bufferPtr,
    le_sim_Command_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_sim_UnpackCommand
(
    uint8_t **bufferPtr,
    le_sim_Command_t* valuePtr
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

static inline bool le_sim_PackStkEvent
(
    uint8_t **bufferPtr,
    le_sim_StkEvent_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_sim_UnpackStkEvent
(
    uint8_t **bufferPtr,
    le_sim_StkEvent_t* valuePtr
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

static inline bool le_sim_PackStkRefreshMode
(
    uint8_t **bufferPtr,
    le_sim_StkRefreshMode_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_sim_UnpackStkRefreshMode
(
    uint8_t **bufferPtr,
    le_sim_StkRefreshMode_t* valuePtr
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

static inline bool le_sim_PackStkRefreshStage
(
    uint8_t **bufferPtr,
    le_sim_StkRefreshStage_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_sim_UnpackStkRefreshStage
(
    uint8_t **bufferPtr,
    le_sim_StkRefreshStage_t* valuePtr
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

static inline bool le_sms_PackFormat
(
    uint8_t **bufferPtr,
    le_sms_Format_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_sms_UnpackFormat
(
    uint8_t **bufferPtr,
    le_sms_Format_t* valuePtr
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

static inline bool le_sms_PackType
(
    uint8_t **bufferPtr,
    le_sms_Type_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_sms_UnpackType
(
    uint8_t **bufferPtr,
    le_sms_Type_t* valuePtr
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

static inline bool le_sms_PackStatus
(
    uint8_t **bufferPtr,
    le_sms_Status_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_sms_UnpackStatus
(
    uint8_t **bufferPtr,
    le_sms_Status_t* valuePtr
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

static inline bool le_sms_PackLanguages
(
    uint8_t **bufferPtr,
    le_sms_Languages_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_sms_UnpackLanguages
(
    uint8_t **bufferPtr,
    le_sms_Languages_t* valuePtr
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

static inline bool le_sms_PackCdmaServiceCat
(
    uint8_t **bufferPtr,
    le_sms_CdmaServiceCat_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_sms_UnpackCdmaServiceCat
(
    uint8_t **bufferPtr,
    le_sms_CdmaServiceCat_t* valuePtr
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

static inline bool le_sms_PackStorage
(
    uint8_t **bufferPtr,
    le_sms_Storage_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_sms_UnpackStorage
(
    uint8_t **bufferPtr,
    le_sms_Storage_t* valuePtr
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

static inline bool le_sms_PackErrorCode3GPP2
(
    uint8_t **bufferPtr,
    le_sms_ErrorCode3GPP2_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_sms_UnpackErrorCode3GPP2
(
    uint8_t **bufferPtr,
    le_sms_ErrorCode3GPP2_t* valuePtr
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

static inline bool le_sms_PackErrorCode
(
    uint8_t **bufferPtr,
    le_sms_ErrorCode_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_sms_UnpackErrorCode
(
    uint8_t **bufferPtr,
    le_sms_ErrorCode_t* valuePtr
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


#endif // LE_SMSINBOX1_MESSAGES_H_INCLUDE_GUARD