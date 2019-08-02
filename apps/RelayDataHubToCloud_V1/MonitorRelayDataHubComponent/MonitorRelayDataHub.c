#include "legato.h"
#include "interfaces.h"

#define CHANNEL_1_NAME "GroveRelayStatus/value/channel1"
#define CHANNEL_2_NAME "GroveRelayStatus/value/channel2"
#define CHANNEL_3_NAME "GroveRelayStatus/value/channel3"
#define CHANNEL_4_NAME "GroveRelayStatus/value/channel4"
#define TEMPERATURE_NAME "GroveRelayStatus/temperature"

#define CHANNEL_1_STATE "/app/RelayDataHub/GroveRelayStatus/value/channel1"
#define CHANNEL_2_STATE "/app/RelayDataHub/GroveRelayStatus/value/channel2"
#define CHANNEL_3_STATE "/app/RelayDataHub/GroveRelayStatus/value/channel3"
#define CHANNEL_4_STATE "/app/RelayDataHub/GroveRelayStatus/value/channel4"
#define TEMPERATURE_SENSOR "/app/RelayDataHub/GroveRelayStatus/temperature"

#define COUNTER_NAME "counter/value"

#define CHANNEL_1_OBS "channel1OffLimits"
#define CHANNEL_2_OBS "channel2OffLimits"
#define CHANNEL_3_OBS "channel3OffLimits"
#define CHANNEL_4_OBS "channel4OffLimits"
#define TEMPERATURE_OBS "temperatureOffLimits"

#define RELAY_CMD_SET_UPPER_CHANNEL1 "channelSetting/UpperChannel1"
#define RELAY_CMD_SET_LOWER_CHANNEL1 "channelSetting/LowerChannel1"

static uint32_t  TEMPERATURE_LOWER_LIMIT = 30;
static uint32_t  TEMPERATURE_UPPER_LIMIT = 32;

struct Item
{
    // A human readable name for the sensor
    const char *name;

    // Reads a value from the sensor
    le_result_t (*read)(void *value);

    // Checks to see if the value read exceeds the threshold relative to the last recorded value. If
    // the function returns true, the readValue will be recorded.
    bool (*thresholdCheck)(const void *recordedValue, const void *readValue);

    // Records the value into the given record.
    le_result_t (*record)(le_avdata_RecordRef_t ref, uint64_t timestamp, void *value);

    // Copies the sensor reading from src to dest. The management of the src and dest data is the
    // responsibility of the caller.
    void (*copyValue)(void *dest, const void *src);

    // Most recently read value from the sensor. Should be initialized to point at a variable large
    // enough to store a sensor reading.
    void *lastValueRead;

    // Most recently recorded value from the sensor. Must be initialized to point to a variable to
    // store a reading and must be a differnt variable from what lastValueRead is pointing at.
    void *lastValueRecorded;

    // Time when the last reading was recorded.
    uint64_t lastTimeRecorded;

    // Time when the last reading was read.
    uint64_t lastTimeRead;
};

struct ChannelState
{
    bool channel1State;
    // bool channel2State;
    // bool channel3State;
    // bool channel4State;
};

static struct
{
    struct ChannelState recorded; // sensor values most recently recorded
    struct ChannelState read;     // sensor values most recently read
} ChannelData;

//--------------------------------------------------------------------------------------------------
/*
 * static function declarations
 */
//--------------------------------------------------------------------------------------------------

static le_result_t Channel1read(void *value);
static bool Channel1Threshold(const void *recordedValue, const void *readValue);
static le_result_t Channel1Record(le_avdata_RecordRef_t ref, uint64_t timestamp, void *value);
static void Channel1CopyValue(void *dest, const void *src);

struct Item Items[] =
    {
        {
            .name = "Channel 1 state",
            .read = Channel1read,
            .thresholdCheck = Channel1Threshold,
            .record = Channel1Record,
            .copyValue = Channel1CopyValue,
            .lastValueRead = &ChannelData.read.channel1State,
            .lastValueRecorded = &ChannelData.recorded.channel1State,
            .lastTimeRead = 0,
            .lastTimeRecorded = 0,
        }};

