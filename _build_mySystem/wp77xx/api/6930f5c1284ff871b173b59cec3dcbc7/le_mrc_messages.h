/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef LE_MRC_MESSAGES_H_INCLUDE_GUARD
#define LE_MRC_MESSAGES_H_INCLUDE_GUARD


#include "le_mrc_common.h"

#define _MAX_MSG_SIZE IFGEN_LE_MRC_MSG_SIZE

// Define the message type for communicating between client and server
typedef struct __attribute__((packed))
{
    uint32_t id;
    uint8_t buffer[_MAX_MSG_SIZE];
}
_Message_t;

#define _MSGID_le_mrc_AddNetRegStateEventHandler 0
#define _MSGID_le_mrc_RemoveNetRegStateEventHandler 1
#define _MSGID_le_mrc_AddRatChangeHandler 2
#define _MSGID_le_mrc_RemoveRatChangeHandler 3
#define _MSGID_le_mrc_AddPacketSwitchedChangeHandler 4
#define _MSGID_le_mrc_RemovePacketSwitchedChangeHandler 5
#define _MSGID_le_mrc_AddSignalStrengthChangeHandler 6
#define _MSGID_le_mrc_RemoveSignalStrengthChangeHandler 7
#define _MSGID_le_mrc_SetSignalStrengthIndThresholds 8
#define _MSGID_le_mrc_SetSignalStrengthIndDelta 9
#define _MSGID_le_mrc_SetAutomaticRegisterMode 10
#define _MSGID_le_mrc_SetManualRegisterMode 11
#define _MSGID_le_mrc_SetManualRegisterModeAsync 12
#define _MSGID_le_mrc_GetRegisterMode 13
#define _MSGID_le_mrc_GetPacketSwitchedState 14
#define _MSGID_le_mrc_GetPlatformSpecificRegistrationErrorCode 15
#define _MSGID_le_mrc_SetRatPreferences 16
#define _MSGID_le_mrc_GetRatPreferences 17
#define _MSGID_le_mrc_SetBandPreferences 18
#define _MSGID_le_mrc_GetBandPreferences 19
#define _MSGID_le_mrc_SetLteBandPreferences 20
#define _MSGID_le_mrc_GetLteBandPreferences 21
#define _MSGID_le_mrc_SetTdScdmaBandPreferences 22
#define _MSGID_le_mrc_GetTdScdmaBandPreferences 23
#define _MSGID_le_mrc_AddPreferredOperator 24
#define _MSGID_le_mrc_RemovePreferredOperator 25
#define _MSGID_le_mrc_GetPreferredOperatorsList 26
#define _MSGID_le_mrc_GetFirstPreferredOperator 27
#define _MSGID_le_mrc_GetNextPreferredOperator 28
#define _MSGID_le_mrc_DeletePreferredOperatorsList 29
#define _MSGID_le_mrc_GetPreferredOperatorDetails 30
#define _MSGID_le_mrc_GetNetRegState 31
#define _MSGID_le_mrc_GetSignalQual 32
#define _MSGID_le_mrc_SetRadioPower 33
#define _MSGID_le_mrc_GetRadioPower 34
#define _MSGID_le_mrc_PerformCellularNetworkScan 35
#define _MSGID_le_mrc_PerformPciNetworkScan 36
#define _MSGID_le_mrc_PerformCellularNetworkScanAsync 37
#define _MSGID_le_mrc_PerformPciNetworkScanAsync 38
#define _MSGID_le_mrc_GetFirstCellularNetworkScan 39
#define _MSGID_le_mrc_GetNextCellularNetworkScan 40
#define _MSGID_le_mrc_GetFirstPciScanInfo 41
#define _MSGID_le_mrc_GetNextPciScanInfo 42
#define _MSGID_le_mrc_GetFirstPlmnInfo 43
#define _MSGID_le_mrc_GetNextPlmnInfo 44
#define _MSGID_le_mrc_GetPciScanCellId 45
#define _MSGID_le_mrc_GetPciScanGlobalCellId 46
#define _MSGID_le_mrc_GetPciScanMccMnc 47
#define _MSGID_le_mrc_DeleteCellularNetworkScan 48
#define _MSGID_le_mrc_DeletePciNetworkScan 49
#define _MSGID_le_mrc_GetCellularNetworkMccMnc 50
#define _MSGID_le_mrc_GetCellularNetworkName 51
#define _MSGID_le_mrc_GetCellularNetworkRat 52
#define _MSGID_le_mrc_IsCellularNetworkInUse 53
#define _MSGID_le_mrc_IsCellularNetworkAvailable 54
#define _MSGID_le_mrc_IsCellularNetworkHome 55
#define _MSGID_le_mrc_IsCellularNetworkForbidden 56
#define _MSGID_le_mrc_GetCurrentNetworkName 57
#define _MSGID_le_mrc_GetCurrentNetworkMccMnc 58
#define _MSGID_le_mrc_GetRadioAccessTechInUse 59
#define _MSGID_le_mrc_GetNeighborCellsInfo 60
#define _MSGID_le_mrc_DeleteNeighborCellsInfo 61
#define _MSGID_le_mrc_GetFirstNeighborCellInfo 62
#define _MSGID_le_mrc_GetNextNeighborCellInfo 63
#define _MSGID_le_mrc_GetNeighborCellId 64
#define _MSGID_le_mrc_GetNeighborCellLocAreaCode 65
#define _MSGID_le_mrc_GetNeighborCellRxLevel 66
#define _MSGID_le_mrc_GetNeighborCellRat 67
#define _MSGID_le_mrc_GetNeighborCellUmtsEcIo 68
#define _MSGID_le_mrc_GetNeighborCellLteIntraFreq 69
#define _MSGID_le_mrc_GetNeighborCellLteInterFreq 70
#define _MSGID_le_mrc_MeasureSignalMetrics 71
#define _MSGID_le_mrc_DeleteSignalMetrics 72
#define _MSGID_le_mrc_GetRatOfSignalMetrics 73
#define _MSGID_le_mrc_GetGsmSignalMetrics 74
#define _MSGID_le_mrc_GetUmtsSignalMetrics 75
#define _MSGID_le_mrc_GetLteSignalMetrics 76
#define _MSGID_le_mrc_GetCdmaSignalMetrics 77
#define _MSGID_le_mrc_GetServingCellId 78
#define _MSGID_le_mrc_GetServingCellLocAreaCode 79
#define _MSGID_le_mrc_GetServingCellLteTracAreaCode 80
#define _MSGID_le_mrc_GetBandCapabilities 81
#define _MSGID_le_mrc_GetLteBandCapabilities 82
#define _MSGID_le_mrc_GetTdScdmaBandCapabilities 83
#define _MSGID_le_mrc_AddNetworkRejectHandler 84
#define _MSGID_le_mrc_RemoveNetworkRejectHandler 85
#define _MSGID_le_mrc_AddNetRegRejectHandler 86
#define _MSGID_le_mrc_RemoveNetRegRejectHandler 87
#define _MSGID_le_mrc_AddJammingDetectionEventHandler 88
#define _MSGID_le_mrc_RemoveJammingDetectionEventHandler 89
#define _MSGID_le_mrc_StartJammingDetection 90
#define _MSGID_le_mrc_StopJammingDetection 91
#define _MSGID_le_mrc_SetSarBackoffState 92
#define _MSGID_le_mrc_GetSarBackoffState 93


