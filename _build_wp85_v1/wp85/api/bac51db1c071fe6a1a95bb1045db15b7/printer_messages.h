/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef PRINTER_MESSAGES_H_INCLUDE_GUARD
#define PRINTER_MESSAGES_H_INCLUDE_GUARD


#include "printer_common.h"

#define _MAX_MSG_SIZE IFGEN_PRINTER_MSG_SIZE

// Define the message type for communicating between client and server
typedef struct __attribute__((packed))
{
    uint32_t id;
    uint8_t buffer[_MAX_MSG_SIZE];
}
_Message_t;

#define _MSGID_printer_Print 0


// Define type-safe pack/unpack functions for all enums, including included types

// Define pack/unpack functions for all structures, including included types


#endif // PRINTER_MESSAGES_H_INCLUDE_GUARD