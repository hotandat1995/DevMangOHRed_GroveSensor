
/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef LE_RSIM_SERVICE_H_INCLUDE_GUARD
#define LE_RSIM_SERVICE_H_INCLUDE_GUARD


#include "legato.h"

#define PROTOCOL_ID_STR IFGEN_LE_RSIM_PROTOCOL_ID
#ifdef MK_TOOLS_BUILD
    extern const char** le_rsim_ServiceInstanceNamePtr;
    #define SERVICE_INSTANCE_NAME (*le_rsim_ServiceInstanceNamePtr)
#else
    #define SERVICE_INSTANCE_NAME "le_rsim"
#endif

#endif // LE_RSIM_SERVICE_H_INCLUDE_GUARD