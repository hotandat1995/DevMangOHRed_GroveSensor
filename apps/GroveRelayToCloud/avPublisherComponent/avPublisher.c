#include "legato.h"
#include "interfaces.h"

//--------------------------------------------------------------------------------------------------
/*
 * command definitions
 */
//--------------------------------------------------------------------------------------------------

// command to set value
#define RELAY_CMD_SET_STATE_CHANNEL1    "/SwitchChannel1"
#define RELAY_CMD_SET_STATE_CHANNEL2    "/SwitchChannel2"
#define RELAY_CMD_SET_STATE_CHANNEL3    "/SwitchChannel3"
#define RELAY_CMD_SET_STATE_CHANNEL4    "/SwitchChannel4"

#define CHANNLE1_BIT  0x01
#define CHANNLE2_BIT  0x02
#define CHANNLE3_BIT  0x04
#define CHANNLE4_BIT  0x08

//--------------------------------------------------------------------------------------------------
/**
 * An abstract representation of a sensor
 *
 * Values are represented as void* because different sensors may produce double, uint32_t or a
 * custom struct type. The only requirement is that all of the functions must expect the same type
 * of value.
 */
//--------------------------------------------------------------------------------------------------
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
//--------------------------------------------------------------------------------------------------
/**
 * 3D acceleration value.
 */
//--------------------------------------------------------------------------------------------------
struct ChannelState
{
    bool channel1State;
    bool channel2State;
    bool channel3State;
    bool channel4State;
};

//--------------------------------------------------------------------------------------------------
/*
 * static function declarations
 */
//--------------------------------------------------------------------------------------------------
static void PushCallbackHandler(le_avdata_PushStatus_t status, void* context);
static uint64_t GetCurrentTimestamp(void);
static void SampleTimerHandler(le_timer_Ref_t timer);

static le_result_t Channel1read(void *value);
static bool Channel1Threshold(const void *recordedValue, const void* readValue);
static le_result_t Channel1Record(le_avdata_RecordRef_t ref, uint64_t timestamp, void *value);
static void Channel1CopyValue(void *dest, const void *src);

static le_result_t Channel2read(void *value);
static bool Channel2Threshold(const void *recordedValue, const void* readValue);
static le_result_t Channel2Record(le_avdata_RecordRef_t ref, uint64_t timestamp, void *value);
static void Channel2CopyValue(void *dest, const void *src);

static le_result_t Channel3read(void *value);
static bool Channel3Threshold(const void *recordedValue, const void* readValue);
static le_result_t Channel3Record(le_avdata_RecordRef_t ref, uint64_t timestamp, void *value);
static void Channel3CopyValue(void *dest, const void *src);

static le_result_t Channel4read(void *value);
static bool Channel4Threshold(const void *recordedValue, const void* readValue);
static le_result_t Channel4Record(le_avdata_RecordRef_t ref, uint64_t timestamp, void *value);
static void Channel4CopyValue(void *dest, const void *src);

static void AvSessionStateHandler (le_avdata_SessionState_t state, void *context);
//--------------------------------------------------------------------------------------------------
/*
 * Data storage for sensor readings.
 *
 * This struct contains the most recently read values from the sensors and the most recently
 * recorded values from the Multi-channel relay.
 */
//--------------------------------------------------------------------------------------------------
static struct
{
    struct ChannelState recorded;  // sensor values most recently recorded
    struct ChannelState read;      // sensor values most recently read
} ChannelData;
//--------------------------------------------------------------------------------------------------
/**
 * An array representing all of the sensor values to read and publish
 */
//--------------------------------------------------------------------------------------------------
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
    },
    {
        .name = "Channel 2 state",
        .read = Channel2read,
        .thresholdCheck = Channel2Threshold,
        .record = Channel2Record,
        .copyValue = Channel2CopyValue,
        .lastValueRead = &ChannelData.read.channel2State,
        .lastValueRecorded = &ChannelData.recorded.channel2State,
        .lastTimeRead = 0,
        .lastTimeRecorded = 0,
    },
    {
        .name = "Channel 3 state",
        .read = Channel3read,
        .thresholdCheck = Channel3Threshold,
        .record = Channel3Record,
        .copyValue = Channel3CopyValue,
        .lastValueRead = &ChannelData.read.channel3State,
        .lastValueRecorded = &ChannelData.recorded.channel3State,
        .lastTimeRead = 0,
        .lastTimeRecorded = 0,
    },
    {
        .name = "Channel 4 state",
        .read = Channel4read,
        .thresholdCheck = Channel4Threshold,
        .record = Channel4Record,
        .copyValue = Channel4CopyValue,
        .lastValueRead = &ChannelData.read.channel4State,
        .lastValueRecorded = &ChannelData.recorded.channel4State,
        .lastTimeRead = 0,
        .lastTimeRecorded = 0,
    }

};
//--------------------------------------------------------------------------------------------------
/*
 * variable definitions
 */
