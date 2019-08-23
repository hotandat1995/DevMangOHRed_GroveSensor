#include "legato.h"
#include "interfaces.h"

#define dhubIO_DataType_t io_DataType_t

static bool IsEnabled = false;

static le_timer_Ref_t Timer;

#define LATITUDE_NAME "location/value/latitude"
#define LONGITUDE_NAME "location/value/longitude"
#define PERIOD_NAME "location/period"
#define ENABLE_NAME "location/enable"

static le_pos_MovementHandlerRef_t  NavigationHandlerRef;

//--------------------------------------------------------------------------------------------------
/**
 * Call-back function called when an update is received from the Data Hub for the "period"
 * config setting.
 */
//--------------------------------------------------------------------------------------------------
static void PeriodUpdateHandler
(
    double timestamp,   ///< time stamp
    double value,       ///< period value, seconds
    void* contextPtr    ///< not used
)
//--------------------------------------------------------------------------------------------------
{
    LE_INFO("Received update to 'period' setting: %lf (timestamped %lf)", value, timestamp);

    uint32_t ms = (uint32_t)(value * 1000);

    if (ms == 0)
    {
        le_timer_Stop(Timer);
    }
    else
    {
        le_timer_SetMsInterval(Timer, ms);

        // If the sensor is enabled and the timer is not already running, start it now.
        if (IsEnabled && (!le_timer_IsRunning(Timer)))
        {
            le_timer_Start(Timer);
        }
    }
}


//--------------------------------------------------------------------------------------------------
/**
 * Call-back function called when an update is received from the Data Hub for the "enable"
 * control.
 */
//--------------------------------------------------------------------------------------------------
static void EnableUpdateHandler
(
    double timestamp,   ///< time stamp
    bool value,         ///< whether input is enabled
    void* contextPtr    ///< not used
)
//--------------------------------------------------------------------------------------------------
{
    LE_INFO("Received update to 'enable' setting: %s (timestamped %lf)",
            value == false ? "false" : "true",
            timestamp);

    IsEnabled = value;

    if (value)
    {
        // If the timer has a non-zero interval and is not already running, start it now.
        if ((le_timer_GetMsInterval(Timer) != 0) && (!le_timer_IsRunning(Timer)))
        {
            le_timer_Start(Timer);
        }
    }
    else
    {
        le_timer_Stop(Timer);
    }
}


//--------------------------------------------------------------------------------------------------
/**
 * Call-back for receiving notification that an update is happening.
 */
//--------------------------------------------------------------------------------------------------
static void UpdateStartEndHandler
(
    bool isStarting,    //< input is starting
    void* contextPtr    //< Not used.
)
//--------------------------------------------------------------------------------------------------
{
    LE_INFO("Configuration update %s.", isStarting ? "starting" : "finished");
}


static void NavigationHandler
(
    le_pos_SampleRef_t positionSampleRef,
    void* contextPtr
)
{
    int32_t  latitude, longitude, accuracy;

    if (NULL == positionSampleRef)
    {
        LE_ERROR("New Position sample is NULL!");
    }
    else
    {
        LE_INFO("New Position sample %p", positionSampleRef);
    }

    le_pos_sample_Get2DLocation(positionSampleRef, &latitude, &longitude, &accuracy);
    
    LE_DEBUG("Get2DLocation: lat.%lf, long.%lf, accuracy.%d", 
                            (float)latitude/1000000, 
                            (float)longitude/1000000, 
                            accuracy);

    // Location units have to be converted from 1e-6 degrees to degrees
    io_PushNumeric(LATITUDE_NAME, IO_NOW, latitude * 0.000001);
    io_PushNumeric(LONGITUDE_NAME, IO_NOW, longitude * 0.000001);

}

COMPONENT_INIT
{
    le_result_t result;
    le_posCtrl_ActivationRef_t      activationRef;

    le_pos_ConnectService();

    io_AddUpdateStartEndHandler(UpdateStartEndHandler, NULL);

    // This will be provided to the Data Hub.
    result = io_CreateInput(LATITUDE_NAME, IO_DATA_TYPE_NUMERIC, "degrees");
    LE_ASSERT(result == LE_OK);
    result = io_CreateInput(LONGITUDE_NAME, IO_DATA_TYPE_NUMERIC, "degrees");
    LE_ASSERT(result == LE_OK);

    // This is my configuration setting.
    result = io_CreateOutput(PERIOD_NAME, IO_DATA_TYPE_NUMERIC, "s");
    LE_ASSERT(result == LE_OK);

    // Register for notification of updates to our configuration setting.
    io_AddNumericPushHandler(PERIOD_NAME, PeriodUpdateHandler, NULL);

    // This is my enable/disable control.
    result = io_CreateOutput(ENABLE_NAME, IO_DATA_TYPE_BOOLEAN, "");
    LE_ASSERT(result == LE_OK);

    // Set the defaults: enable the sensor, set period to 1s
    io_SetBooleanDefault(ENABLE_NAME, true);
    io_SetNumericDefault(PERIOD_NAME, 1);

    // Register for notification of updates to our enable/disable control.
    io_AddBooleanPushHandler(ENABLE_NAME, EnableUpdateHandler, NULL);

    LE_INFO("Request activation of the positioning service");
    LE_ASSERT((activationRef = le_posCtrl_Request()) != NULL);

    // Test the registration of an handler for movement notifications with horizontal or vertical
    // magnitude of 0 meters. (It will set an acquisition rate of 1sec).
    NavigationHandlerRef = le_pos_AddMovementHandler(0, 0, NavigationHandler, NULL);
    LE_ASSERT(NULL != NavigationHandlerRef);
}

