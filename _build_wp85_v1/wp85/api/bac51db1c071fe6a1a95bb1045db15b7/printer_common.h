
/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */
#ifndef PRINTER_COMMON_H_INCLUDE_GUARD
#define PRINTER_COMMON_H_INCLUDE_GUARD


#include "legato.h"

#define IFGEN_PRINTER_PROTOCOL_ID "f9db99806649629c780644fb01ff236e"
#define IFGEN_PRINTER_MSG_SIZE 112



//--------------------------------------------------------------------------------------------------
/**
 */
//--------------------------------------------------------------------------------------------------
#define PRINTER_MESSAGE_LEN 100


//--------------------------------------------------------------------------------------------------
/**
 * Get if this client bound locally.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED bool ifgen_printer_HasLocalBinding
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Init data that is common across all threads
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED void ifgen_printer_InitCommonData
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Perform common initialization and open a session
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_printer_OpenSession
(
    le_msg_SessionRef_t _ifgen_sessionRef,
    bool isBlocking
);

//--------------------------------------------------------------------------------------------------
/**
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED void ifgen_printer_Print
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        const char* LE_NONNULL message
        ///< [IN]
);

#endif // PRINTER_COMMON_H_INCLUDE_GUARD