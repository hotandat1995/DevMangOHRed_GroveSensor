
/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */
#ifndef LE_FRAMEWORK_COMMON_H_INCLUDE_GUARD
#define LE_FRAMEWORK_COMMON_H_INCLUDE_GUARD


#include "legato.h"

#define IFGEN_LE_FRAMEWORK_PROTOCOL_ID "28525bdc36d66247ae720424d0b28b51"
#define IFGEN_LE_FRAMEWORK_MSG_SIZE 12




//--------------------------------------------------------------------------------------------------
/**
 * Get if this client bound locally.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED bool ifgen_le_framework_HasLocalBinding
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Init data that is common across all threads
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED void ifgen_le_framework_InitCommonData
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Perform common initialization and open a session
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_framework_OpenSession
(
    le_msg_SessionRef_t _ifgen_sessionRef,
    bool isBlocking
);

//--------------------------------------------------------------------------------------------------
/**
 * Stops the Legato framework.
 *
 * @return
 *      LE_OK if successful.
 *      LE_DUPLICATE if the framework is in the process of shutting down (perhaps someone else has
 *                   already requested the framework be stopped or restarted).
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_framework_Stop
(
    le_msg_SessionRef_t _ifgen_sessionRef
);

//--------------------------------------------------------------------------------------------------
/**
 * Restarts the Legato framework.
 *
 * @return
 *      LE_OK if the request was accepted and the restart procedure has begun.
 *      LE_DUPLICATE if the framework is already in the process of shutting down (perhaps someone
 *                   else has already requested the framework be stopped or restarted).
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_framework_Restart
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        bool manualRestart
        ///< [IN] Was the restart manually triggered e.g. "legato restart"
);

//--------------------------------------------------------------------------------------------------
/**
 * Reports if the Legato framework is stopping.
 *
 * @return
 *     true if the framework is stopping or rebooting
 *     false otherwise
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED bool ifgen_le_framework_IsStopping
(
    le_msg_SessionRef_t _ifgen_sessionRef
);

//--------------------------------------------------------------------------------------------------
/**
 * Checks whether legato framework is Read-Only or not.
 *
 * @return
 *     true if the framework is Read-Only
 *     false otherwise
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED bool ifgen_le_framework_IsReadOnly
(
    le_msg_SessionRef_t _ifgen_sessionRef
);

//--------------------------------------------------------------------------------------------------
/**
 * Mark the next reboot as expected. Should be called by short lived app that shutdown platform
 * after a small wakeup. This prevents system not to rollback on expected reboot.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED void ifgen_le_framework_NotifyExpectedReboot
(
    le_msg_SessionRef_t _ifgen_sessionRef
);

#endif // LE_FRAMEWORK_COMMON_H_INCLUDE_GUARD