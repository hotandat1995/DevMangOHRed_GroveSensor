
/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */
#ifndef LE_WDOG_COMMON_H_INCLUDE_GUARD
#define LE_WDOG_COMMON_H_INCLUDE_GUARD


#include "legato.h"

#define IFGEN_LE_WDOG_PROTOCOL_ID "572ba176b790548eed89b7ba6f9f4b14"
#define IFGEN_LE_WDOG_MSG_SIZE 20



//--------------------------------------------------------------------------------------------------
/**
 * Suspend the watchdog so that it never times out.
 */
//--------------------------------------------------------------------------------------------------
#define LE_WDOG_TIMEOUT_NEVER -1

//--------------------------------------------------------------------------------------------------
/**
 * Timeout immediately
 */
//--------------------------------------------------------------------------------------------------
#define LE_WDOG_TIMEOUT_NOW 0

//--------------------------------------------------------------------------------------------------
/**
 * External watchdog kick handler
 */
//--------------------------------------------------------------------------------------------------
typedef void (*le_wdog_ExternalWatchdogHandlerFunc_t)
(
        void* contextPtr
        ///<
);


//--------------------------------------------------------------------------------------------------
/**
 * Get if this client bound locally.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED bool ifgen_le_wdog_HasLocalBinding
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Init data that is common across all threads
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED void ifgen_le_wdog_InitCommonData
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Perform common initialization and open a session
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_wdog_OpenSession
(
    le_msg_SessionRef_t _ifgen_sessionRef,
    bool isBlocking
);

//--------------------------------------------------------------------------------------------------
/**
 * Kicks the watchdog timer.
 *
 * Once the watchdog has been kicked it must be kicked again before the expiration of the current
 * effective timeout else the configured WatchdogAction will be executed.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED void ifgen_le_wdog_Kick
(
    le_msg_SessionRef_t _ifgen_sessionRef
);

//--------------------------------------------------------------------------------------------------
/**
 * Set a time out.
 *
 * The watchdog is kicked and a new effective timeout value is set. The new timeout will be
 * effective until the next kick at which point it will revert to the original value.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED void ifgen_le_wdog_Timeout
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        int32_t milliseconds
        ///< [IN] The number of milliseconds until this timer expires
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the watchdog timeout configured for this process
 *
 * @return
 *      - LE_OK            The watchdog timeout is configured and returned
 *      - LE_NOT_FOUND     The watchdog timeout is not set
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_wdog_GetWatchdogTimeout
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        uint64_t* millisecondsPtr
        ///< [OUT] The watchdog timeout set for this process
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the max watchdog timeout configured for this process
 *
 * @return
 *      - LE_OK            The max watchdog timeout is configured and returned
 *      - LE_NOT_FOUND     The max watchdog timeout is not set
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_wdog_GetMaxWatchdogTimeout
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        uint64_t* millisecondsPtr
        ///< [OUT] The max watchdog timeout set for this process
);

#endif // LE_WDOG_COMMON_H_INCLUDE_GUARD