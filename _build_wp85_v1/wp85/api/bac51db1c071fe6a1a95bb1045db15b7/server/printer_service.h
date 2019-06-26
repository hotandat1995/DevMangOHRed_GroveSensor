
/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef PRINTER_SERVICE_H_INCLUDE_GUARD
#define PRINTER_SERVICE_H_INCLUDE_GUARD


#include "legato.h"

#define PROTOCOL_ID_STR IFGEN_PRINTER_PROTOCOL_ID
#ifdef MK_TOOLS_BUILD
    extern const char** printer_ServiceInstanceNamePtr;
    #define SERVICE_INSTANCE_NAME (*printer_ServiceInstanceNamePtr)
#else
    #define SERVICE_INSTANCE_NAME "printer"
#endif

#endif // PRINTER_SERVICE_H_INCLUDE_GUARD