
/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */
#ifndef LE_PM_COMMON_H_INCLUDE_GUARD
#define LE_PM_COMMON_H_INCLUDE_GUARD


#include "legato.h"

#define IFGEN_LE_PM_PROTOCOL_ID "39f21a4932ef87d302a8f31dfe5a0af7"
#define IFGEN_LE_PM_MSG_SIZE 47



//--------------------------------------------------------------------------------------------------
/**
 * Maximum string length for a wake-up source tag (not including the null-terminator)
 */
//--------------------------------------------------------------------------------------------------
#define LE_PM_TAG_LEN 31

//--------------------------------------------------------------------------------------------------
/**
 * Maximum string length for a wake-up source tag (including the null-terminator)
 */
//--------------------------------------------------------------------------------------------------
#define LE_PM_TAG_LEN_BYTES 32

//--------------------------------------------------------------------------------------------------
/**
 * Option LE_PM_REF_COUNT to manage a reference counted wakeup source
 */
//--------------------------------------------------------------------------------------------------
#define LE_PM_REF_COUNT 1

//--------------------------------------------------------------------------------------------------
/**
 * Reference to wakeup source used by StayAwake and Relax function
 */
//--------------------------------------------------------------------------------------------------
typedef struct le_pm_WakeupSource* le_pm_WakeupSourceRef_t;



//--------------------------------------------------------------------------------------------------
/**
 * Get if this client bound locally.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED bool ifgen_le_pm_HasLocalBinding
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Init data that is common across all threads
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED void ifgen_le_pm_InitCommonData
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Perform common initialization and open a session
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_pm_OpenSession
(
    le_msg_SessionRef_t _ifgen_sessionRef,
    bool isBlocking
);

//--------------------------------------------------------------------------------------------------
/**
 * Create a wakeup source
 *
 * @return
 *      - Reference to wakeup source (to be used later for acquiring/releasing)
 *
 * @note The process exits if an invalid or existing tag is passed
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_pm_WakeupSourceRef_t ifgen_le_pm_NewWakeupSource
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        uint32_t createOpts,
        ///< [IN] Wakeup source options
        const char* LE_NONNULL wsTag
        ///< [IN] Context-specific wakeup source tag
);

//--------------------------------------------------------------------------------------------------
/**
 * Acquire a wakeup source
 *
 * @return
 *     - LE_OK          if the wakeup source is acquired
 *     - LE_NO_MEMORY   if the wakeup sources limit is reached
 *     - LE_FAULT       for other errors
 *
 * @note The process exits if an invalid reference is passed
 * @note The wakeup sources limit is fixed by the kernel CONFIG_PM_WAKELOCKS_LIMIT configuration
 *       variable
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_pm_StayAwake
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_pm_WakeupSourceRef_t wsRef
        ///< [IN] Reference to a created wakeup source
);

//--------------------------------------------------------------------------------------------------
/**
 * Release a previously acquired wakeup source
 *
 * @return
 *     - LE_OK          if the wakeup source is acquired
 *     - LE_NOT_FOUND   if the wakeup source was not currently acquired
 *     - LE_FAULT       for other errors
 *
 * @note The process exits if an invalid reference is passed
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_pm_Relax
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_pm_WakeupSourceRef_t wsRef
        ///< [IN] Reference to a created wakeup source
);

//--------------------------------------------------------------------------------------------------
/**
 * Release and destroy all acquired wakeup source, kill all clients
 *
 * @return
 *     - LE_OK              if the wakeup source is acquired
 *     - LE_NOT_PERMITTED   if the le_pm_StayAwake() has not failed with LE_NO_MEMORY
 *     - LE_FAULT           for other errors
 *
 * @note The service is available only if le_pm_StayAwake() has returned LE_NO_MEMORY. It should be
 *       used in the way to release and destroy all wakeup sources.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_pm_ForceRelaxAndDestroyAllWakeupSource
(
    le_msg_SessionRef_t _ifgen_sessionRef
);

#endif // LE_PM_COMMON_H_INCLUDE_GUARD