// Define type-safe pack/unpack functions for all enums, including included types

static inline bool le_mrc_PackBandBitMask
(
    uint8_t **bufferPtr,
    le_mrc_BandBitMask_t value
)
{
    return le_pack_PackUint64(bufferPtr, value);
}

static inline bool le_mrc_UnpackBandBitMask
(
    uint8_t **bufferPtr,
    le_mrc_BandBitMask_t* valuePtr
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

static inline bool le_mrc_PackLteBandBitMask
(
    uint8_t **bufferPtr,
    le_mrc_LteBandBitMask_t value
)
{
    return le_pack_PackUint64(bufferPtr, value);
}

static inline bool le_mrc_UnpackLteBandBitMask
(
    uint8_t **bufferPtr,
    le_mrc_LteBandBitMask_t* valuePtr
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

static inline bool le_mrc_PackTdScdmaBandBitMask
(
    uint8_t **bufferPtr,
    le_mrc_TdScdmaBandBitMask_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_mrc_UnpackTdScdmaBandBitMask
(
    uint8_t **bufferPtr,
    le_mrc_TdScdmaBandBitMask_t* valuePtr
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

static inline bool le_mrc_PackNetRegState
(
    uint8_t **bufferPtr,
    le_mrc_NetRegState_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_mrc_UnpackNetRegState
(
    uint8_t **bufferPtr,
    le_mrc_NetRegState_t* valuePtr
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

static inline bool le_mrc_PackRat
(
    uint8_t **bufferPtr,
    le_mrc_Rat_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_mrc_UnpackRat
(
    uint8_t **bufferPtr,
    le_mrc_Rat_t* valuePtr
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

static inline bool le_mrc_PackJammingReport
(
    uint8_t **bufferPtr,
    le_mrc_JammingReport_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_mrc_UnpackJammingReport
(
    uint8_t **bufferPtr,
    le_mrc_JammingReport_t* valuePtr
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

static inline bool le_mrc_PackJammingStatus
(
    uint8_t **bufferPtr,
    le_mrc_JammingStatus_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_mrc_UnpackJammingStatus
(
    uint8_t **bufferPtr,
    le_mrc_JammingStatus_t* valuePtr
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

static inline bool le_mrc_PackRatBitMask
(
    uint8_t **bufferPtr,
    le_mrc_RatBitMask_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_mrc_UnpackRatBitMask
(
    uint8_t **bufferPtr,
    le_mrc_RatBitMask_t* valuePtr
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

static inline bool le_mrc_PackNetRejCause
(
    uint8_t **bufferPtr,
    le_mrc_NetRejCause_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_mrc_UnpackNetRejCause
(
    uint8_t **bufferPtr,
    le_mrc_NetRejCause_t* valuePtr
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

static inline bool le_mrc_PackNetRegDomain
(
    uint8_t **bufferPtr,
    le_mrc_NetRegDomain_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_mrc_UnpackNetRegDomain
(
    uint8_t **bufferPtr,
    le_mrc_NetRegDomain_t* valuePtr
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


static inline bool le_mrc_PackNetRegRejectInd
(
    uint8_t **bufferPtr,
    const le_mrc_NetRegRejectInd_t *valuePtr
)
{
    __attribute__((unused))
    bool subResult, result = true;

    LE_ASSERT(valuePtr);
    subResult = le_mrc_PackRat( bufferPtr,
                                                 valuePtr->rat );
    result = result && subResult;
    subResult = le_pack_PackString( bufferPtr,
                                    valuePtr->mcc, 4);
    result = result && subResult;
    subResult = le_pack_PackString( bufferPtr,
                                    valuePtr->mnc, 4);
    result = result && subResult;
    subResult = le_mrc_PackNetRejCause( bufferPtr,
                                                 valuePtr->cause );
    result = result && subResult;
    subResult = le_mrc_PackNetRegDomain( bufferPtr,
                                                 valuePtr->rejDomain );
    result = result && subResult;

    return result;
}

static inline bool le_mrc_UnpackNetRegRejectInd
(
    uint8_t **bufferPtr,
    le_mrc_NetRegRejectInd_t *valuePtr
)
{
    bool result = true;
    if (result)
    {
        result = le_mrc_UnpackRat(bufferPtr,
                                                   &valuePtr->rat );
    }
    if (result)
    {
        result = le_pack_UnpackString(bufferPtr,
                                      valuePtr->mcc,
                                      sizeof(valuePtr->mcc),
                                      4);
    }
    if (result)
    {
        result = le_pack_UnpackString(bufferPtr,
                                      valuePtr->mnc,
                                      sizeof(valuePtr->mnc),
                                      4);
    }
    if (result)
    {
        result = le_mrc_UnpackNetRejCause(bufferPtr,
                                                   &valuePtr->cause );
    }
    if (result)
    {
        result = le_mrc_UnpackNetRegDomain(bufferPtr,
                                                   &valuePtr->rejDomain );
    }
    return result;
}

#endif // LE_MRC_MESSAGES_H_INCLUDE_GUARD