//--------------------------------------------------------------------------------------------------
/*
 * variable definitions
 */
//--------------------------------------------------------------------------------------------------
static le_avdata_RequestSessionObjRef_t AvSession;
static le_avdata_SessionStateHandlerRef_t HandlerRef;
static le_avdata_RecordRef_t RecordRef;
static le_timer_Ref_t SampleTimer;
static const int DelayBetweenReadings = 1;
static const int MaxIntervalBetweenPublish = 120;
static const int MinIntervalBetweenPublish = 10;
// How old the last published value must be for an item to be considered stale. The next time a
// publish occurs, the most recent reading of all stale items will be published.
static const int TimeToStale = 60;

static bool DeferredPublish = false;
static uint64_t LastTimePublished = 0;

static le_result_t Channel1read(
    void *value ///< Pointer to the int32_t variable to store the reading in
)
{
    bool *v = value;
    *v = (multiChannelRelay_getChannelState() & 0b0001) >> 0;
    return LE_OK;
}

static bool Channel1Threshold(
    const void *recordedValue, ///< Last recorded light sensor reading
    const void *readValue      ///< Most recent light sensor reading
)
{
    const bool *v1 = recordedValue;
    const bool *v2 = readValue;

    return abs(*v1 - *v2) > 200;
}

static le_result_t Channel1Record(
    le_avdata_RecordRef_t ref, ///< Record reference to record the value into
    uint64_t timestamp,        ///< Timestamp to associate with the value
    void *value                ///< The int32_t value to record
)
{
    const char *path = "MonitorRelayDataHub.MonitorRelayDataHub.Channel1Status";

    bool *v = value;
    le_result_t result = le_avdata_RecordInt(RecordRef, path, *v, timestamp);
    if (result != LE_OK)
    {
        LE_ERROR("Couldn't record channel 1 status - %s", LE_RESULT_TXT(result));
    }

    return result;
}

static void Channel1CopyValue(
    void *dest,     ///< copy destination
    const void *src ///< copy source
)
{
    bool *d = dest;
    const bool *s = src;
    *d = *s;
}

//--------------------------------------------------------------------------------------------------
/**
 * Call-back function called when an update is received from the Data Hub for the latitude value.
 */
//--------------------------------------------------------------------------------------------------
static void Channel1UpdateHandler(
    double timestamp, ///< time stamp
    bool value,       ///< latitude value, degrees
    void *contextPtr  ///< not used
)
{
    LE_DEBUG("Channel1 = %d (timestamped %lf)", value, timestamp);
}

static void Channel2UpdateHandler(
    double timestamp, ///< time stamp
    bool value,       ///< latitude value, degrees
    void *contextPtr  ///< not used
)
{
    LE_DEBUG("Channel2 = %d (timestamped %lf)", value, timestamp);
}

static void Channel3UpdateHandler(
    double timestamp, ///< time stamp
    bool value,       ///< latitude value, degrees
    void *contextPtr  ///< not used
)
{
    LE_DEBUG("Channel3 = %d (timestamped %lf)", value, timestamp);
}
static void Channel4UpdateHandler(
    double timestamp, ///< time stamp
    bool value,       ///< latitude value, degrees
    void *contextPtr  ///< not used
)
{
    LE_DEBUG("Channel4 = %d (timestamped %lf)", value, timestamp);
}

static void TemperatureUpdateHanler(
    double timestamp, ///< time stamp
    double value,     ///< latitude value, degrees
    void *contextPtr  ///< not used
)
{
    LE_DEBUG("Observed filtered Temperature = %lf (timestamped %lf)", value, timestamp);
}
//--------------------------------------------------------------------------------------------------
/**
 * Call-back function called when an update is received from the Data Hub for the latitude value.
 */
