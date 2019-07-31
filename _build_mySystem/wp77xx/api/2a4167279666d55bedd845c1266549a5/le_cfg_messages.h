/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef LE_CFG_MESSAGES_H_INCLUDE_GUARD
#define LE_CFG_MESSAGES_H_INCLUDE_GUARD


#include "le_cfg_common.h"

#define _MAX_MSG_SIZE IFGEN_LE_CFG_MSG_SIZE

// Define the message type for communicating between client and server
typedef struct __attribute__((packed))
{
    uint32_t id;
    uint8_t buffer[_MAX_MSG_SIZE];
}
_Message_t;

#define _MSGID_le_cfg_CreateReadTxn 0
#define _MSGID_le_cfg_CreateWriteTxn 1
#define _MSGID_le_cfg_CommitTxn 2
#define _MSGID_le_cfg_CancelTxn 3
#define _MSGID_le_cfg_GoToNode 4
#define _MSGID_le_cfg_GoToParent 5
#define _MSGID_le_cfg_GoToFirstChild 6
#define _MSGID_le_cfg_GoToNextSibling 7
#define _MSGID_le_cfg_GetPath 8
#define _MSGID_le_cfg_GetNodeType 9
#define _MSGID_le_cfg_GetNodeName 10
#define _MSGID_le_cfg_AddChangeHandler 11
#define _MSGID_le_cfg_RemoveChangeHandler 12
#define _MSGID_le_cfg_DeleteNode 13
#define _MSGID_le_cfg_IsEmpty 14
#define _MSGID_le_cfg_SetEmpty 15
#define _MSGID_le_cfg_NodeExists 16
#define _MSGID_le_cfg_GetString 17
#define _MSGID_le_cfg_SetString 18
#define _MSGID_le_cfg_GetBinary 19
#define _MSGID_le_cfg_SetBinary 20
#define _MSGID_le_cfg_GetInt 21
#define _MSGID_le_cfg_SetInt 22
#define _MSGID_le_cfg_GetFloat 23
#define _MSGID_le_cfg_SetFloat 24
#define _MSGID_le_cfg_GetBool 25
#define _MSGID_le_cfg_SetBool 26
#define _MSGID_le_cfg_QuickDeleteNode 27
#define _MSGID_le_cfg_QuickSetEmpty 28
#define _MSGID_le_cfg_QuickGetString 29
#define _MSGID_le_cfg_QuickSetString 30
#define _MSGID_le_cfg_QuickGetBinary 31
#define _MSGID_le_cfg_QuickSetBinary 32
#define _MSGID_le_cfg_QuickGetInt 33
#define _MSGID_le_cfg_QuickSetInt 34
#define _MSGID_le_cfg_QuickGetFloat 35
#define _MSGID_le_cfg_QuickSetFloat 36
#define _MSGID_le_cfg_QuickGetBool 37
#define _MSGID_le_cfg_QuickSetBool 38


// Define type-safe pack/unpack functions for all enums, including included types

static inline bool le_cfg_PacknodeType
(
    uint8_t **bufferPtr,
    le_cfg_nodeType_t value
)
{
    return le_pack_PackUint32(bufferPtr, value);
}

static inline bool le_cfg_UnpacknodeType
(
    uint8_t **bufferPtr,
    le_cfg_nodeType_t* valuePtr
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


#endif // LE_CFG_MESSAGES_H_INCLUDE_GUARD