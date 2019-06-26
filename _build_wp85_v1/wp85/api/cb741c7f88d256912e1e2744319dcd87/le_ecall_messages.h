/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef LE_ECALL_MESSAGES_H_INCLUDE_GUARD
#define LE_ECALL_MESSAGES_H_INCLUDE_GUARD


#include "le_ecall_common.h"

#define _MAX_MSG_SIZE IFGEN_LE_ECALL_MSG_SIZE

// Define the message type for communicating between client and server
typedef struct __attribute__((packed))
{
    uint32_t id;
    uint8_t buffer[_MAX_MSG_SIZE];
}
_Message_t;

#define _MSGID_le_ecall_ForceOnlyMode 0
#define _MSGID_le_ecall_ForcePersistentOnlyMode 1
#define _MSGID_le_ecall_ExitOnlyMode 2
#define _MSGID_le_ecall_GetConfiguredOperationMode 3
#define _MSGID_le_ecall_AddStateChangeHandler 4
#define _MSGID_le_ecall_RemoveStateChangeHandler 5
#define _MSGID_le_ecall_Create 6
#define _MSGID_le_ecall_Delete 7
#define _MSGID_le_ecall_SetSystemStandard 8
#define _MSGID_le_ecall_GetSystemStandard 9
#define _MSGID_le_ecall_SetMsdVersion 10
#define _MSGID_le_ecall_GetMsdVersion 11
#define _MSGID_le_ecall_SetVehicleType 12
#define _MSGID_le_ecall_GetVehicleType 13
#define _MSGID_le_ecall_SetVIN 14
#define _MSGID_le_ecall_GetVIN 15
#define _MSGID_le_ecall_SetPropulsionType 16
#define _MSGID_le_ecall_GetPropulsionType 17
#define _MSGID_le_ecall_SetMsdTxMode 18
#define _MSGID_le_ecall_GetMsdTxMode 19
#define _MSGID_le_ecall_SetMsdPosition 20
#define _MSGID_le_ecall_SetMsdPositionN1 21
#define _MSGID_le_ecall_SetMsdPositionN2 22
#define _MSGID_le_ecall_SetMsdPassengersCount 23
#define _MSGID_le_ecall_ImportMsd 24
#define _MSGID_le_ecall_ExportMsd 25
#define _MSGID_le_ecall_SendMsd 26
#define _MSGID_le_ecall_StartAutomatic 27
#define _MSGID_le_ecall_StartManual 28
#define _MSGID_le_ecall_StartTest 29
#define _MSGID_le_ecall_End 30
#define _MSGID_le_ecall_GetState 31
#define _MSGID_le_ecall_SetPsapNumber 32
#define _MSGID_le_ecall_GetPsapNumber 33
#define _MSGID_le_ecall_UseUSimNumbers 34
#define _MSGID_le_ecall_SetNadDeregistrationTime 35
#define _MSGID_le_ecall_GetNadDeregistrationTime 36
#define _MSGID_le_ecall_SetIntervalBetweenDialAttempts 37
#define _MSGID_le_ecall_GetIntervalBetweenDialAttempts 38
#define _MSGID_le_ecall_SetEraGlonassManualDialAttempts 39
#define _MSGID_le_ecall_SetEraGlonassAutoDialAttempts 40
#define _MSGID_le_ecall_SetEraGlonassDialDuration 41
#define _MSGID_le_ecall_SetEraGlonassFallbackTime 42
#define _MSGID_le_ecall_SetEraGlonassAutoAnswerTime 43
#define _MSGID_le_ecall_SetEraGlonassMSDMaxTransmissionTime 44
#define _MSGID_le_ecall_SetEraGlonassPostTestRegistrationTime 45
#define _MSGID_le_ecall_GetEraGlonassManualDialAttempts 46
#define _MSGID_le_ecall_GetEraGlonassAutoDialAttempts 47
#define _MSGID_le_ecall_GetEraGlonassDialDuration 48
#define _MSGID_le_ecall_GetEraGlonassFallbackTime 49
#define _MSGID_le_ecall_GetEraGlonassAutoAnswerTime 50
#define _MSGID_le_ecall_GetEraGlonassMSDMaxTransmissionTime 51
#define _MSGID_le_ecall_GetEraGlonassPostTestRegistrationTime 52
#define _MSGID_le_ecall_SetMsdEraGlonassCrashSeverity 53
#define _MSGID_le_ecall_ResetMsdEraGlonassCrashSeverity 54
#define _MSGID_le_ecall_SetMsdEraGlonassDiagnosticResult 55
#define _MSGID_le_ecall_ResetMsdEraGlonassDiagnosticResult 56
#define _MSGID_le_ecall_SetMsdEraGlonassCrashInfo 57
#define _MSGID_le_ecall_ResetMsdEraGlonassCrashInfo 58
#define _MSGID_le_ecall_SetMsdEraGlonassCoordinateSystemType 59
#define _MSGID_le_ecall_ResetMsdEraGlonassCoordinateSystemType 60
#define _MSGID_le_ecall_GetTerminationReason 61
#define _MSGID_le_ecall_GetPlatformSpecificTerminationCode 62


