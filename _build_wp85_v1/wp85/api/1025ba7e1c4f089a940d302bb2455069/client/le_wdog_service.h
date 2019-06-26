
/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef LE_WDOG_SERVICE_H_INCLUDE_GUARD
#define LE_WDOG_SERVICE_H_INCLUDE_GUARD


#include "legato.h"

#define PROTOCOL_ID_STR IFGEN_LE_WDOG_PROTOCOL_ID
#ifdef MK_TOOLS_BUILD
    extern const char** le_wdog_ServiceInstanceNamePtr;
    #define SERVICE_INSTANCE_NAME (*le_wdog_ServiceInstanceNamePtr)
#else
    #define SERVICE_INSTANCE_NAME "le_wdog"
#endif

#endif // LE_WDOG_SERVICE_H_INCLUDE_GUARD