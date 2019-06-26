/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef LE_SPI_MESSAGES_H_INCLUDE_GUARD
#define LE_SPI_MESSAGES_H_INCLUDE_GUARD


#include "le_spi_common.h"

#define _MAX_MSG_SIZE IFGEN_LE_SPI_MSG_SIZE

// Define the message type for communicating between client and server
typedef struct __attribute__((packed))
{
    uint32_t id;
    uint8_t buffer[_MAX_MSG_SIZE];
}
_Message_t;

#define _MSGID_le_spi_Open 0
#define _MSGID_le_spi_Close 1
#define _MSGID_le_spi_Configure 2
#define _MSGID_le_spi_WriteReadHD 3
#define _MSGID_le_spi_WriteHD 4
#define _MSGID_le_spi_ReadHD 5
#define _MSGID_le_spi_WriteReadFD 6


// Define type-safe pack/unpack functions for all enums, including included types

// Define pack/unpack functions for all structures, including included types


#endif // LE_SPI_MESSAGES_H_INCLUDE_GUARD