
/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef ADMIN_SERVICE_H_INCLUDE_GUARD
#define ADMIN_SERVICE_H_INCLUDE_GUARD


#include "legato.h"

#define PROTOCOL_ID_STR IFGEN_ADMIN_PROTOCOL_ID
#ifdef MK_TOOLS_BUILD
    extern const char** admin_ServiceInstanceNamePtr;
    #define SERVICE_INSTANCE_NAME (*admin_ServiceInstanceNamePtr)
#else
    #define SERVICE_INSTANCE_NAME "admin"
#endif

#endif // ADMIN_SERVICE_H_INCLUDE_GUARD