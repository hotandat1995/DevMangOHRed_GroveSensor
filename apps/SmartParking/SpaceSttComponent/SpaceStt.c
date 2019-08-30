#include "legato.h"
#include "interfaces.h"

/* AirVantage path */
#define PARKING_VAR_STT_SPACE1 "SpacesStatus/Space1"
#define PARKING_VAR_STT_SPACE2 "SpacesStatus/Space2"
#define PARKING_VAR_STT_SPACE3 "SpacesStatus/Space3"
#define PARKING_VAR_STT_SPACE4 "SpacesStatus/Space4"
#define PARKING_VAR_STT_SPACE5 "SpacesStatus/Space5"
#define PARKING_VAR_STT_SPACE6 "SpacesStatus/Space6"
#define PARKING_VAR_STT_SPACE7 "SpacesStatus/Space7"
#define PARKING_VAR_STT_SPACE8 "SpacesStatus/Space8"

#define PARKING_SETTING_STT_SPACE1 "SettingSpacesStatus/Space1"
#define PARKING_SETTING_STT_SPACE2 "SettingSpacesStatus/Space2"
#define PARKING_SETTING_STT_SPACE3 "SettingSpacesStatus/Space3"
#define PARKING_SETTING_STT_SPACE4 "SettingSpacesStatus/Space4"
#define PARKING_SETTING_STT_SPACE5 "SettingSpacesStatus/Space5"
#define PARKING_SETTING_STT_SPACE6 "SettingSpacesStatus/Space6"
#define PARKING_SETTING_STT_SPACE7 "SettingSpacesStatus/Space7"
#define PARKING_SETTING_STT_SPACE8 "SettingSpacesStatus/Space8"

#define LATITUDE_NAME "location/value/latitude"
#define LONGITUDE_NAME "location/value/longitude"

/* DataHub path*/
#define LATITUDE_SENSOR "/app/gpsSensor/location/value/latitude"
#define LONGITUDE_SENSOR "/app/gpsSensor/location/value/longitude"

//--------------------------------------------------------------------------------------------------
/*
 * Type definitions
 */
//--------------------------------------------------------------------------------------------------

struct SpacesStatus{
    int space1;
    int space2;
    int space3;
    int space4;
    int space5;
    int space6;
    int space7;
    int space8;
} LastSpacesStatus;

struct Position{
    double latitude;
    double longtitude;
} LastPosition;


static le_avdata_RequestSessionObjRef_t AvSession;
static le_avdata_SessionStateHandlerRef_t HandlerRef;
static le_avdata_RecordRef_t RecordRef;
static le_timer_Ref_t SampleTimer;

/* Prototype */


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

static uint64_t GetCurrentTimestamp(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    uint64_t utcMilliSec = (uint64_t)(tv.tv_sec) * 1000 + (uint64_t)(tv.tv_usec) / 1000;
    return utcMilliSec;
}

