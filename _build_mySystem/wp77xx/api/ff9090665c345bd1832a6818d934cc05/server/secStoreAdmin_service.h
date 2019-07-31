
/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef SECSTOREADMIN_SERVICE_H_INCLUDE_GUARD
#define SECSTOREADMIN_SERVICE_H_INCLUDE_GUARD


#include "legato.h"

#define PROTOCOL_ID_STR IFGEN_SECSTOREADMIN_PROTOCOL_ID
#ifdef MK_TOOLS_BUILD
    extern const char** secStoreAdmin_ServiceInstanceNamePtr;
    #define SERVICE_INSTANCE_NAME (*secStoreAdmin_ServiceInstanceNamePtr)
#else
    #define SERVICE_INSTANCE_NAME "secStoreAdmin"
#endif

#endif // SECSTOREADMIN_SERVICE_H_INCLUDE_GUARD