/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef MULTICHANNELRELAY_MESSAGES_H_INCLUDE_GUARD
#define MULTICHANNELRELAY_MESSAGES_H_INCLUDE_GUARD


#include "multiChannelRelay_common.h"

#define _MAX_MSG_SIZE IFGEN_MULTICHANNELRELAY_MSG_SIZE

// Define the message type for communicating between client and server
typedef struct __attribute__((packed))
{
    uint32_t id;
    uint8_t buffer[_MAX_MSG_SIZE];
}
_Message_t;

#define _MSGID_multiChannelRelay_turn_off_channel 0
#define _MSGID_multiChannelRelay_turn_on_channel 1
#define _MSGID_multiChannelRelay_getChannelState 2


// Define type-safe pack/unpack functions for all enums, including included types

// Define pack/unpack functions for all structures, including included types


#endif // MULTICHANNELRELAY_MESSAGES_H_INCLUDE_GUARD