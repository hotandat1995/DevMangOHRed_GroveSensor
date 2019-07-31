
/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef LE_FLASH_SERVICE_H_INCLUDE_GUARD
#define LE_FLASH_SERVICE_H_INCLUDE_GUARD


#include "legato.h"

#define PROTOCOL_ID_STR IFGEN_LE_FLASH_PROTOCOL_ID
#ifdef MK_TOOLS_BUILD
    extern const char** le_flash_ServiceInstanceNamePtr;
    #define SERVICE_INSTANCE_NAME (*le_flash_ServiceInstanceNamePtr)
#else
    #define SERVICE_INSTANCE_NAME "le_flash"
#endif

#endif // LE_FLASH_SERVICE_H_INCLUDE_GUARD