#include "legato.h"
#include "interfaces.h"

#define dhubIO_DataType_t io_DataType_t

static bool IsEnabled = false;

static le_timer_Ref_t Timer;

#define CHANNEL_1_NAME "GroveRelayStatus/value/channel1"
#define CHANNEL_2_NAME "GroveRelayStatus/value/channel2"
#define CHANNEL_3_NAME "GroveRelayStatus/value/channel3"
#define CHANNEL_4_NAME "GroveRelayStatus/value/channel4"

#define TEMPERATURE_NAME "GroveRelayStatus/temperature"
#define PERIOD_NAME "GroveRelayStatus/period"
#define ENABLE_NAME "GroveRelayStatus/enable"

static uint8_t Channel_Status;
static uint8_t test_temperature = 30;

//--------------------------------------------------------------------------------------------------
/**
 * Function called when the timer expires.
 */
//--------------------------------------------------------------------------------------------------
static void TimerExpired(
    le_timer_Ref_t timer ///< Timer reference
)
//--------------------------------------------------------------------------------------------------
{
    static unsigned int counter = 0;
    counter += 1;
    // static uint8_t counterled = 0;
    // if(counterled > 15) counterled = 0;
    // multiChannelRelay_turn_on_channel(counterled++);
    test_temperature++;
    if (test_temperature > 38)
        test_temperature = 30;
    Channel_Status = multiChannelRelay_getChannelState();
    //LE_INFO("Relay status: %d, temperature: %d",Channel_Status,test_temperature);

    io_PushNumeric(TEMPERATURE_NAME, IO_NOW, (double_t)test_temperature);

    io_PushBoolean(CHANNEL_1_NAME, IO_NOW, (bool)(Channel_Status & 0b0001) >> 0);
    io_PushBoolean(CHANNEL_2_NAME, IO_NOW, (bool)(Channel_Status & 0b0010) >> 1);
    io_PushBoolean(CHANNEL_3_NAME, IO_NOW, (bool)(Channel_Status & 0b0100) >> 2);
    io_PushBoolean(CHANNEL_4_NAME, IO_NOW, (bool)(Channel_Status & 0b1000) >> 3);
}

//--------------------------------------------------------------------------------------------------
/**
 * Call-back function called when an update is received from the Data Hub for the "period"
 * config setting.
 */
//--------------------------------------------------------------------------------------------------
static void PeriodUpdateHandler(
    double timestamp, ///< time stamp
    double value,     ///< period value, seconds
    void *contextPtr  ///< not used
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
static void EnableUpdateHandler(
    double timestamp, ///< time stamp
    bool value,       ///< whether input is enabled
    void *contextPtr  ///< not used
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
static void UpdateStartEndHandler(
    bool isStarting, //< input is starting
    void *contextPtr //< Not used.
)
//--------------------------------------------------------------------------------------------------
{
    LE_INFO("Configuration update %s.", isStarting ? "starting" : "finished");
}

COMPONENT_INIT
{
    le_result_t result;

    io_AddUpdateStartEndHandler(UpdateStartEndHandler, NULL);

    // This will be provided to the Data Hub.
    result = io_CreateInput(CHANNEL_1_NAME, IO_DATA_TYPE_BOOLEAN, "State");
    LE_ASSERT(result == LE_OK);
    result = io_CreateInput(CHANNEL_2_NAME, IO_DATA_TYPE_BOOLEAN, "State");
    LE_ASSERT(result == LE_OK);
    result = io_CreateInput(CHANNEL_3_NAME, IO_DATA_TYPE_BOOLEAN, "State");
    LE_ASSERT(result == LE_OK);
    result = io_CreateInput(CHANNEL_4_NAME, IO_DATA_TYPE_BOOLEAN, "State");
    LE_ASSERT(result == LE_OK);
    //Add temparature hard code
    result = io_CreateInput(TEMPERATURE_NAME, IO_DATA_TYPE_NUMERIC, "Celsius degree");
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
    io_SetNumericDefault(PERIOD_NAME, 2);

    // Register for notification of updates to our enable/disable control.
    io_AddBooleanPushHandler(ENABLE_NAME, EnableUpdateHandler, NULL);

    // Create a repeating timer that will call TimerExpired() each time it expires.
    // Note: we'll start the timer when we receive our configuration setting.
    Timer = le_timer_Create("RelayTimer");
    le_timer_SetRepeat(Timer, 0);
    le_timer_SetHandler(Timer, TimerExpired);
}
