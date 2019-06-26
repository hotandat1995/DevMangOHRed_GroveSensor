
/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */
#ifndef LE_RIPIN_COMMON_H_INCLUDE_GUARD
#define LE_RIPIN_COMMON_H_INCLUDE_GUARD


#include "legato.h"

#define IFGEN_LE_RIPIN_PROTOCOL_ID "aafbc3c57eec1350fdae5eb6fe083e5a"
#define IFGEN_LE_RIPIN_MSG_SIZE 13




//--------------------------------------------------------------------------------------------------
/**
 * Get if this client bound locally.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED bool ifgen_le_riPin_HasLocalBinding
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Init data that is common across all threads
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED void ifgen_le_riPin_InitCommonData
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Perform common initialization and open a session
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_riPin_OpenSession
(
    le_msg_SessionRef_t _ifgen_sessionRef,
    bool isBlocking
);

//--------------------------------------------------------------------------------------------------
/**
 * Check whether the application core is the current owner of the Ring Indicator signal.
 *
 * @return
 *      - LE_OK              The function succeeded.
 *      - LE_FAULT           The function failed.
 *      - LE_BAD_PARAMETER   Bad input parameter.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_riPin_AmIOwnerOfRingSignal
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        bool* amIOwnerPtr
        ///< [OUT] true when application core is the owner of the Ring Indicator signal,
);

//--------------------------------------------------------------------------------------------------
/**
 * Take control of the Ring Indicator signal.
 *
 * @return
 *      - LE_OK           The function succeeded.
 *      - LE_FAULT        The function failed.
 *      - LE_UNSUPPORTED  The platform does not support this operation.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_riPin_TakeRingSignal
(
    le_msg_SessionRef_t _ifgen_sessionRef
);

//--------------------------------------------------------------------------------------------------
/**
 * Release control of the Ring Indicator signal.
 *
 * @return
 *      - LE_OK           The function succeeded.
 *      - LE_FAULT        The function failed.
 *      - LE_UNSUPPORTED  The platform does not support this operation.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_riPin_ReleaseRingSignal
(
    le_msg_SessionRef_t _ifgen_sessionRef
);

//--------------------------------------------------------------------------------------------------
/**
 * Set the Ring Indicator signal to high for configurable duration of time before lowering it.
 *
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED void ifgen_le_riPin_PulseRingSignal
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        uint32_t duration
        ///< [IN] [IN] Duration in ms
);

#endif // LE_RIPIN_COMMON_H_INCLUDE_GUARD