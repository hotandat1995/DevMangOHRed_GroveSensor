#include "legato.h"
#include "interfaces.h"

#define dhubIO_DataType_t io_DataType_t

#define LATITUDE_NAME "location/value/latitude"
#define LONGITUDE_NAME "location/value/longitude"

static le_timer_Ref_t SampleTimer;

static void SampleTimerHandler
(
    le_timer_Ref_t timer ///< Sensor sampling timer
)
{
    LE_INFO("Sent DataHub");
    //LE_INFO("Sent DataHub %lf" ,gpsSensor_get_LastLatitude());
    //io_PushNumeric(LATITUDE_NAME, IO_NOW, gpsSensor_get_LastLatitude());
}

COMPONENT_INIT
{
    LE_INFO("GPS sent data to DataHub started");

    le_result_t result;

    // Connect to the GPS sensor
    // This will be received from the Data Hub.
    result = io_CreateInput(LATITUDE_NAME, IO_DATA_TYPE_NUMERIC, "degrees");
    LE_ASSERT(result == LE_OK);
    result = io_CreateInput(LONGITUDE_NAME, IO_DATA_TYPE_NUMERIC, "degrees");
    LE_ASSERT(result == LE_OK);

    // AirVantage congfigure
    LE_INFO("Create sameple time");
    SampleTimer = le_timer_Create("Get GPS");
    LE_ASSERT_OK(le_timer_SetMsInterval(SampleTimer, 2 * 1000));
    LE_ASSERT_OK(le_timer_SetRepeat(SampleTimer, 0));
    LE_ASSERT_OK(le_timer_SetHandler(SampleTimer, SampleTimerHandler));
    le_timer_Start(SampleTimer);
}