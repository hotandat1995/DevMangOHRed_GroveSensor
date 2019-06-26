
/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef LE_ECALL_SERVICE_H_INCLUDE_GUARD
#define LE_ECALL_SERVICE_H_INCLUDE_GUARD


#include "legato.h"

#define PROTOCOL_ID_STR IFGEN_LE_ECALL_PROTOCOL_ID
#ifdef MK_TOOLS_BUILD
    extern const char** le_ecall_ServiceInstanceNamePtr;
    #define SERVICE_INSTANCE_NAME (*le_ecall_ServiceInstanceNamePtr)
#else
    #define SERVICE_INSTANCE_NAME "le_ecall"
#endif

#endif // LE_ECALL_SERVICE_H_INCLUDE_GUARD