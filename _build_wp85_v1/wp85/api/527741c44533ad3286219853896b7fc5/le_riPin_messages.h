/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef LE_RIPIN_MESSAGES_H_INCLUDE_GUARD
#define LE_RIPIN_MESSAGES_H_INCLUDE_GUARD


#include "le_riPin_common.h"

#define _MAX_MSG_SIZE IFGEN_LE_RIPIN_MSG_SIZE

// Define the message type for communicating between client and server
typedef struct __attribute__((packed))
{
    uint32_t id;
    uint8_t buffer[_MAX_MSG_SIZE];
}
_Message_t;

#define _MSGID_le_riPin_AmIOwnerOfRingSignal 0
#define _MSGID_le_riPin_TakeRingSignal 1
#define _MSGID_le_riPin_ReleaseRingSignal 2
#define _MSGID_le_riPin_PulseRingSignal 3


// Define type-safe pack/unpack functions for all enums, including included types

// Define pack/unpack functions for all structures, including included types


#endif // LE_RIPIN_MESSAGES_H_INCLUDE_GUARD