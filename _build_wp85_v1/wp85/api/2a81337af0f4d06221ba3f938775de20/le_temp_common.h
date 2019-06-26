
/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */
#ifndef LE_TEMP_COMMON_H_INCLUDE_GUARD
#define LE_TEMP_COMMON_H_INCLUDE_GUARD


#include "legato.h"

#define IFGEN_LE_TEMP_PROTOCOL_ID "01650568439a3a9c47468069df5a7783"
#define IFGEN_LE_TEMP_MSG_SIZE 120



//--------------------------------------------------------------------------------------------------
/**
 * Maximum sensor's name string length.
 */
//--------------------------------------------------------------------------------------------------
#define LE_TEMP_SENSOR_NAME_MAX_LEN 100

//--------------------------------------------------------------------------------------------------
/**
 * Maximum sensor's name string length.
 * One extra byte is added for the null character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_TEMP_SENSOR_NAME_MAX_BYTES 101

//--------------------------------------------------------------------------------------------------
/**
 * Maximum threshold's name string length.
 */
//--------------------------------------------------------------------------------------------------
#define LE_TEMP_THRESHOLD_NAME_MAX_LEN 100

//--------------------------------------------------------------------------------------------------
/**
 * Maximum threshold's name string length.
 * One extra byte is added for the null character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_TEMP_THRESHOLD_NAME_MAX_BYTES 101

//--------------------------------------------------------------------------------------------------
/**
 *  Reference type for a temperature sensor.
 */
//--------------------------------------------------------------------------------------------------
typedef struct le_temp_Sensor* le_temp_SensorRef_t;


//--------------------------------------------------------------------------------------------------
/**
 * Reference type used by Add/Remove functions for EVENT 'le_temp_ThresholdEvent'
 */
//--------------------------------------------------------------------------------------------------
typedef struct le_temp_ThresholdEventHandler* le_temp_ThresholdEventHandlerRef_t;


//--------------------------------------------------------------------------------------------------
/**
 * Handler for Temperature event.
 */
//--------------------------------------------------------------------------------------------------
typedef void (*le_temp_ThresholdEventHandlerFunc_t)
(
        le_temp_SensorRef_t sensor,
        ///< Temperature sensor reference.
        const char* LE_NONNULL threshold,
        ///< Name of the raised threshold.
        void* contextPtr
        ///<
);


//--------------------------------------------------------------------------------------------------
/**
 * Get if this client bound locally.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED bool ifgen_le_temp_HasLocalBinding
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Init data that is common across all threads
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED void ifgen_le_temp_InitCommonData
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Perform common initialization and open a session
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_temp_OpenSession
(
    le_msg_SessionRef_t _ifgen_sessionRef,
    bool isBlocking
);

//--------------------------------------------------------------------------------------------------
/**
 * Request a temperature sensor reference.
 *
 * @return
 *      - Reference to the temperature sensor.
 *      - NULL when the requested sensor is not supported.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_temp_SensorRef_t ifgen_le_temp_Request
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        const char* LE_NONNULL sensorName
        ///< [IN] Name of the temperature sensor.
);

//--------------------------------------------------------------------------------------------------
/**
 * Retrieve the temperature sensor's name from its reference.
 *
 * @return
 *      - LE_OK            The function succeeded.
 *      - LE_OVERFLOW      The name length exceed the maximum length.
 *      - LE_FAULT         The function failed.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_temp_GetSensorName
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_temp_SensorRef_t sensor,
        ///< [IN] Temperature sensor reference.
        char* sensorName,
        ///< [OUT] Name of the temperature sensor.
        size_t sensorNameSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Add handler function for EVENT 'le_temp_ThresholdEvent'
 *
 * This event provides information on the threshold reached.
 *
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_temp_ThresholdEventHandlerRef_t ifgen_le_temp_AddThresholdEventHandler
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_temp_ThresholdEventHandlerFunc_t ThresholdEventHandlerPtr,
        ///< [IN]
        void* contextPtr
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Remove handler function for EVENT 'le_temp_ThresholdEvent'
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED void ifgen_le_temp_RemoveThresholdEventHandler
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_temp_ThresholdEventHandlerRef_t handlerRef
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the temperature in degree Celsius.
 *
 * @return
 *      - LE_OK            The function succeeded.
 *      - LE_FAULT         The function failed to get the temperature.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_temp_GetTemperature
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_temp_SensorRef_t sensor,
        ///< [IN] Temperature sensor reference.
        int32_t* temperaturePtr
        ///< [OUT] Temperature in degree Celsius.
);

//--------------------------------------------------------------------------------------------------
/**
 * Set the temperature threshold in degree Celsius. This function does not start the temperature
 * monitoring, call le_temp_StartMonitoring() to start it.
 *
 * @return
 *      - LE_OK            The function succeeded.
 *      - LE_FAULT         The function failed.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_temp_SetThreshold
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_temp_SensorRef_t sensor,
        ///< [IN] Temperature sensor reference.
        const char* LE_NONNULL threshold,
        ///< [IN] Name of the threshold.
        int32_t temperature
        ///< [IN] Temperature threshold in degree Celsius.
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the temperature threshold in degree Celsius.
 *
 * @return
 *      - LE_OK            The function succeeded.
 *      - LE_FAULT         The function failed.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_temp_GetThreshold
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_temp_SensorRef_t sensor,
        ///< [IN] Temperature sensor reference.
        const char* LE_NONNULL threshold,
        ///< [IN] Name of the threshold.
        int32_t* temperaturePtr
        ///< [OUT] Temperature threshold in degree Celsius.
);

//--------------------------------------------------------------------------------------------------
/**
 * Start the temperature monitoring with the temperature thresholds configured by
 * le_temp_SetThreshold() function.
 *
 * @return
 *      - LE_OK            The function succeeded.
 *      - LE_BAD_PARAMETER There is a mismatch with the configured threshold values.
 *      - LE_FAULT         The function failed to apply the thresholds.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_temp_StartMonitoring
(
    le_msg_SessionRef_t _ifgen_sessionRef
);

#endif // LE_TEMP_COMMON_H_INCLUDE_GUARD