//--------------------------------------------------------------------------------------------------
static void Channel1ObservationUpdateHandler(
    double timestamp, ///< time stamp
    bool value,       ///< latitude value, degrees
    void *contextPtr  ///< not used00
)
{
    LE_DEBUG("Observed filtered channel 1 = %d (timestamped %lf)", value, timestamp);
}

static void Channel2ObservationUpdateHandler(
    double timestamp, ///< time stamp
    bool value,       ///< latitude value, degrees
    void *contextPtr  ///< not used
)
{
    LE_DEBUG("Observed filtered channel 2 = %d (timestamped %lf)", value, timestamp);
}

static void Channel3ObservationUpdateHandler(
    double timestamp, ///< time stamp
    bool value,       ///< latitude value, degrees
    void *contextPtr  ///< not used
)
{
    LE_DEBUG("Observed filtered channel 3 = %d (timestamped %lf)", value, timestamp);
}

static void Channel4ObservationUpdateHandler(
    double timestamp, ///< time stamp
    bool value,       ///< latitude value, degrees
    void *contextPtr  ///< not used
)
{
    LE_DEBUG("Observed filtered channel 4 = %d (timestamped %lf)", value, timestamp);
}

static void TemperatureObservationUpdateHandler(
    double timestamp, ///< time stamp
    double value,     ///< latitude value, degrees
    void *contextPtr  ///< not used
)
{
    multiChannelRelay_turn_on_channel(1);
    sleep(1);
    multiChannelRelay_turn_on_channel(0);
    LE_DEBUG("Observed filtered Temperature = %lf (timestamped %lf)", value, timestamp);
}

//--------------------------------------------------------------------------------------------------
/**
 * Call-back function called when an update is received from the AirVantage to change Lower bound
 */
//--------------------------------------------------------------------------------------------------
static void LowerChannel1Setting(
    const char *path,
    le_avdata_AccessType_t accessType,
    le_avdata_ArgumentListRef_t argumentList,
    void *contextPtr)
{
    LE_INFO("Change Lower bound");
    le_result_t result = LE_OK;
    int32_t newBound = 0;

    LE_INFO("Set value");
    result = le_avdata_GetInt(RELAY_CMD_SET_LOWER_CHANNEL1,&newBound);
    if (result != LE_OK)
    {
        LE_ERROR("le_avdata_GetStringArg('%s') failed(%d)", RELAY_CMD_SET_LOWER_CHANNEL1, result);
    }

    le_result_t resultSetting = le_avdata_SetInt(RELAY_CMD_SET_LOWER_CHANNEL1,newBound);
    if (LE_FAULT == resultSetting)
    {
        LE_ERROR("Error in setting RELAY_CMD_SET_LOWER_CHANNEL1");
    }

    LE_INFO("value('%d')", newBound);
    admin_SetLowLimit(TEMPERATURE_OBS, (double)newBound);
}
//--------------------------------------------------------------------------------------------------
/**
 * Call-back function called when an update is received from the AirVantage to change Upper bound
 */
//--------------------------------------------------------------------------------------------------
static void UpperChannel1Setting(
    const char *path,
    le_avdata_AccessType_t accessType,
    le_avdata_ArgumentListRef_t argumentList,
    void *contextPtr)
{
    LE_INFO("Change Lower bound");
    le_result_t result = LE_OK;
    int32_t newBound = 0;

    LE_INFO("Set value");
    result = le_avdata_GetInt(RELAY_CMD_SET_UPPER_CHANNEL1,&newBound);
    if (result != LE_OK)
    {
        LE_ERROR("le_avdata_GetStringArg('%s') failed(%d)", RELAY_CMD_SET_UPPER_CHANNEL1, result);
    }
    le_result_t resultSetting = le_avdata_SetInt(RELAY_CMD_SET_UPPER_CHANNEL1,newBound);
    if (LE_FAULT == resultSetting)
    {
        LE_ERROR("Error in setting RELAY_CMD_SET_UPPER_CHANNEL1");
    }
    LE_INFO("value('%d')", newBound);
    admin_SetHighLimit(TEMPERATURE_OBS, (double)newBound);
}

