/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef LE_SIM_MESSAGES_H_INCLUDE_GUARD
#define LE_SIM_MESSAGES_H_INCLUDE_GUARD


#include "le_sim_common.h"

#define _MAX_MSG_SIZE IFGEN_LE_SIM_MSG_SIZE

// Define the message type for communicating between client and server
typedef struct __attribute__((packed))
{
    uint32_t id;
    uint8_t buffer[_MAX_MSG_SIZE];
}
_Message_t;

#define _MSGID_le_sim_AddNewStateHandler 0
#define _MSGID_le_sim_RemoveNewStateHandler 1
#define _MSGID_le_sim_AddProfileUpdateHandler 2
#define _MSGID_le_sim_RemoveProfileUpdateHandler 3
#define _MSGID_le_sim_AddSimToolkitEventHandler 4
#define _MSGID_le_sim_RemoveSimToolkitEventHandler 5
#define _MSGID_le_sim_AddIccidChangeHandler 6
#define _MSGID_le_sim_RemoveIccidChangeHandler 7
#define _MSGID_le_sim_GetSelectedCard 8
#define _MSGID_le_sim_SelectCard 9
#define _MSGID_le_sim_GetICCID 10
#define _MSGID_le_sim_GetEID 11
#define _MSGID_le_sim_GetIMSI 12
#define _MSGID_le_sim_IsPresent 13
#define _MSGID_le_sim_IsReady 14
#define _MSGID_le_sim_EnterPIN 15
#define _MSGID_le_sim_ChangePIN 16
#define _MSGID_le_sim_GetRemainingPINTries 17
#define _MSGID_le_sim_GetRemainingPUKTries 18
#define _MSGID_le_sim_Unlock 19
#define _MSGID_le_sim_Lock 20
#define _MSGID_le_sim_Unblock 21
#define _MSGID_le_sim_GetState 22
#define _MSGID_le_sim_GetSubscriberPhoneNumber 23
#define _MSGID_le_sim_GetHomeNetworkOperator 24
#define _MSGID_le_sim_GetHomeNetworkMccMnc 25
#define _MSGID_le_sim_LocalSwapToEmergencyCallSubscription 26
#define _MSGID_le_sim_LocalSwapToCommercialSubscription 27
#define _MSGID_le_sim_IsEmergencyCallSubscriptionSelected 28
#define _MSGID_le_sim_AcceptSimToolkitCommand 29
#define _MSGID_le_sim_RejectSimToolkitCommand 30
#define _MSGID_le_sim_GetSimToolkitRefreshMode 31
#define _MSGID_le_sim_GetSimToolkitRefreshStage 32
#define _MSGID_le_sim_SendApdu 33
#define _MSGID_le_sim_SendCommand 34
#define _MSGID_le_sim_SetAutomaticSelection 35
#define _MSGID_le_sim_GetAutomaticSelection 36
#define _MSGID_le_sim_Reset 37
#define _MSGID_le_sim_CreateFPLMNList 38
#define _MSGID_le_sim_AddFPLMNOperator 39
#define _MSGID_le_sim_WriteFPLMNList 40
#define _MSGID_le_sim_ReadFPLMNList 41
#define _MSGID_le_sim_GetFirstFPLMNOperator 42
#define _MSGID_le_sim_GetNextFPLMNOperator 43
#define _MSGID_le_sim_DeleteFPLMNList 44
#define _MSGID_le_sim_OpenLogicalChannel 45
#define _MSGID_le_sim_CloseLogicalChannel 46
#define _MSGID_le_sim_SendApduOnChannel 47
#define _MSGID_le_sim_SetPower 48


// Define type-safe pack/unpack functions for all enums, including included types

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

// Define pack/unpack functions for all structures, including included types


#endif // LE_SIM_MESSAGES_H_INCLUDE_GUARD