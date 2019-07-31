/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef LE_GNSS_MESSAGES_H_INCLUDE_GUARD
#define LE_GNSS_MESSAGES_H_INCLUDE_GUARD


#include "le_gnss_common.h"

#define _MAX_MSG_SIZE IFGEN_LE_GNSS_MSG_SIZE

// Define the message type for communicating between client and server
typedef struct __attribute__((packed))
{
    uint32_t id;
    uint8_t buffer[_MAX_MSG_SIZE];
}
_Message_t;

#define _MSGID_le_gnss_SetConstellation 0
#define _MSGID_le_gnss_GetConstellation 1
#define _MSGID_le_gnss_SetConstellationArea 2
#define _MSGID_le_gnss_GetConstellationArea 3
#define _MSGID_le_gnss_EnableExtendedEphemerisFile 4
#define _MSGID_le_gnss_DisableExtendedEphemerisFile 5
#define _MSGID_le_gnss_LoadExtendedEphemerisFile 6
#define _MSGID_le_gnss_GetExtendedEphemerisValidity 7
#define _MSGID_le_gnss_InjectUtcTime 8
#define _MSGID_le_gnss_Start 9
#define _MSGID_le_gnss_Stop 10
#define _MSGID_le_gnss_ForceHotRestart 11
#define _MSGID_le_gnss_ForceWarmRestart 12
#define _MSGID_le_gnss_ForceColdRestart 13
#define _MSGID_le_gnss_ForceFactoryRestart 14
#define _MSGID_le_gnss_GetTtff 15
#define _MSGID_le_gnss_Enable 16
#define _MSGID_le_gnss_Disable 17
#define _MSGID_le_gnss_SetAcquisitionRate 18
#define _MSGID_le_gnss_GetAcquisitionRate 19
#define _MSGID_le_gnss_AddPositionHandler 20
#define _MSGID_le_gnss_RemovePositionHandler 21
#define _MSGID_le_gnss_GetPositionState 22
#define _MSGID_le_gnss_GetLocation 23
#define _MSGID_le_gnss_GetAltitude 24
#define _MSGID_le_gnss_GetTime 25
#define _MSGID_le_gnss_GetGpsTime 26
#define _MSGID_le_gnss_GetEpochTime 27
#define _MSGID_le_gnss_GetTimeAccuracy 28
#define _MSGID_le_gnss_GetGpsLeapSeconds 29
#define _MSGID_le_gnss_GetLeapSeconds 30
#define _MSGID_le_gnss_GetDate 31
#define _MSGID_le_gnss_GetHorizontalSpeed 32
#define _MSGID_le_gnss_GetVerticalSpeed 33
#define _MSGID_le_gnss_GetDirection 34
#define _MSGID_le_gnss_GetSatellitesInfo 35
#define _MSGID_le_gnss_GetSbasConstellationCategory 36
#define _MSGID_le_gnss_GetSatellitesStatus 37
#define _MSGID_le_gnss_GetDop 38
#define _MSGID_le_gnss_GetDilutionOfPrecision 39
#define _MSGID_le_gnss_GetAltitudeOnWgs84 40
#define _MSGID_le_gnss_GetMagneticDeviation 41
#define _MSGID_le_gnss_GetLastSampleRef 42
#define _MSGID_le_gnss_ReleaseSampleRef 43
#define _MSGID_le_gnss_SetSuplAssistedMode 44
#define _MSGID_le_gnss_GetSuplAssistedMode 45
#define _MSGID_le_gnss_SetSuplServerUrl 46
#define _MSGID_le_gnss_InjectSuplCertificate 47
#define _MSGID_le_gnss_DeleteSuplCertificate 48
#define _MSGID_le_gnss_SetNmeaSentences 49
#define _MSGID_le_gnss_GetNmeaSentences 50
#define _MSGID_le_gnss_GetState 51
#define _MSGID_le_gnss_SetMinElevation 52
#define _MSGID_le_gnss_GetMinElevation 53
#define _MSGID_le_gnss_SetDopResolution 54
#define _MSGID_le_gnss_SetDataResolution 55
#define _MSGID_le_gnss_ConvertDataCoordinateSystem 56