//--------------------------------------------------------------------------------------------------
/**
 * Handle changes in the AirVantage session state
 *
 * When the session is started the timer to sample the sensors is started and when the session is
 * stopped so is the timer.
 */
//--------------------------------------------------------------------------------------------------
static void AvSessionStateHandler(
    le_avdata_SessionState_t state,
    void *context)
{
    switch (state)
    {
    case LE_AVDATA_SESSION_STARTED:
    {
        // TODO: checking for LE_BUSY is a temporary workaround for the session state problem
        // described below.
        LE_DEBUG("Session Started");
        le_result_t status = le_timer_Start(SampleTimer);
        if (status == LE_BUSY)
        {
            LE_INFO("Received session started when timer was already running");
        }
        else
        {
            LE_ASSERT_OK(status);
        }
        break;
    }

    case LE_AVDATA_SESSION_STOPPED:
    {
        LE_DEBUG("Session Stopped");
        le_result_t status = le_timer_Stop(SampleTimer);
        if (status != LE_OK)
        {
            LE_DEBUG("Record push timer not running");
        }

        break;
    }

    default:
        LE_ERROR("Unsupported AV session state %d", state);
        break;
    }
}

//--------------------------------------------------------------------------------------------------
/**
 * Handles notification of LWM2M push status.
 *
 * This function will warn if there is an error in pushing data, but it does not make any attempt to
 * retry pushing the data.
 */
//--------------------------------------------------------------------------------------------------
static void PushCallbackHandler(
    le_avdata_PushStatus_t status, ///< Push success/failure status
    void *context                  ///< Not used
)
{
    switch (status)
    {
    case LE_AVDATA_PUSH_SUCCESS:
        // data pushed successfully
        //LE_INFO("Data push successfully");
        break;

    case LE_AVDATA_PUSH_FAILED:
        LE_WARN("Push was not successful");
        break;

    default:
        LE_ERROR("Unhandled push status %d", status);
        break;
    }
}
//--------------------------------------------------------------------------------------------------
/**
 * Convenience function to get current time as uint64_t.
 *
 * @return
 *      Current time as a uint64_t
 */
//--------------------------------------------------------------------------------------------------
static uint64_t GetCurrentTimestamp(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    uint64_t utcMilliSec = (uint64_t)(tv.tv_sec) * 1000 + (uint64_t)(tv.tv_usec) / 1000;
    return utcMilliSec;
}

//--------------------------------------------------------------------------------------------------
/**
 * Handler for the sensor sampling timer
 *
 * Each time this function is called due to timer expiry each sensor described in the Items array
 * will be read. If any sensor item's thresholdCheck() function returns true, then that reading is
 * recorded and a publish action is scheduled. The data will be published immediately unless fewer
 * than MinIntervalBetweenPublish seconds have elapsed since the last publish. If that is the case,
 * the publish will be deferred until the minimum wait has elapsed. If no publish has occurred for
 * MaxIntervalBetweenPublish seconds, then a publish is forced. When a push is about to be executed
 * the list of items is checked again for any entries which have not been recorded in greater than
 * TimeToStale seconds. Stale items are recorded and then the record is published.
 */