// Define type-safe pack/unpack functions for all enums, including included types

static inline bool le_ecall_PackOpMode
(
    uint8_t **bufferPtr,
    le_ecall_OpMode_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_ecall_UnpackOpMode
(
    uint8_t **bufferPtr,
    le_ecall_OpMode_t* valuePtr
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

static inline bool le_ecall_PackSystemStandard
(
    uint8_t **bufferPtr,
    le_ecall_SystemStandard_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_ecall_UnpackSystemStandard
(
    uint8_t **bufferPtr,
    le_ecall_SystemStandard_t* valuePtr
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

static inline bool le_ecall_PackState
(
    uint8_t **bufferPtr,
    le_ecall_State_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_ecall_UnpackState
(
    uint8_t **bufferPtr,
    le_ecall_State_t* valuePtr
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

static inline bool le_ecall_PackMsdTxMode
(
    uint8_t **bufferPtr,
    le_ecall_MsdTxMode_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_ecall_UnpackMsdTxMode
(
    uint8_t **bufferPtr,
    le_ecall_MsdTxMode_t* valuePtr
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

static inline bool le_ecall_PackMsdVehicleType
(
    uint8_t **bufferPtr,
    le_ecall_MsdVehicleType_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_ecall_UnpackMsdVehicleType
(
    uint8_t **bufferPtr,
    le_ecall_MsdVehicleType_t* valuePtr
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

static inline bool le_ecall_PackPropulsionTypeBitMask
(
    uint8_t **bufferPtr,
    le_ecall_PropulsionTypeBitMask_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_ecall_UnpackPropulsionTypeBitMask
(
    uint8_t **bufferPtr,
    le_ecall_PropulsionTypeBitMask_t* valuePtr
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

static inline bool le_ecall_PackDiagnosticResultBitMask
(
    uint8_t **bufferPtr,
    le_ecall_DiagnosticResultBitMask_t value
)
{
    return le_pack_PackUint64(bufferPtr, value);
}

static inline bool le_ecall_UnpackDiagnosticResultBitMask
(
    uint8_t **bufferPtr,
    le_ecall_DiagnosticResultBitMask_t* valuePtr
)
{
    bool result;
    uint64_t value = 0;
    result = le_pack_UnpackUint64(bufferPtr, &value);
    if (result)
    {
        *valuePtr = value;
    }
    return result;
}

static inline bool le_ecall_PackCrashInfoBitMask
(
    uint8_t **bufferPtr,
    le_ecall_CrashInfoBitMask_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_ecall_UnpackCrashInfoBitMask
(
    uint8_t **bufferPtr,
    le_ecall_CrashInfoBitMask_t* valuePtr
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

static inline bool le_ecall_PackMsdCoordinateType
(
    uint8_t **bufferPtr,
    le_ecall_MsdCoordinateType_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_ecall_UnpackMsdCoordinateType
(
    uint8_t **bufferPtr,
    le_ecall_MsdCoordinateType_t* valuePtr
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

static inline bool le_mcc_PackEvent
(
    uint8_t **bufferPtr,
    le_mcc_Event_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_mcc_UnpackEvent
(
    uint8_t **bufferPtr,
    le_mcc_Event_t* valuePtr
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

static inline bool le_mcc_PackTerminationReason
(
    uint8_t **bufferPtr,
    le_mcc_TerminationReason_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_mcc_UnpackTerminationReason
(
    uint8_t **bufferPtr,
    le_mcc_TerminationReason_t* valuePtr
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


#endif // LE_ECALL_MESSAGES_H_INCLUDE_GUARD