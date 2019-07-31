
/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */
#ifndef LE_IPS_COMMON_H_INCLUDE_GUARD
#define LE_IPS_COMMON_H_INCLUDE_GUARD


#include "legato.h"

#define IFGEN_LE_IPS_PROTOCOL_ID "90ad953c761aa71f85c44f94ee91aff1"
#define IFGEN_LE_IPS_MSG_SIZE 20



//--------------------------------------------------------------------------------------------------
/**
 * Platform input voltage event type.
 */
//--------------------------------------------------------------------------------------------------
typedef enum
{
    LE_IPS_VOLTAGE_HI_CRITICAL = 0,
        ///< High Critical input voltage threshold is reached.
    LE_IPS_VOLTAGE_NORMAL = 1,
        ///< Normal input voltage threshold is reached.
    LE_IPS_VOLTAGE_WARNING = 2,
        ///< Warning input voltage threshold is reached.
    LE_IPS_VOLTAGE_CRITICAL = 3
        ///< Critical input voltage threshold is reached.
}
le_ips_ThresholdStatus_t;


//--------------------------------------------------------------------------------------------------
/**
 * Platform power source type.
 */
//--------------------------------------------------------------------------------------------------
typedef enum
{
    LE_IPS_POWER_SOURCE_EXTERNAL = 0,
        ///< Platform is powered by an external source.
    LE_IPS_POWER_SOURCE_BATTERY = 1
        ///< Platform is powered by a battery.
}
le_ips_PowerSource_t;


//--------------------------------------------------------------------------------------------------
/**
 * Reference type used by Add/Remove functions for EVENT 'le_ips_ThresholdEvent'
 */
//--------------------------------------------------------------------------------------------------
typedef struct le_ips_ThresholdEventHandler* le_ips_ThresholdEventHandlerRef_t;


//--------------------------------------------------------------------------------------------------
/**
 * Handler for Platform input voltage event.
 */
//--------------------------------------------------------------------------------------------------
typedef void (*le_ips_ThresholdEventHandlerFunc_t)
(
        le_ips_ThresholdStatus_t event,
        ///< Input voltage threshold event reached.
        void* contextPtr
        ///<
);


//--------------------------------------------------------------------------------------------------
/**
 * Get if this client bound locally.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED bool ifgen_le_ips_HasLocalBinding
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Init data that is common across all threads
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED void ifgen_le_ips_InitCommonData
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Perform common initialization and open a session
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_ips_OpenSession
(
    le_msg_SessionRef_t _ifgen_sessionRef,
    bool isBlocking
);

//--------------------------------------------------------------------------------------------------
/**
 * Add handler function for EVENT 'le_ips_ThresholdEvent'
 *
 * This event provides information on Threshold reached.
 *
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_ips_ThresholdEventHandlerRef_t ifgen_le_ips_AddThresholdEventHandler
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_ips_ThresholdEventHandlerFunc_t handlerPtr,
        ///< [IN]
        void* contextPtr
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Remove handler function for EVENT 'le_ips_ThresholdEvent'
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED void ifgen_le_ips_RemoveThresholdEventHandler
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_ips_ThresholdEventHandlerRef_t handlerRef
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the Platform input voltage in [mV].
 *
 * @return
 *      - LE_OK            The function succeeded.
 *      - LE_FAULT         The function failed to get the value.
 *
 * @note If the caller is passing a bad pointer into this function, it is a fatal error, the
 *       function will not return.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_ips_GetInputVoltage
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        uint32_t* inputVoltagePtr
        ///< [OUT] [OUT] The input voltage in [mV]
);

//--------------------------------------------------------------------------------------------------
/**
 * Set the Platform warning and critical input voltage thresholds in [mV].
 *  When thresholds input voltage are reached, a input voltage event is triggered.
 *
 * @return
 *      - LE_OK            The function succeeded.
 *      - LE_BAD_PARAMETER The hiCriticalVolt threshold is equal or lower than the (normalVolt+1)
 *                           threshold.
 *                         The warningVolt threshold is equal to or higher than the normalVolt
 *                           threshold.
 *                         The warningVolt threshold is equal to or lower than the criticalVolt
 *                           threshold.
 *      - LE_FAULT         The function failed to set the thresholds.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_ips_SetVoltageThresholds
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        uint16_t criticalVolt,
        ///< [IN] [IN] The critical input voltage threshold in [mV].
        uint16_t warningVolt,
        ///< [IN] [IN] The warning input voltage threshold in [mV].
        uint16_t normalVolt,
        ///< [IN] [IN] The normal input voltage threshold in [mV].
        uint16_t hiCriticalVolt
        ///< [IN] [IN] The high critical input voltage threshold in [mV].
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the Platform warning and critical input voltage thresholds in [mV].
 *
 * @return
 *      - LE_OK            The function succeeded.
 *      - LE_FAULT         The function failed to get the thresholds.
 *
 * @note If the caller is passing a bad pointer into this function, it is a fatal error, the
 *       function will not return.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_ips_GetVoltageThresholds
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        uint16_t* criticalVoltPtr,
        ///< [OUT] [OUT] The critical input voltage threshold in [mV].
        uint16_t* warningVoltPtr,
        ///< [OUT] [OUT] The warning input voltage threshold in [mV].
        uint16_t* normalVoltPtr,
        ///< [OUT] [OUT] The normal input voltage threshold in [mV].
        uint16_t* hiCriticalVoltPtr
        ///< [OUT] [IN] The high critical input voltage threshold in [mV].
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the Platform power source.
 *
 * @return
 *      - LE_OK            The function succeeded.
 *      - LE_FAULT         The function failed to get the value.
 *
 * @note If the caller is passing a bad pointer into this function, it is a fatal error, the
 *       function will not return.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_ips_GetPowerSource
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_ips_PowerSource_t* powerSourcePtr
        ///< [OUT] [OUT] The power source.
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the Platform battery level in percent:
 *  - 0: battery is exhausted or platform does not have a battery connected
 *  - 1 to 100: percentage of battery capacity remaining
 *
 * @return
 *      - LE_OK            The function succeeded.
 *      - LE_FAULT         The function failed to get the value.
 *
 * @note If the caller is passing a bad pointer into this function, it is a fatal error, the
 *       function will not return.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_ips_GetBatteryLevel
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        uint8_t* batteryLevelPtr
        ///< [OUT] [OUT] The battery level in percent.
);

//--------------------------------------------------------------------------------------------------
/**
 * Set the Platform battery level in percent.
 * This is useful when an external battery is used and its level is provided by the application
 * monitoring it.
 *
 * @note The battery level set through this API will be the value reported by
 * le_ips_GetBatteryLevel() until Legato is restarted.
 *
 * @return
 *      - LE_OK            The function succeeded.
 *      - LE_BAD_PARAMETER Incorrect battery level provided.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_ips_SetBatteryLevel
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        uint8_t batteryLevel
        ///< [IN] [IN] The battery level in percent.
);

#endif // LE_IPS_COMMON_H_INCLUDE_GUARD