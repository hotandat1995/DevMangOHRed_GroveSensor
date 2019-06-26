
/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef LE_KERNELMODULE_SERVICE_H_INCLUDE_GUARD
#define LE_KERNELMODULE_SERVICE_H_INCLUDE_GUARD


#include "legato.h"

#define PROTOCOL_ID_STR IFGEN_LE_KERNELMODULE_PROTOCOL_ID
#ifdef MK_TOOLS_BUILD
    extern const char** le_kernelModule_ServiceInstanceNamePtr;
    #define SERVICE_INSTANCE_NAME (*le_kernelModule_ServiceInstanceNamePtr)
#else
    #define SERVICE_INSTANCE_NAME "le_kernelModule"
#endif

#endif // LE_KERNELMODULE_SERVICE_H_INCLUDE_GUARD