static void SampleTimerHandler
(
    le_timer_Ref_t timer ///< Sensor sampling timer
)
{
    uint64_t now = GetCurrentTimestamp();
    
    le_avdata_SetFloat(LATITUDE_NAME,LastPosition.latitude);

    const char *path1 = "SmartParking.SpacesStatus.Space1";
    le_avdata_RecordInt(RecordRef, path1, LastSpacesStatus.space1, now);
    const char *path2 = "SmartParking.SpacesStatus.Space2";
    le_avdata_RecordInt(RecordRef, path2, LastSpacesStatus.space2, now);
    const char *path3 = "SmartParking.SpacesStatus.Space3";
    le_avdata_RecordInt(RecordRef, path3, LastSpacesStatus.space3, now);
    const char *path4 = "SmartParking.SpacesStatus.Space4";
    le_avdata_RecordInt(RecordRef, path4, LastSpacesStatus.space4, now);
    const char *path5 = "SmartParking.SpacesStatus.Space5";
    le_avdata_RecordInt(RecordRef, path5, LastSpacesStatus.space5, now);
    const char *path6 = "SmartParking.SpacesStatus.Space6";
    le_avdata_RecordInt(RecordRef, path6, LastSpacesStatus.space6, now);
    const char *path7 = "SmartParking.SpacesStatus.Space7";
    le_avdata_RecordInt(RecordRef, path7, LastSpacesStatus.space7, now);
    const char *path8 = "SmartParking.SpacesStatus.Space8";
    le_avdata_RecordInt(RecordRef, path8, LastSpacesStatus.space8, now);
    
    le_avdata_PushRecord(RecordRef, PushCallbackHandler, NULL);
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

// //--------------------------------------------------------------------------------------------------
// /**
//  * Call-back function called when an update is received from the Data Hub for the latitude value.
//  */
// //--------------------------------------------------------------------------------------------------
// static void LatitudeUpdateHandler
// (
//     double timestamp,   ///< time stamp
//     double value,       ///< latitude value, degrees
//     void* contextPtr    ///< not used
// )
// {
//     LastPosition.latitude = value;
//     LE_DEBUG("latitude = %lf (timestamped %lf)", value, timestamp);
// }

// //--------------------------------------------------------------------------------------------------
// /**
//  * Call-back function called when an update is received from the Data Hub for the longitude value.
//  */
// //--------------------------------------------------------------------------------------------------
// static void LongitudeUpdateHandler
// (
//     double timestamp,   ///< time stamp
//     double value,       ///< longitude value, degrees
//     void* contextPtr    ///< not used
// )
// {
//     LastPosition.longtitude = value;
//     LE_DEBUG("longitude = %lf (timestamped %lf)", value, timestamp);
// }

static void SettingSpace1Handler
(
    const char *path,
    le_avdata_AccessType_t accessType,
    le_avdata_ArgumentListRef_t argumentList,
    void *contextPtr
)
{
    le_result_t result = LE_OK;
    int32_t newBound = 0;

    result = le_avdata_GetInt(PARKING_SETTING_STT_SPACE1,&newBound);
    if (result != LE_OK)
    {
        LE_ERROR("le_avdata_GetStringArg('%s') failed(%d)", PARKING_SETTING_STT_SPACE1, result);
    }
    le_result_t resultSetting = le_avdata_SetInt(PARKING_SETTING_STT_SPACE1,newBound);
    if (LE_FAULT == resultSetting)
    {
        LE_ERROR("Error in setting PARKING_SETTING_STT_SPACE1");
    }
    LastSpacesStatus.space1 = newBound;
}

static void SettingSpace2Handler
(
    const char *path,
    le_avdata_AccessType_t accessType,
    le_avdata_ArgumentListRef_t argumentList,
    void *contextPtr
)
{
    le_result_t result = LE_OK;
    int32_t newBound = 0;

    result = le_avdata_GetInt(PARKING_SETTING_STT_SPACE2,&newBound);
    if (result != LE_OK)
    {
        LE_ERROR("le_avdata_GetStringArg('%s') failed(%d)", PARKING_SETTING_STT_SPACE2, result);
    }
    le_result_t resultSetting = le_avdata_SetInt(PARKING_SETTING_STT_SPACE2,newBound);
    if (LE_FAULT == resultSetting)
    {
        LE_ERROR("Error in setting PARKING_SETTING_STT_SPACE2");
    }
    LastSpacesStatus.space2 = newBound;
}

static void SettingSpace3Handler
(
    const char *path,
    le_avdata_AccessType_t accessType,
    le_avdata_ArgumentListRef_t argumentList,
    void *contextPtr
)
{
    le_result_t result = LE_OK;
    int32_t newBound = 0;

    result = le_avdata_GetInt(PARKING_SETTING_STT_SPACE3,&newBound);
    if (result != LE_OK)
    {
        LE_ERROR("le_avdata_GetStringArg('%s') failed(%d)", PARKING_SETTING_STT_SPACE3, result);
    }
    le_result_t resultSetting = le_avdata_SetInt(PARKING_SETTING_STT_SPACE3,newBound);
    if (LE_FAULT == resultSetting)
    {
        LE_ERROR("Error in setting PARKING_SETTING_STT_SPACE3");
    }
    LastSpacesStatus.space3 = newBound;
}

static void SettingSpace4Handler
(
    const char *path,
    le_avdata_AccessType_t accessType,
    le_avdata_ArgumentListRef_t argumentList,
    void *contextPtr
)
{
    le_result_t result = LE_OK;
    int32_t newBound = 0;

    result = le_avdata_GetInt(PARKING_SETTING_STT_SPACE4,&newBound);
    if (result != LE_OK)
    {
        LE_ERROR("le_avdata_GetStringArg('%s') failed(%d)", PARKING_SETTING_STT_SPACE4, result);
    }
    le_result_t resultSetting = le_avdata_SetInt(PARKING_SETTING_STT_SPACE4,newBound);
    if (LE_FAULT == resultSetting)
    {
        LE_ERROR("Error in setting PARKING_SETTING_STT_SPACE4");
    }
    LastSpacesStatus.space4 = newBound;
}

static void SettingSpace5Handler
(
    const char *path,
    le_avdata_AccessType_t accessType,
    le_avdata_ArgumentListRef_t argumentList,
    void *contextPtr
)
{
    le_result_t result = LE_OK;
    int32_t newBound = 0;

    result = le_avdata_GetInt(PARKING_SETTING_STT_SPACE5,&newBound);
    if (result != LE_OK)
    {
        LE_ERROR("le_avdata_GetStringArg('%s') failed(%d)", PARKING_SETTING_STT_SPACE5, result);
    }
    le_result_t resultSetting = le_avdata_SetInt(PARKING_SETTING_STT_SPACE5,newBound);
    if (LE_FAULT == resultSetting)
    {
        LE_ERROR("Error in setting PARKING_SETTING_STT_SPACE5");
    }
    LastSpacesStatus.space5 = newBound;
}

static void SettingSpace6Handler
(
    const char *path,
    le_avdata_AccessType_t accessType,
    le_avdata_ArgumentListRef_t argumentList,
    void *contextPtr
)
{
    le_result_t result = LE_OK;
    int32_t newBound = 0;

    result = le_avdata_GetInt(PARKING_SETTING_STT_SPACE6,&newBound);
    if (result != LE_OK)
    {
        LE_ERROR("le_avdata_GetStringArg('%s') failed(%d)", PARKING_SETTING_STT_SPACE6, result);
    }
    le_result_t resultSetting = le_avdata_SetInt(PARKING_SETTING_STT_SPACE6,newBound);
    if (LE_FAULT == resultSetting)
    {
        LE_ERROR("Error in setting PARKING_SETTING_STT_SPACE6");
    }
    LastSpacesStatus.space6 = newBound;
}

static void SettingSpace7Handler
(
    const char *path,
    le_avdata_AccessType_t accessType,
    le_avdata_ArgumentListRef_t argumentList,
    void *contextPtr
)
{
    le_result_t result = LE_OK;
    int32_t newBound = 0;

    result = le_avdata_GetInt(PARKING_SETTING_STT_SPACE7,&newBound);
    if (result != LE_OK)
    {
        LE_ERROR("le_avdata_GetStringArg('%s') failed(%d)", PARKING_SETTING_STT_SPACE7, result);
    }
    le_result_t resultSetting = le_avdata_SetInt(PARKING_SETTING_STT_SPACE7,newBound);
    if (LE_FAULT == resultSetting)
    {
        LE_ERROR("Error in setting PARKING_SETTING_STT_SPACE7");
    }
    LastSpacesStatus.space7 = newBound;
}

static void SettingSpace8Handler
(
    const char *path,
    le_avdata_AccessType_t accessType,
    le_avdata_ArgumentListRef_t argumentList,
    void *contextPtr
)
{
    le_result_t result = LE_OK;
    int32_t newBound = 0;

    result = le_avdata_GetInt(PARKING_SETTING_STT_SPACE8,&newBound);
    if (result != LE_OK)
    {
        LE_ERROR("le_avdata_GetStringArg('%s') failed(%d)", PARKING_SETTING_STT_SPACE8, result);
    }
    le_result_t resultSetting = le_avdata_SetInt(PARKING_SETTING_STT_SPACE8,newBound);
    if (LE_FAULT == resultSetting)
    {
        LE_ERROR("Error in setting PARKING_SETTING_STT_SPACE8");
    }
    LastSpacesStatus.space8 = newBound;
}

COMPONENT_INIT
{
    LE_INFO("Smart Parking Publisher Started!");

    // AirVantage congfigure
    LE_INFO("Create sameple time");
    SampleTimer = le_timer_Create("Update Parking Status");
    LE_ASSERT_OK(le_timer_SetMsInterval(SampleTimer, 5 * 1000));
    LE_ASSERT_OK(le_timer_SetRepeat(SampleTimer, 0));
    LE_ASSERT_OK(le_timer_SetHandler(SampleTimer, SampleTimerHandler));
    le_timer_Start(SampleTimer);

    LE_INFO("Create record reference");
    RecordRef = le_avdata_CreateRecord();

    //le_result_t result;

    // // Connect to the GPS sensor
    // // This will be received from the Data Hub.
    // result = io_CreateOutput(LATITUDE_NAME, IO_DATA_TYPE_NUMERIC, "degrees");
    // LE_ASSERT(result == LE_OK);
    // result = io_CreateOutput(LONGITUDE_NAME, IO_DATA_TYPE_NUMERIC, "degrees");
    // LE_ASSERT(result == LE_OK);

    // // Register for notification of updates to the counter value.
    // io_AddNumericPushHandler(LATITUDE_NAME, LatitudeUpdateHandler, NULL);
    // io_AddNumericPushHandler(LONGITUDE_NAME, LongitudeUpdateHandler, NULL);

    le_avdata_CreateResource(PARKING_SETTING_STT_SPACE1, LE_AVDATA_ACCESS_SETTING);
    le_avdata_AddResourceEventHandler(PARKING_SETTING_STT_SPACE1, SettingSpace1Handler, NULL);
    le_avdata_CreateResource(PARKING_SETTING_STT_SPACE2, LE_AVDATA_ACCESS_SETTING);
    le_avdata_AddResourceEventHandler(PARKING_SETTING_STT_SPACE2, SettingSpace2Handler, NULL);
    le_avdata_CreateResource(PARKING_SETTING_STT_SPACE3, LE_AVDATA_ACCESS_SETTING);
    le_avdata_AddResourceEventHandler(PARKING_SETTING_STT_SPACE3, SettingSpace3Handler, NULL);
    le_avdata_CreateResource(PARKING_SETTING_STT_SPACE4, LE_AVDATA_ACCESS_SETTING);
    le_avdata_AddResourceEventHandler(PARKING_SETTING_STT_SPACE4, SettingSpace4Handler, NULL);
    le_avdata_CreateResource(PARKING_SETTING_STT_SPACE5, LE_AVDATA_ACCESS_SETTING);
    le_avdata_AddResourceEventHandler(PARKING_SETTING_STT_SPACE5, SettingSpace5Handler, NULL);
    le_avdata_CreateResource(PARKING_SETTING_STT_SPACE6, LE_AVDATA_ACCESS_SETTING);
    le_avdata_AddResourceEventHandler(PARKING_SETTING_STT_SPACE6, SettingSpace6Handler, NULL);
    le_avdata_CreateResource(PARKING_SETTING_STT_SPACE7, LE_AVDATA_ACCESS_SETTING);
    le_avdata_AddResourceEventHandler(PARKING_SETTING_STT_SPACE7, SettingSpace7Handler, NULL);
    le_avdata_CreateResource(PARKING_SETTING_STT_SPACE8, LE_AVDATA_ACCESS_SETTING);
    le_avdata_AddResourceEventHandler(PARKING_SETTING_STT_SPACE8, SettingSpace8Handler, NULL);
    
    // Connect to the Parking space status
    // Waiting for Kieu's app

    LE_INFO("Create session");
    HandlerRef = le_avdata_AddSessionStateHandler(AvSessionStateHandler, NULL);
    AvSession = le_avdata_RequestSession();

    LE_FATAL_IF(AvSession == NULL, "Failed to request avdata session");
}
