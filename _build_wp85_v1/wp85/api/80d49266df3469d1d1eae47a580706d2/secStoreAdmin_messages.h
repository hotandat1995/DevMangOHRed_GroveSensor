/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef SECSTOREADMIN_MESSAGES_H_INCLUDE_GUARD
#define SECSTOREADMIN_MESSAGES_H_INCLUDE_GUARD


#include "secStoreAdmin_common.h"

#define _MAX_MSG_SIZE IFGEN_SECSTOREADMIN_MSG_SIZE

// Define the message type for communicating between client and server
typedef struct __attribute__((packed))
{
    uint32_t id;
    uint8_t buffer[_MAX_MSG_SIZE];
}
_Message_t;

#define _MSGID_secStoreAdmin_CreateIter 0
#define _MSGID_secStoreAdmin_DeleteIter 1
#define _MSGID_secStoreAdmin_Next 2
#define _MSGID_secStoreAdmin_GetEntry 3
#define _MSGID_secStoreAdmin_Write 4
#define _MSGID_secStoreAdmin_Read 5
#define _MSGID_secStoreAdmin_CopyMetaTo 6
#define _MSGID_secStoreAdmin_Delete 7
#define _MSGID_secStoreAdmin_GetSize 8
#define _MSGID_secStoreAdmin_GetTotalSpace 9


// Define type-safe pack/unpack functions for all enums, including included types

// Define pack/unpack functions for all structures, including included types


#endif // SECSTOREADMIN_MESSAGES_H_INCLUDE_GUARD