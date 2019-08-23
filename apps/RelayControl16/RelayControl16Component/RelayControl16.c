#include "legato.h"
#include "interfaces.h"

#define RELAY_CMD_SET_STATE_CHANNEL1 "channelSetting/StateChannel1"
#define RELAY_CMD_SET_STATE_CHANNEL2 "channelSetting/StateChannel2"
#define RELAY_CMD_SET_BLINK_CHANNEL1 "channelSetting/BlinkChannel1"

static le_avdata_RequestSessionObjRef_t AvSession;
static le_avdata_SessionStateHandlerRef_t HandlerRef;
static le_avdata_RecordRef_t RecordRef;
static le_timer_Ref_t SampleTimer;

static bool channel1_status = false;
static bool channel2_status = false;

le_result_t setting_Pin(void){
    le_gpioPin8_SetPushPullOutput(LE_GPIOPIN8_ACTIVE_LOW,true);
    le_gpioPin40_SetPushPullOutput(LE_GPIOPIN40_ACTIVE_LOW,true);
    return LE_OK;
}

static void SampleTimerHandler
(
    le_timer_Ref_t timer ///< Sensor sampling timer
)
{
    //uint64_t now = GetCurrentTimestamp();

    // le_result_t r = le_avdata_PushRecord(RecordRef, PushCallbackHandler, NULL);
    // if (r != LE_OK)
    // {
    //     LE_ERROR("Failed to push record - %s", LE_RESULT_TXT(r));
    // }
    // if(channel1_status == true)
    // {
    //     le_gpioPin8_Deactivate();
    //     channel1_status = false;
    // }
    // else
    // {
    //     le_gpioPin8_Activate();
    //     channel1_status = true;
    // }
    
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

static void StateChannel1Setting
(
    const char *path,
    le_avdata_AccessType_t accessType,
    le_avdata_ArgumentListRef_t argumentList,
    void *contextPtr
)
{
    LE_INFO("Change Lower bound");
    le_result_t result = LE_OK;
    int32_t newBound = 0;

    LE_INFO("Set value");
    result = le_avdata_GetInt(RELAY_CMD_SET_STATE_CHANNEL1,&newBound);
    if (result != LE_OK)
    {
        LE_ERROR("le_avdata_GetStringArg('%s') failed(%d)", RELAY_CMD_SET_STATE_CHANNEL1, result);
    }
    le_result_t resultSetting = le_avdata_SetInt(RELAY_CMD_SET_STATE_CHANNEL1,newBound);
    if (LE_FAULT == resultSetting)
    {
        LE_ERROR("Error in setting RELAY_CMD_SET_STATE_CHANNEL1");
    }
    if(newBound == 0){
        le_gpioPin8_Deactivate();
        channel1_status = false;
        LE_INFO("LED 1 OFF");
    }
    else
    {
        le_gpioPin8_Activate();
        channel1_status = true;
        LE_INFO("LED 1 ONN");
    }
}

static void StateChannel2Setting
(
    const char *path,
    le_avdata_AccessType_t accessType,
    le_avdata_ArgumentListRef_t argumentList,
    void *contextPtr
)
{
    LE_INFO("Change Lower bound");
    le_result_t result = LE_OK;
    int32_t newBound = 0;

    LE_INFO("Set value");
    result = le_avdata_GetInt(RELAY_CMD_SET_STATE_CHANNEL2,&newBound);
    if (result != LE_OK)
    {
        LE_ERROR("le_avdata_GetStringArg('%s') failed(%d)", RELAY_CMD_SET_STATE_CHANNEL2, result);
    }
    le_result_t resultSetting = le_avdata_SetInt(RELAY_CMD_SET_STATE_CHANNEL2,newBound);
    if (LE_FAULT == resultSetting)
    {
        LE_ERROR("Error in setting RELAY_CMD_SET_STATE_CHANNEL2");
    }
    if(newBound == 0){
        le_gpioPin40_Deactivate();
        channel2_status = false;
        LE_INFO("LED 2 OFF");
    }
    else
    {
        le_gpioPin40_Activate();
        channel2_status = true;
        LE_INFO("LED 2 ON");
    }
}

static void BlinkChannel1Setting(
    const char *path,
    le_avdata_AccessType_t accessType,
    le_avdata_ArgumentListRef_t argumentList,
    void *contextPtr)
{
    LE_INFO("Change Blink interval");
    le_result_t result = LE_OK;
    int32_t newBound = 0;

    result = le_avdata_GetInt(RELAY_CMD_SET_BLINK_CHANNEL1,&newBound);
    if (result != LE_OK)
    {
        LE_ERROR("le_avdata_GetStringArg('%s') failed(%d)", RELAY_CMD_SET_BLINK_CHANNEL1, result);
    }
    le_result_t resultSetting = le_avdata_SetInt(RELAY_CMD_SET_BLINK_CHANNEL1,newBound);
    if (LE_FAULT == resultSetting)
    {
        LE_ERROR("Error in setting RELAY_CMD_SET_BLINK_CHANNEL1");
    }
    if(newBound < 0){
        LE_INFO("Wrong value");
    }
    else if(newBound == 0){
        le_timer_Stop(SampleTimer);
        LE_INFO("Stop Timer");
    }
    else
    {
        LE_ASSERT_OK(le_timer_SetMsInterval(SampleTimer, newBound * 1000));
        LE_INFO("Set new timer Interval success!");
        LE_INFO("LED ON");
    }
}

COMPONENT_INIT
{
    LE_INFO("LED control sensor started");
    setting_Pin();

    // AirVantage congfigure
    LE_INFO("Create sameple time");
    SampleTimer = le_timer_Create("Read channel");
    LE_ASSERT_OK(le_timer_SetMsInterval(SampleTimer, 1 * 1000));
    LE_ASSERT_OK(le_timer_SetRepeat(SampleTimer, 0));
    LE_ASSERT_OK(le_timer_SetHandler(SampleTimer, SampleTimerHandler));
    le_timer_Start(SampleTimer);

    LE_INFO("Create record reference");
    RecordRef = le_avdata_CreateRecord();

    le_avdata_CreateResource(RELAY_CMD_SET_STATE_CHANNEL1, LE_AVDATA_ACCESS_SETTING);
    le_avdata_AddResourceEventHandler(RELAY_CMD_SET_STATE_CHANNEL1, StateChannel1Setting, NULL);
    le_avdata_CreateResource(RELAY_CMD_SET_STATE_CHANNEL2, LE_AVDATA_ACCESS_SETTING);
    le_avdata_AddResourceEventHandler(RELAY_CMD_SET_STATE_CHANNEL2, StateChannel2Setting, NULL);

    le_avdata_CreateResource(RELAY_CMD_SET_BLINK_CHANNEL1, LE_AVDATA_ACCESS_SETTING);
    le_avdata_AddResourceEventHandler(RELAY_CMD_SET_BLINK_CHANNEL1, BlinkChannel1Setting, NULL);

    LE_INFO("Create session");
    HandlerRef = le_avdata_AddSessionStateHandler(AvSessionStateHandler, NULL);
    AvSession = le_avdata_RequestSession();

    LE_FATAL_IF(AvSession == NULL, "Failed to request avdata session")
}