//--------------------------------------------------------------------------------------------------

// Current Channel status
static uint8_t channelStatus;
// Wait time between each round of sensor readings.
static const int DelayBetweenReadings = 1;

// The maximum amount of time to wait for a reading to exceed a threshold before a publish is
// forced.
static const int MaxIntervalBetweenPublish = 120;

// The minimum amount of time to wait between publishing data.
static const int MinIntervalBetweenPublish = 10;

// How old the last published value must be for an item to be considered stale. The next time a
// publish occurs, the most recent reading of all stale items will be published.
static const int TimeToStale = 60;

static le_timer_Ref_t SampleTimer;
static le_avdata_RequestSessionObjRef_t AvSession;
static le_avdata_RecordRef_t RecordRef;
static le_avdata_SessionStateHandlerRef_t HandlerRef;

static bool DeferredPublish = false;
static uint64_t LastTimePublished = 0;

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
static void SampleTimerHandler
(
    le_timer_Ref_t timer  ///< Sensor sampling timer
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
            // Find all of the stale items and record their current reading
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
//--------------------------------------------------------------------------------------------------
/**
 * Read the light sensor
 *
 * @return
 *      LE_OK on success.  Any other return value is a failure.
 */
//--------------------------------------------------------------------------------------------------
static le_result_t Channel1read
(
    void *value  ///< Pointer to the int32_t variable to store the reading in
)
{
    bool *v = value;
    *v = (multiChannelRelay_getChannelState() & 0b0001)>>0;
    return LE_OK;
}


//--------------------------------------------------------------------------------------------------
/**
 * Checks to see if the light level has changed sufficiently to warrant recording of a new reading.
 *
 * @return
 *      true if the threshold for recording has been exceeded
 */
//--------------------------------------------------------------------------------------------------
static bool Channel1Threshold
(
    const void *recordedValue, ///< Last recorded light sensor reading
    const void *readValue      ///< Most recent light sensor reading
)
{
    const bool *v1 = recordedValue;
    const bool *v2 = readValue;

    return abs(*v1 - *v2) > 200;
}

//--------------------------------------------------------------------------------------------------
/**
 * Records a light sensor reading at the given time into the given record
 *
 * @return
 *      - LE_OK on success
 *      - LE_OVERFLOW if the record is full
 *      - LE_FAULT non-specific failure
 */
//--------------------------------------------------------------------------------------------------
static le_result_t Channel1Record
(
    le_avdata_RecordRef_t ref, ///< Record reference to record the value into
    uint64_t timestamp,        ///< Timestamp to associate with the value
    void *value                ///< The int32_t value to record
)
{
    const char *path = "MangOH.GroveRelayToCloud.Channel1status";

    bool *v = value;
    le_result_t result = le_avdata_RecordInt(RecordRef, path, *v, timestamp);
    if (result != LE_OK)
    {
        LE_ERROR("Couldn't record channel 1 status - %s", LE_RESULT_TXT(result));
    }

    return result;
}

//--------------------------------------------------------------------------------------------------
/**
 * Copies an int32_t light sensor reading between two void pointers
 */
//--------------------------------------------------------------------------------------------------
static void Channel1CopyValue
(
    void *dest,      ///< copy destination
    const void *src  ///< copy source
)
{
    bool *d = dest;
    const bool *s = src;
    *d = *s;
}

//--------------------------------------------------------------------------------------------------
/**
 * Read the light sensor
 *
 * @return
 *      LE_OK on success.  Any other return value is a failure.
 */
//--------------------------------------------------------------------------------------------------
static le_result_t Channel2read
(
    void *value  ///< Pointer to the int32_t variable to store the reading in
)
{
    bool *v = value;
    *v = (multiChannelRelay_getChannelState() & 0b0010)>>1;
    return LE_OK;
}


//--------------------------------------------------------------------------------------------------
/**
 * Checks to see if the light level has changed sufficiently to warrant recording of a new reading.
 *
 * @return
 *      true if the threshold for recording has been exceeded
 */
//--------------------------------------------------------------------------------------------------
static bool Channel2Threshold
(
    const void *recordedValue, ///< Last recorded light sensor reading
    const void *readValue      ///< Most recent light sensor reading
)
{
    const bool *v1 = recordedValue;
    const bool *v2 = readValue;

    return abs(*v1 - *v2) > 200;
}

//--------------------------------------------------------------------------------------------------
/**
 * Records a light sensor reading at the given time into the given record
 *
 * @return
 *      - LE_OK on success
 *      - LE_OVERFLOW if the record is full
 *      - LE_FAULT non-specific failure
 */
//--------------------------------------------------------------------------------------------------
static le_result_t Channel2Record
(
    le_avdata_RecordRef_t ref, ///< Record reference to record the value into
    uint64_t timestamp,        ///< Timestamp to associate with the value
    void *value                ///< The int32_t value to record
)
{
    const char *path = "MangOH.GroveRelayToCloud.Channel2status";

    bool *v = value;
    le_result_t result = le_avdata_RecordInt(RecordRef, path, *v, timestamp);
    if (result != LE_OK)
    {
        LE_ERROR("Couldn't record channel 2 status - %s", LE_RESULT_TXT(result));
    }

    return result;
}

//--------------------------------------------------------------------------------------------------
/**
 * Copies an int32_t light sensor reading between two void pointers
 */
//--------------------------------------------------------------------------------------------------
static void Channel2CopyValue
(
    void *dest,      ///< copy destination
    const void *src  ///< copy source
)
{
    bool *d = dest;
    const bool *s = src;
    *d = *s;
}


//--------------------------------------------------------------------------------------------------
/**
 * Read the light sensor
 *
 * @return
 *      LE_OK on success.  Any other return value is a failure.
 */
//--------------------------------------------------------------------------------------------------
static le_result_t Channel3read
(
    void *value  ///< Pointer to the int32_t variable to store the reading in
)
{
    bool *v = value;
    *v = (multiChannelRelay_getChannelState() & 0b0100)>>2;
    return LE_OK;
}


//--------------------------------------------------------------------------------------------------
/**
 * Checks to see if the light level has changed sufficiently to warrant recording of a new reading.
 *
 * @return
 *      true if the threshold for recording has been exceeded
 */
//--------------------------------------------------------------------------------------------------
static bool Channel3Threshold
(
    const void *recordedValue, ///< Last recorded light sensor reading
    const void *readValue      ///< Most recent light sensor reading
)
{
    const bool *v1 = recordedValue;
    const bool *v2 = readValue;

    return abs(*v1 - *v2) > 200;
}

//--------------------------------------------------------------------------------------------------
/**
 * Records a light sensor reading at the given time into the given record
 *
 * @return
 *      - LE_OK on success
 *      - LE_OVERFLOW if the record is full
 *      - LE_FAULT non-specific failure
 */
//--------------------------------------------------------------------------------------------------
static le_result_t Channel3Record
(
    le_avdata_RecordRef_t ref, ///< Record reference to record the value into
    uint64_t timestamp,        ///< Timestamp to associate with the value
    void *value                ///< The int32_t value to record
)
{
    const char *path = "MangOH.GroveRelayToCloud.Channel3status";

    bool *v = value;
    le_result_t result = le_avdata_RecordInt(RecordRef, path, *v, timestamp);
    if (result != LE_OK)
    {
        LE_ERROR("Couldn't record channel 3 status - %s", LE_RESULT_TXT(result));
    }

    return result;
}

//--------------------------------------------------------------------------------------------------
/**
 * Copies an int32_t light sensor reading between two void pointers
 */
//--------------------------------------------------------------------------------------------------
static void Channel3CopyValue
(
    void *dest,      ///< copy destination
    const void *src  ///< copy source
)
{
    bool *d = dest;
    const bool *s = src;
    *d = *s;
}

//--------------------------------------------------------------------------------------------------
/**
 * Read the light sensor
 *
 * @return
 *      LE_OK on success.  Any other return value is a failure.
 */
//--------------------------------------------------------------------------------------------------
static le_result_t Channel4read
(
    void *value  ///< Pointer to the int32_t variable to store the reading in
)
{
    bool *v = value;
    *v = (multiChannelRelay_getChannelState() & 0b1000)>>3;
    return LE_OK;
}


//--------------------------------------------------------------------------------------------------
/**
 * Checks to see if the light level has changed sufficiently to warrant recording of a new reading.
 *
 * @return
 *      true if the threshold for recording has been exceeded
 */
//--------------------------------------------------------------------------------------------------
static bool Channel4Threshold
(
    const void *recordedValue, ///< Last recorded light sensor reading
    const void *readValue      ///< Most recent light sensor reading
)
{
    const bool *v1 = recordedValue;
    const bool *v2 = readValue;

    return abs(*v1 - *v2) > 200;
}

//--------------------------------------------------------------------------------------------------
/**
 * Records a light sensor reading at the given time into the given record
 *
 * @return
 *      - LE_OK on success
 *      - LE_OVERFLOW if the record is full
 *      - LE_FAULT non-specific failure
 */
//--------------------------------------------------------------------------------------------------
static le_result_t Channel4Record
(
    le_avdata_RecordRef_t ref, ///< Record reference to record the value into
    uint64_t timestamp,        ///< Timestamp to associate with the value
    void *value                ///< The int32_t value to record
)
{
    const char *path = "MangOH.GroveRelayToCloud.Channel4status";

    bool *v = value;
    le_result_t result = le_avdata_RecordInt(RecordRef, path, *v, timestamp);
    if (result != LE_OK)
    {
        LE_ERROR("Couldn't record channel 4 status - %s", LE_RESULT_TXT(result));
    }

    return result;
}

//--------------------------------------------------------------------------------------------------
/**
 * Copies an int32_t light sensor reading between two void pointers
 */
//--------------------------------------------------------------------------------------------------
static void Channel4CopyValue
(
    void *dest,      ///< copy destination
    const void *src  ///< copy source
)
{
    bool *d = dest;
    const bool *s = src;
    *d = *s;
}



//--------------------------------------------------------------------------------------------------
/**
 * Handles notification of LWM2M push status.
 *
 * This function will warn if there is an error in pushing data, but it does not make any attempt to
 * retry pushing the data.
 */
//--------------------------------------------------------------------------------------------------
static void PushCallbackHandler
(
    le_avdata_PushStatus_t status, ///< Push success/failure status
    void* context                  ///< Not used
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
 * Command data handler.
 * This function is returned whenever AirVantage performs an execute on the switch Channel 1 command
 */
//--------------------------------------------------------------------------------------------------
static void SwitchChannel1Cmd
(
    const char* path,
    le_avdata_AccessType_t accessType,
    le_avdata_ArgumentListRef_t argumentList,
    void* contextPtr
)
{
    LE_INFO("Change channel 1 status");
    if ((channelStatus & 0b0001) == 0)
    {
        LE_DEBUG("turn on channel");
        channelStatus = channelStatus | 0b0001;
        multiChannelRelay_turn_on_channel(channelStatus);
        
    }
    else
    {
        LE_DEBUG("turn off channel");
        channelStatus = channelStatus & 0b1110;
        multiChannelRelay_turn_on_channel(channelStatus);
    }
    //LE_INFO("Channel status: %d",channelStatus);
    le_avdata_ReplyExecResult(argumentList, LE_OK);
}
//--------------------------------------------------------------------------------------------------
/**
 * Command data handler.
 * This function is returned whenever AirVantage performs an execute on the switch Channel 2 command
 */
//--------------------------------------------------------------------------------------------------
static void SwitchChannel2Cmd
(
    const char* path,
    le_avdata_AccessType_t accessType,
    le_avdata_ArgumentListRef_t argumentList,
    void* contextPtr
)
{
    LE_INFO("Change channel 2 status");
    if ((channelStatus & 0b0010)>>1 == 0)
    {
        LE_DEBUG("turn on channel");
        channelStatus = channelStatus | 0b0010;
        multiChannelRelay_turn_on_channel(channelStatus);
        
    }
    else
    {
        LE_DEBUG("turn off channel");
        channelStatus = channelStatus & 0b1101;
        multiChannelRelay_turn_on_channel(channelStatus);
    }
    //LE_INFO("Channel status: %d",channelStatus);
    le_avdata_ReplyExecResult(argumentList, LE_OK);
}
//--------------------------------------------------------------------------------------------------
/**
 * Command data handler.
 * This function is returned whenever AirVantage performs an execute on the switch Channel 1 command
 */
//--------------------------------------------------------------------------------------------------
static void SwitchChannel3Cmd
(
    const char* path,
    le_avdata_AccessType_t accessType,
    le_avdata_ArgumentListRef_t argumentList,
    void* contextPtr
)
{
    LE_INFO("Change channel 3 status");
    if ((channelStatus & 0b0100)>>2 == 0)
    {
        LE_DEBUG("turn on channel");
        channelStatus = channelStatus | 0b0100;
        multiChannelRelay_turn_on_channel(channelStatus);
        
    }
    else
    {
        LE_DEBUG("turn off channel");
        channelStatus = channelStatus & 0b1011;
        multiChannelRelay_turn_on_channel(channelStatus);
    }
    //LE_INFO("Channel status: %d",channelStatus);
    le_avdata_ReplyExecResult(argumentList, LE_OK);
}
//--------------------------------------------------------------------------------------------------
/**
 * Command data handler.
 * This function is returned whenever AirVantage performs an execute on the switch Channel 1 command
 */
//--------------------------------------------------------------------------------------------------
static void SwitchChannel4Cmd
(
    const char* path,
    le_avdata_AccessType_t accessType,
    le_avdata_ArgumentListRef_t argumentList,
    void* contextPtr
)
{
    LE_INFO("Change channel 4 status");
    if ((channelStatus & 0b1000)>>3 == 0)
    {
        LE_DEBUG("turn on channel");
        channelStatus = channelStatus | 0b1000;
        multiChannelRelay_turn_on_channel(channelStatus);
        
    }
    else
    {
        LE_DEBUG("turn off channel");
        channelStatus = channelStatus & 0b0111;
        multiChannelRelay_turn_on_channel(channelStatus);
    }
    //LE_INFO("Channel status: %d",channelStatus);
    le_avdata_ReplyExecResult(argumentList, LE_OK);
}

//--------------------------------------------------------------------------------------------------
/**
 * Handle changes in the AirVantage session state
 *
 * When the session is started the timer to sample the sensors is started and when the session is
 * stopped so is the timer.
 */
//--------------------------------------------------------------------------------------------------
static void AvSessionStateHandler
(
    le_avdata_SessionState_t state,
    void *context
)
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


COMPONENT_INIT
{
    LE_INFO("Begin avPub");
    uint8_t res = multiChannelRelay_getChannelState();
    LE_INFO("Channel state : %x",res);

    LE_INFO("Begin set CMD");
    le_avdata_CreateResource(RELAY_CMD_SET_STATE_CHANNEL1, LE_AVDATA_ACCESS_COMMAND);
    le_avdata_AddResourceEventHandler(RELAY_CMD_SET_STATE_CHANNEL1, SwitchChannel1Cmd, NULL);

    le_avdata_CreateResource(RELAY_CMD_SET_STATE_CHANNEL2, LE_AVDATA_ACCESS_COMMAND);
    le_avdata_AddResourceEventHandler(RELAY_CMD_SET_STATE_CHANNEL2, SwitchChannel2Cmd, NULL);

    le_avdata_CreateResource(RELAY_CMD_SET_STATE_CHANNEL3, LE_AVDATA_ACCESS_COMMAND);
    le_avdata_AddResourceEventHandler(RELAY_CMD_SET_STATE_CHANNEL3, SwitchChannel3Cmd, NULL);

    le_avdata_CreateResource(RELAY_CMD_SET_STATE_CHANNEL4, LE_AVDATA_ACCESS_COMMAND);
    le_avdata_AddResourceEventHandler(RELAY_CMD_SET_STATE_CHANNEL4, SwitchChannel4Cmd, NULL);
    channelStatus = multiChannelRelay_getChannelState() &0x0f;
    
    LE_INFO("Create record reference");
    RecordRef  = le_avdata_CreateRecord();

    LE_INFO("Create sameple time");
    SampleTimer = le_timer_Create("Read channel");
    LE_ASSERT_OK(le_timer_SetMsInterval(SampleTimer, DelayBetweenReadings * 1000));
    LE_ASSERT_OK(le_timer_SetRepeat    (SampleTimer, 0));
    LE_ASSERT_OK(le_timer_SetHandler   (SampleTimer, SampleTimerHandler));
    
    LE_INFO("Create session");
    HandlerRef = le_avdata_AddSessionStateHandler(AvSessionStateHandler, NULL);
    AvSession  = le_avdata_RequestSession();
    
    LE_FATAL_IF(AvSession == NULL, "Failed to request avdata session");
}