// Define type-safe pack/unpack functions for all enums, including included types

static inline bool le_gnss_PackState
(
    uint8_t **bufferPtr,
    le_gnss_State_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_gnss_UnpackState
(
    uint8_t **bufferPtr,
    le_gnss_State_t* valuePtr
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

static inline bool le_gnss_PackFixState
(
    uint8_t **bufferPtr,
    le_gnss_FixState_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_gnss_UnpackFixState
(
    uint8_t **bufferPtr,
    le_gnss_FixState_t* valuePtr
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

static inline bool le_gnss_PackResolution
(
    uint8_t **bufferPtr,
    le_gnss_Resolution_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_gnss_UnpackResolution
(
    uint8_t **bufferPtr,
    le_gnss_Resolution_t* valuePtr
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

static inline bool le_gnss_PackDataType
(
    uint8_t **bufferPtr,
    le_gnss_DataType_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_gnss_UnpackDataType
(
    uint8_t **bufferPtr,
    le_gnss_DataType_t* valuePtr
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

static inline bool le_gnss_PackSbasConstellationCategory
(
    uint8_t **bufferPtr,
    le_gnss_SbasConstellationCategory_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_gnss_UnpackSbasConstellationCategory
(
    uint8_t **bufferPtr,
    le_gnss_SbasConstellationCategory_t* valuePtr
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

static inline bool le_gnss_PackConstellation
(
    uint8_t **bufferPtr,
    le_gnss_Constellation_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_gnss_UnpackConstellation
(
    uint8_t **bufferPtr,
    le_gnss_Constellation_t* valuePtr
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

static inline bool le_gnss_PackAssistedMode
(
    uint8_t **bufferPtr,
    le_gnss_AssistedMode_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_gnss_UnpackAssistedMode
(
    uint8_t **bufferPtr,
    le_gnss_AssistedMode_t* valuePtr
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

static inline bool le_gnss_PackDopType
(
    uint8_t **bufferPtr,
    le_gnss_DopType_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_gnss_UnpackDopType
(
    uint8_t **bufferPtr,
    le_gnss_DopType_t* valuePtr
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

static inline bool le_gnss_PackConstellationArea
(
    uint8_t **bufferPtr,
    le_gnss_ConstellationArea_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_gnss_UnpackConstellationArea
(
    uint8_t **bufferPtr,
    le_gnss_ConstellationArea_t* valuePtr
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

static inline bool le_gnss_PackConstellationBitMask
(
    uint8_t **bufferPtr,
    le_gnss_ConstellationBitMask_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_gnss_UnpackConstellationBitMask
(
    uint8_t **bufferPtr,
    le_gnss_ConstellationBitMask_t* valuePtr
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

static inline bool le_gnss_PackNmeaBitMask
(
    uint8_t **bufferPtr,
    le_gnss_NmeaBitMask_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_gnss_UnpackNmeaBitMask
(
    uint8_t **bufferPtr,
    le_gnss_NmeaBitMask_t* valuePtr
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

static inline bool le_gnss_PackCoordinateSystem
(
    uint8_t **bufferPtr,
    le_gnss_CoordinateSystem_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_gnss_UnpackCoordinateSystem
(
    uint8_t **bufferPtr,
    le_gnss_CoordinateSystem_t* valuePtr
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

static inline bool le_gnss_PackLocationDataType
(
    uint8_t **bufferPtr,
    le_gnss_LocationDataType_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_gnss_UnpackLocationDataType
(
    uint8_t **bufferPtr,
    le_gnss_LocationDataType_t* valuePtr
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


#endif // LE_GNSS_MESSAGES_H_INCLUDE_GUARD