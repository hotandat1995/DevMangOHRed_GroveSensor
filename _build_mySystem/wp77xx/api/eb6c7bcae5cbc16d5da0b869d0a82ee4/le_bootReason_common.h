
/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */
#ifndef LE_BOOTREASON_COMMON_H_INCLUDE_GUARD
#define LE_BOOTREASON_COMMON_H_INCLUDE_GUARD


#include "legato.h"

#define IFGEN_LE_BOOTREASON_PROTOCOL_ID "2d79668511adea4d675f5d77b9f2d3ea"
#define IFGEN_LE_BOOTREASON_MSG_SIZE 12




//--------------------------------------------------------------------------------------------------
/**
 * Get if this client bound locally.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED bool ifgen_le_bootReason_HasLocalBinding
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Init data that is common across all threads
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED void ifgen_le_bootReason_InitCommonData
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Perform common initialization and open a session
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_bootReason_OpenSession
(
    le_msg_SessionRef_t _ifgen_sessionRef,
    bool isBlocking
);

//--------------------------------------------------------------------------------------------------
/**
 * Checks whether boot-reason was timer expiry.
 *
 * @return
 *      - TRUE if boot-reason was timer expiry.
 *      - FALSE otherwise.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED bool ifgen_le_bootReason_WasTimer
(
    le_msg_SessionRef_t _ifgen_sessionRef
);

//--------------------------------------------------------------------------------------------------
/**
 * Checks whether boot-reason was specific gpio change. GPIO number is specified in parameter.
 *
 * @return
 *      - TRUE if boot-reason was specified gpio change.
 *      - FALSE otherwise.
 *
 * @note The process exits if invalid gpio number is passed.  Check corresponding device documents
 * for valid list of gpio.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED bool ifgen_le_bootReason_WasGpio
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        uint32_t gpioNum
        ///< [IN] GPIO number.
);

//--------------------------------------------------------------------------------------------------
/**
 * Checks whether boot reason was due to the specified ADC having a reading above or below the
 * configured limits.
 *
 * @return
 *      true if boot reason was due to the given ADC or false otherwise.
 *
 * @note
 *      The process exits if an invalid ADC number is passed. Check corresponding device documents
 *      for valid list of ADC numbers.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED bool ifgen_le_bootReason_WasAdc
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        uint32_t adcNum
        ///< [IN] ADC number
);

#endif // LE_BOOTREASON_COMMON_H_INCLUDE_GUARD