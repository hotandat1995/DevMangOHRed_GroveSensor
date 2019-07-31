
/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */
#ifndef LE_RTC_COMMON_H_INCLUDE_GUARD
#define LE_RTC_COMMON_H_INCLUDE_GUARD


#include "legato.h"

#define IFGEN_LE_RTC_PROTOCOL_ID "88750cf71cdf94a8a7d68de367465558"
#define IFGEN_LE_RTC_MSG_SIZE 20




//--------------------------------------------------------------------------------------------------
/**
 * Get if this client bound locally.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED bool ifgen_le_rtc_HasLocalBinding
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Init data that is common across all threads
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED void ifgen_le_rtc_InitCommonData
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Perform common initialization and open a session
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_rtc_OpenSession
(
    le_msg_SessionRef_t _ifgen_sessionRef,
    bool isBlocking
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the current time from the RTC
 *
 * @return
 *      - LE_OK            Function succeeded.
 *      - LE_FAULT         Function failed.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_rtc_GetUserTime
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        uint64_t* millisecondsPastGpsEpochPtr
        ///< [OUT]
);

//--------------------------------------------------------------------------------------------------
/**
 * Set the RTC with the given time string
 *
 * @return
 *      - LE_OK            Function succeeded.
 *      - LE_FAULT         Function failed.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_rtc_SetUserTime
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        uint64_t millisecondsPastGpsEpoch
        ///< [IN]
);

#endif // LE_RTC_COMMON_H_INCLUDE_GUARD