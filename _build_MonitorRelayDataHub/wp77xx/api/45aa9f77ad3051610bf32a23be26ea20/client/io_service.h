
/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef IO_SERVICE_H_INCLUDE_GUARD
#define IO_SERVICE_H_INCLUDE_GUARD


#include "legato.h"

#define PROTOCOL_ID_STR IFGEN_IO_PROTOCOL_ID
#ifdef MK_TOOLS_BUILD
    extern const char** io_ServiceInstanceNamePtr;
    #define SERVICE_INSTANCE_NAME (*io_ServiceInstanceNamePtr)
#else
    #define SERVICE_INSTANCE_NAME "io"
#endif

#endif // IO_SERVICE_H_INCLUDE_GUARD