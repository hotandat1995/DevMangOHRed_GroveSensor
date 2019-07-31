
/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef SECSTOREGLOBAL_SERVICE_H_INCLUDE_GUARD
#define SECSTOREGLOBAL_SERVICE_H_INCLUDE_GUARD


#include "legato.h"

#define PROTOCOL_ID_STR IFGEN_LE_SECSTORE_PROTOCOL_ID
#ifdef MK_TOOLS_BUILD
    extern const char** secStoreGlobal_ServiceInstanceNamePtr;
    #define SERVICE_INSTANCE_NAME (*secStoreGlobal_ServiceInstanceNamePtr)
#else
    #define SERVICE_INSTANCE_NAME "le_secStore"
#endif

#endif // SECSTOREGLOBAL_SERVICE_H_INCLUDE_GUARD