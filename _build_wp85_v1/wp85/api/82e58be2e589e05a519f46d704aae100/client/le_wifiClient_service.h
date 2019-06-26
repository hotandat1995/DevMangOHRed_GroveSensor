
/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef LE_WIFICLIENT_SERVICE_H_INCLUDE_GUARD
#define LE_WIFICLIENT_SERVICE_H_INCLUDE_GUARD


#include "legato.h"

#define PROTOCOL_ID_STR IFGEN_LE_WIFICLIENT_PROTOCOL_ID
#ifdef MK_TOOLS_BUILD
    extern const char** le_wifiClient_ServiceInstanceNamePtr;
    #define SERVICE_INSTANCE_NAME (*le_wifiClient_ServiceInstanceNamePtr)
#else
    #define SERVICE_INSTANCE_NAME "le_wifiClient"
#endif

#endif // LE_WIFICLIENT_SERVICE_H_INCLUDE_GUARD