/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef LE_ATCLIENT_MESSAGES_H_INCLUDE_GUARD
#define LE_ATCLIENT_MESSAGES_H_INCLUDE_GUARD


#include "le_atClient_common.h"

#define _MAX_MSG_SIZE IFGEN_LE_ATCLIENT_MSG_SIZE

// Define the message type for communicating between client and server
typedef struct __attribute__((packed))
{
    uint32_t id;
    uint8_t buffer[_MAX_MSG_SIZE];
}
_Message_t;

#define _MSGID_le_atClient_Start 0
#define _MSGID_le_atClient_Stop 1
#define _MSGID_le_atClient_Create 2
#define _MSGID_le_atClient_Delete 3
#define _MSGID_le_atClient_SetCommand 4
#define _MSGID_le_atClient_SetIntermediateResponse 5
#define _MSGID_le_atClient_SetFinalResponse 6
#define _MSGID_le_atClient_SetText 7
#define _MSGID_le_atClient_SetTimeout 8
#define _MSGID_le_atClient_SetDevice 9
#define _MSGID_le_atClient_Send 10
#define _MSGID_le_atClient_GetFirstIntermediateResponse 11
#define _MSGID_le_atClient_GetNextIntermediateResponse 12
#define _MSGID_le_atClient_GetFinalResponse 13
#define _MSGID_le_atClient_SetCommandAndSend 14
#define _MSGID_le_atClient_AddUnsolicitedResponseHandler 15
#define _MSGID_le_atClient_RemoveUnsolicitedResponseHandler 16


// Define type-safe pack/unpack functions for all enums, including included types

// Define pack/unpack functions for all structures, including included types


#endif // LE_ATCLIENT_MESSAGES_H_INCLUDE_GUARD