//--------------------------------------------------------------------------------------------------
static void SampleTimerHandler(
    le_timer_Ref_t timer ///< Sensor sampling timer
)
{
    uint64_t now = GetCurrentTimestamp();
    bool publish = false;

    for (int i = 0; i < NUM_ARRAY_MEMBERS(Items); i++)
    {
        le_result_t r;
        struct Item *it = &Items[i];
        r = it->read(it->lastValueRead);
        if (r == LE_OK)
        {
            it->lastTimeRead = now;
            if (it->lastTimeRecorded == 0 || it->thresholdCheck(it->lastValueRead, it->lastValueRecorded))
            {
                r = it->record(RecordRef, now, it->lastValueRead);
                if (r == LE_OK)
                {
                    it->copyValue(it->lastValueRecorded, it->lastValueRead);
                    publish = true;
                }
                else
                {
                    LE_WARN("Failed to record %s", it->name);
                }
            }
        }
        else
        {
            LE_WARN("Failed to read %s", it->name);
        }

        if ((now - it->lastTimeRecorded) > (MaxIntervalBetweenPublish * 1000) &&
            it->lastTimeRead > LastTimePublished)
        {
            publish = true;
        }
    }

    if (publish || DeferredPublish)
    {
        if ((now - LastTimePublished) < MinIntervalBetweenPublish)
        {
            DeferredPublish = true;
        }
        else
        {
            //Find all of the stale items and record their current reading
            for (int i = 0; i < NUM_ARRAY_MEMBERS(Items); i++)
            {
                struct Item *it = &Items[i];
                if ((now - it->lastTimeRecorded) > (TimeToStale * 1000) &&
                    it->lastTimeRead > it->lastTimeRecorded)
                {
                    le_result_t r = it->record(RecordRef, it->lastTimeRead, it->lastValueRead);
                    if (r == LE_OK)
                    {
                        it->copyValue(it->lastValueRecorded, it->lastValueRead);
                        it->lastTimeRecorded = it->lastTimeRead;
                    }
                    else
                    {
                        LE_WARN("Failed to record %s", it->name);
                    }
                }
            }

            le_result_t r = le_avdata_PushRecord(RecordRef, PushCallbackHandler, NULL);
            if (r != LE_OK)
            {
                LE_ERROR("Failed to push record - %s", LE_RESULT_TXT(r));
            }
            else
            {
                LastTimePublished = now;
                DeferredPublish = false;
            }
        }
    }
}

