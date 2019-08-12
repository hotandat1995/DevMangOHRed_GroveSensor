#include "legato.h"
#include "interfaces.h"

#define SENSORNUM_VALUE_NAME "hm3301/value/sensorNum"
#define PM1CF1_VALUE_NAME    "hm3301/value/pm1_cf1"
#define PM25CF1_VALUE_NAME   "hm3301/value/pm25_cf1"
#define PM10CF1_VALUE_NAME   "hm3301/value/pm10_cf1"
#define PM1_VALUE_NAME       "hm3301/value/pm1"
#define PM25_VALUE_NAME      "hm3301/value/pm25"
#define PM10_VALUE_NAME      "hm3301/value/pm10"

#define SENSORNUM_STATE "/app/hm3301/value/channel1"
#define PM1CF1_STATE    "/app/hm3301/value/pm1_cf1"
#define PM25CF1_STATE   "/app/hm3301/value/pm25_cf1"
#define PM10CF1_STATE   "/app/hm3301/value/pm10_cf1"
#define PM1_STATE       "/app/hm3301/value/pm1"
#define PM25_STATE      "/app/hm3301/value/pm25"
#define PM10_STATE      "/app/hm3301/value/pm10"

#define COUNTER_NAME "counter/value"

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

struct HM3301State
{
    double sensorNum;
    // bool channel2State;
    // bool channel3State;
    // bool channel4State;
};

static struct
{
    struct HM3301State recorded; // sensor values most recently recorded
    struct HM3301State read;     // sensor values most recently read
} ChannelData;

//--------------------------------------------------------------------------------------------------
/*
 * static function declarations
 */
//--------------------------------------------------------------------------------------------------

static le_result_t SensorNumRead(void *value);
static bool SensorNumThreshold(const void *recordedValue, const void *readValue);
static le_result_t SensorNumRecord(le_avdata_RecordRef_t ref, uint64_t timestamp, void *value);
static void SensorNumCopyValue(void *dest, const void *src);

struct Item Items[] =
    {
        {
            .name = "Channel 1 state",
            .read = SensorNumRead,
            .thresholdCheck = SensorNumThreshold,
            .record = SensorNumRecord,
            .copyValue = SensorNumCopyValue,
            .lastValueRead = &ChannelData.read.sensorNum,
            .lastValueRecorded = &ChannelData.recorded.sensorNum,
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

static le_result_t SensorNumRead(
    void *value ///< Pointer to the int32_t variable to store the reading in
)
{
    bool *v = value;
    *v = ;//On going
    return LE_OK;
}

static bool SensorNumThreshold(
    const void *recordedValue, ///< Last recorded light sensor reading
    const void *readValue      ///< Most recent light sensor reading
)
{
    const bool *v1 = recordedValue;
    const bool *v2 = readValue;

    return abs(*v1 - *v2) > 200;
}

static le_result_t SensorNumRecord(
    le_avdata_RecordRef_t ref, ///< Record reference to record the value into
    uint64_t timestamp,        ///< Timestamp to associate with the value
    void *value                ///< The int32_t value to record
)
{
    const char *path = "HM3301.HM3301.SensorNum";

    bool *v = value;
    le_result_t result = le_avdata_RecordInt(RecordRef, path, *v, timestamp);
    if (result != LE_OK)
    {
        LE_ERROR("Couldn't record channel 1 status - %s", LE_RESULT_TXT(result));
    }

    return result;
}

static void SensorNumCopyValue(
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
    result = io_CreateInput(SENSORNUM_VALUE_NAME, IO_DATA_TYPE_NUMERIC, "");
    LE_ASSERT(result == LE_OK);
    result = io_CreateInput(PM1CF1_VALUE_NAME,    IO_DATA_TYPE_NUMERIC, "ug/m3");
    LE_ASSERT(result == LE_OK);
    result = io_CreateInput(PM25CF1_VALUE_NAME,   IO_DATA_TYPE_NUMERIC, "ug/m3");
    LE_ASSERT(result == LE_OK);
    result = io_CreateInput(PM10CF1_VALUE_NAME,   IO_DATA_TYPE_NUMERIC, "ug/m3");
    LE_ASSERT(result == LE_OK);
    result = io_CreateInput(PM1_VALUE_NAME,       IO_DATA_TYPE_NUMERIC, "ug/m3");
    LE_ASSERT(result == LE_OK);
    result = io_CreateInput(PM25_VALUE_NAME,      IO_DATA_TYPE_NUMERIC, "ug/m3");
    LE_ASSERT(result == LE_OK);
    result = io_CreateInput(PM10_VALUE_NAME,      IO_DATA_TYPE_NUMERIC, "ug/m3");
    LE_ASSERT(result == LE_OK);

    // Connect to the sensor data stored in DataHub
    result = admin_SetSource("/app/hm3301/" SENSORNUM_VALUE_NAME, SENSORNUM_STATE);
    LE_ASSERT(result == LE_OK);
    

    // AirVantage congfigure
    LE_INFO("Create sample time");
    SampleTimer = le_timer_Create("Read HM3301");
    LE_ASSERT_OK(le_timer_SetMsInterval(SampleTimer, DelayBetweenReadings * 1000));
    LE_ASSERT_OK(le_timer_SetRepeat(SampleTimer, 0));
    LE_ASSERT_OK(le_timer_SetHandler(SampleTimer, SampleTimerHandler));

    LE_INFO("Create record reference");
    RecordRef = le_avdata_CreateRecord();

    LE_INFO("Create session");
    HandlerRef = le_avdata_AddSessionStateHandler(AvSessionStateHandler, NULL);
    AvSession = le_avdata_RequestSession();

    LE_FATAL_IF(AvSession == NULL, "Failed to request avdata session")
}