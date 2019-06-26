/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef LE_WIFICLIENT_MESSAGES_H_INCLUDE_GUARD
#define LE_WIFICLIENT_MESSAGES_H_INCLUDE_GUARD


#include "le_wifiClient_common.h"

#define _MAX_MSG_SIZE IFGEN_LE_WIFICLIENT_MSG_SIZE

// Define the message type for communicating between client and server
typedef struct __attribute__((packed))
{
    uint32_t id;
    uint8_t buffer[_MAX_MSG_SIZE];
}
_Message_t;

#define _MSGID_le_wifiClient_AddNewEventHandler 0
#define _MSGID_le_wifiClient_RemoveNewEventHandler 1
#define _MSGID_le_wifiClient_AddConnectionEventHandler 2
#define _MSGID_le_wifiClient_RemoveConnectionEventHandler 3
#define _MSGID_le_wifiClient_Start 4
#define _MSGID_le_wifiClient_Stop 5
#define _MSGID_le_wifiClient_Scan 6
#define _MSGID_le_wifiClient_GetFirstAccessPoint 7
#define _MSGID_le_wifiClient_GetNextAccessPoint 8
#define _MSGID_le_wifiClient_GetSignalStrength 9
#define _MSGID_le_wifiClient_GetBssid 10
#define _MSGID_le_wifiClient_GetSsid 11
#define _MSGID_le_wifiClient_GetCurrentConnection 12
#define _MSGID_le_wifiClient_SetPassphrase 13
#define _MSGID_le_wifiClient_SetPreSharedKey 14
#define _MSGID_le_wifiClient_SetSecurityProtocol 15
#define _MSGID_le_wifiClient_SetUserCredentials 16
#define _MSGID_le_wifiClient_SetWepKey 17
#define _MSGID_le_wifiClient_SetHiddenNetworkAttribute 18
#define _MSGID_le_wifiClient_Create 19
#define _MSGID_le_wifiClient_Delete 20
#define _MSGID_le_wifiClient_Connect 21
#define _MSGID_le_wifiClient_Disconnect 22
#define _MSGID_le_wifiClient_LoadSsid 23
#define _MSGID_le_wifiClient_ConfigureWep 24
#define _MSGID_le_wifiClient_ConfigurePsk 25
#define _MSGID_le_wifiClient_ConfigureEap 26
#define _MSGID_le_wifiClient_RemoveSsidSecurityConfigs 27


// Define type-safe pack/unpack functions for all enums, including included types

static inline bool le_wifiClient_PackEvent
(
    uint8_t **bufferPtr,
    le_wifiClient_Event_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_wifiClient_UnpackEvent
(
    uint8_t **bufferPtr,
    le_wifiClient_Event_t* valuePtr
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

static inline bool le_wifiClient_PackDisconnectionCause
(
    uint8_t **bufferPtr,
    le_wifiClient_DisconnectionCause_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_wifiClient_UnpackDisconnectionCause
(
    uint8_t **bufferPtr,
    le_wifiClient_DisconnectionCause_t* valuePtr
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

static inline bool le_wifiClient_PackSecurityProtocol
(
    uint8_t **bufferPtr,
    le_wifiClient_SecurityProtocol_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_wifiClient_UnpackSecurityProtocol
(
    uint8_t **bufferPtr,
    le_wifiClient_SecurityProtocol_t* valuePtr
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


static inline bool le_wifiClient_PackEventInd
(
    uint8_t **bufferPtr,
    const le_wifiClient_EventInd_t *valuePtr
)
{
    __attribute__((unused))
    bool subResult, result = true;

    LE_ASSERT(valuePtr);
    subResult = le_wifiClient_PackEvent( bufferPtr,
                                                 valuePtr->event );
    result = result && subResult;
    subResult = le_wifiClient_PackDisconnectionCause( bufferPtr,
                                                 valuePtr->disconnectionCause );
    result = result && subResult;
    subResult = le_pack_PackString( bufferPtr,
                                    valuePtr->ifName, 31);
    result = result && subResult;
    subResult = le_pack_PackString( bufferPtr,
                                    valuePtr->apBssid, 18);
    result = result && subResult;

    return result;
}

static inline bool le_wifiClient_UnpackEventInd
(
    uint8_t **bufferPtr,
    le_wifiClient_EventInd_t *valuePtr
)
{
    bool result = true;
    if (result)
    {
        result = le_wifiClient_UnpackEvent(bufferPtr,
                                                   &valuePtr->event );
    }
    if (result)
    {
        result = le_wifiClient_UnpackDisconnectionCause(bufferPtr,
                                                   &valuePtr->disconnectionCause );
    }
    if (result)
    {
        result = le_pack_UnpackString(bufferPtr,
                                      valuePtr->ifName,
                                      sizeof(valuePtr->ifName),
                                      31);
    }
    if (result)
    {
        result = le_pack_UnpackString(bufferPtr,
                                      valuePtr->apBssid,
                                      sizeof(valuePtr->apBssid),
                                      18);
    }
    return result;
}

#endif // LE_WIFICLIENT_MESSAGES_H_INCLUDE_GUARD