COMPONENT_INIT
{
    le_result_t result;

    // This will be received from the Data Hub.
    // This will be provided to the Data Hub.
    result = io_CreateOutput(CHANNEL_1_NAME, IO_DATA_TYPE_BOOLEAN, "State");
    LE_ASSERT(result == LE_OK);
    result = io_CreateOutput(CHANNEL_2_NAME, IO_DATA_TYPE_BOOLEAN, "State");
    LE_ASSERT(result == LE_OK);
    result = io_CreateOutput(CHANNEL_3_NAME, IO_DATA_TYPE_BOOLEAN, "State");
    LE_ASSERT(result == LE_OK);
    result = io_CreateOutput(CHANNEL_4_NAME, IO_DATA_TYPE_BOOLEAN, "State");
    LE_ASSERT(result == LE_OK);

    result = io_CreateOutput(TEMPERATURE_NAME, IO_DATA_TYPE_NUMERIC, "Celsius degree");
    LE_ASSERT(result == LE_OK);

    // Register for notification of updates to the counter value.
    io_AddBooleanPushHandler(CHANNEL_1_NAME, Channel1UpdateHandler, NULL);
    io_AddBooleanPushHandler(CHANNEL_2_NAME, Channel2UpdateHandler, NULL);
    io_AddBooleanPushHandler(CHANNEL_3_NAME, Channel3UpdateHandler, NULL);
    io_AddBooleanPushHandler(CHANNEL_4_NAME, Channel4UpdateHandler, NULL);
    io_AddNumericPushHandler(TEMPERATURE_NAME, TemperatureUpdateHanler, NULL);

    // Connect to the sensor
    result = admin_SetSource("/app/MonitorRelayDataHub/" CHANNEL_1_NAME, CHANNEL_1_STATE);
    LE_ASSERT(result == LE_OK);
    result = admin_SetSource("/app/MonitorRelayDataHub/" CHANNEL_2_NAME, CHANNEL_2_STATE);
    LE_ASSERT(result == LE_OK);
    result = admin_SetSource("/app/MonitorRelayDataHub/" CHANNEL_3_NAME, CHANNEL_3_STATE);
    LE_ASSERT(result == LE_OK);
    result = admin_SetSource("/app/MonitorRelayDataHub/" CHANNEL_4_NAME, CHANNEL_4_STATE);
    LE_ASSERT(result == LE_OK);
    result = admin_SetSource("/app/MonitorRelayDataHub/" TEMPERATURE_NAME, TEMPERATURE_SENSOR);
    LE_ASSERT(result == LE_OK);

    // Create observation (filter) for Channel state. To set up the "dead band" filter,
    // lower limit assigned to high limit and vice versa (see admin.io doc for details).
    admin_CreateObs(CHANNEL_1_OBS);
    result = admin_SetSource("/obs/" CHANNEL_1_OBS, CHANNEL_1_STATE);
    LE_ASSERT(result == LE_OK);
    admin_AddBooleanPushHandler("/obs/" CHANNEL_1_OBS, Channel1ObservationUpdateHandler, NULL);

    admin_CreateObs(CHANNEL_2_OBS);
    result = admin_SetSource("/obs/" CHANNEL_2_OBS, CHANNEL_2_STATE);
    LE_ASSERT(result == LE_OK);
    admin_AddBooleanPushHandler("/obs/" CHANNEL_2_OBS, Channel2ObservationUpdateHandler, NULL);

    admin_CreateObs(CHANNEL_3_OBS);
    result = admin_SetSource("/obs/" CHANNEL_3_OBS, CHANNEL_3_STATE);
    LE_ASSERT(result == LE_OK);
    admin_AddBooleanPushHandler("/obs/" CHANNEL_3_OBS, Channel3ObservationUpdateHandler, NULL);

    admin_CreateObs(CHANNEL_4_OBS);
    result = admin_SetSource("/obs/" CHANNEL_4_OBS, CHANNEL_1_STATE);
    LE_ASSERT(result == LE_OK);
    admin_AddBooleanPushHandler("/obs/" CHANNEL_4_OBS, Channel4ObservationUpdateHandler, NULL);

    admin_CreateObs(TEMPERATURE_OBS);
    result = admin_SetSource("/obs/" TEMPERATURE_OBS, TEMPERATURE_SENSOR);
    admin_SetLowLimit(TEMPERATURE_OBS, TEMPERATURE_LOWER_LIMIT);
    admin_SetHighLimit(TEMPERATURE_OBS, TEMPERATURE_UPPER_LIMIT);
    LE_ASSERT(result == LE_OK);
    admin_AddNumericPushHandler("/obs/" TEMPERATURE_OBS, TemperatureObservationUpdateHandler, NULL);
    LE_ASSERT(result == LE_OK);

    // AirVantage congfigure
    LE_INFO("Create sameple time");
    SampleTimer = le_timer_Create("Read channel");
    LE_ASSERT_OK(le_timer_SetMsInterval(SampleTimer, DelayBetweenReadings * 1000));
    LE_ASSERT_OK(le_timer_SetRepeat(SampleTimer, 0));
    LE_ASSERT_OK(le_timer_SetHandler(SampleTimer, SampleTimerHandler));

    LE_INFO("Create record reference");
    RecordRef = le_avdata_CreateRecord();

    le_avdata_CreateResource(RELAY_CMD_SET_UPPER_CHANNEL1, LE_AVDATA_ACCESS_SETTING);
    le_avdata_AddResourceEventHandler(RELAY_CMD_SET_UPPER_CHANNEL1, UpperChannel1Setting, NULL);

    le_avdata_CreateResource(RELAY_CMD_SET_LOWER_CHANNEL1, LE_AVDATA_ACCESS_SETTING);
    le_avdata_AddResourceEventHandler(RELAY_CMD_SET_LOWER_CHANNEL1, LowerChannel1Setting, NULL);

    LE_INFO("Create session");
    HandlerRef = le_avdata_AddSessionStateHandler(AvSessionStateHandler, NULL);
    AvSession = le_avdata_RequestSession();

    LE_FATAL_IF(AvSession == NULL, "Failed to request avdata session")
}