
/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */
#ifndef LE_ADC_COMMON_H_INCLUDE_GUARD
#define LE_ADC_COMMON_H_INCLUDE_GUARD


#include "legato.h"

#define IFGEN_LE_ADC_PROTOCOL_ID "cca082d3d4d6b9c7c11466ab2ea74e4f"
#define IFGEN_LE_ADC_MSG_SIZE 42



//--------------------------------------------------------------------------------------------------
/**
 * Maximum ADC's name string length.
 */
//--------------------------------------------------------------------------------------------------
#define LE_ADC_ADC_NAME_MAX_LEN 30

//--------------------------------------------------------------------------------------------------
/**
 * Maximum ADC's name string length.
 * One extra byte is added for the null character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_ADC_ADC_NAME_MAX_BYTES 31


//--------------------------------------------------------------------------------------------------
/**
 * Get if this client bound locally.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED bool ifgen_le_adc_HasLocalBinding
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Init data that is common across all threads
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED void ifgen_le_adc_InitCommonData
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Perform common initialization and open a session
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_adc_OpenSession
(
    le_msg_SessionRef_t _ifgen_sessionRef,
    bool isBlocking
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the value of an ADC input
 *
 * @return
 *      - LE_OK            The function succeeded.
 *      - LE_FAULT         The function failed.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_adc_ReadValue
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        const char* LE_NONNULL adcName,
        ///< [IN] Name of the ADC to read.
        int32_t* adcValuePtr
        ///< [OUT] The adc value
);

#endif // LE_ADC_COMMON_H_INCLUDE_GUARD