
/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */
#ifndef LE_SHUTDOWN_COMMON_H_INCLUDE_GUARD
#define LE_SHUTDOWN_COMMON_H_INCLUDE_GUARD


#include "legato.h"

#define IFGEN_LE_SHUTDOWN_PROTOCOL_ID "d51647e41642ee914b6b0848c009001c"
#define IFGEN_LE_SHUTDOWN_MSG_SIZE 12




//--------------------------------------------------------------------------------------------------
/**
 * Get if this client bound locally.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED bool ifgen_le_shutdown_HasLocalBinding
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Init data that is common across all threads
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED void ifgen_le_shutdown_InitCommonData
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Perform common initialization and open a session
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_shutdown_OpenSession
(
    le_msg_SessionRef_t _ifgen_sessionRef,
    bool isBlocking
);

//--------------------------------------------------------------------------------------------------
/**
 * Issue a fast shutdown.
 *
 * @return
 *  - LE_OK            Function success
 *  - LE_FAULT         Internal error
 *  - LE_UNSUPPORTED   Feature not supported
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_shutdown_IssueFast
(
    le_msg_SessionRef_t _ifgen_sessionRef
);

#endif // LE_SHUTDOWN_COMMON_H_INCLUDE_GUARD