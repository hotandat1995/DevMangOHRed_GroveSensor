/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef LE_APPCTRL_MESSAGES_H_INCLUDE_GUARD
#define LE_APPCTRL_MESSAGES_H_INCLUDE_GUARD


#include "le_appCtrl_common.h"

#define _MAX_MSG_SIZE IFGEN_LE_APPCTRL_MSG_SIZE

// Define the message type for communicating between client and server
typedef struct __attribute__((packed))
{
    uint32_t id;
    uint8_t buffer[_MAX_MSG_SIZE];
}
_Message_t;

#define _MSGID_le_appCtrl_GetRef 0
#define _MSGID_le_appCtrl_ReleaseRef 1
#define _MSGID_le_appCtrl_SetRun 2
#define _MSGID_le_appCtrl_SetDebug 3
#define _MSGID_le_appCtrl_Import 4
#define _MSGID_le_appCtrl_SetDevicePerm 5
#define _MSGID_le_appCtrl_AddTraceAttachHandler 6
#define _MSGID_le_appCtrl_RemoveTraceAttachHandler 7
#define _MSGID_le_appCtrl_TraceUnblock 8
#define _MSGID_le_appCtrl_Start 9
#define _MSGID_le_appCtrl_Stop 10


// Define type-safe pack/unpack functions for all enums, including included types

// Define pack/unpack functions for all structures, including included types


#endif // LE_APPCTRL_MESSAGES_H_INCLUDE_GUARD