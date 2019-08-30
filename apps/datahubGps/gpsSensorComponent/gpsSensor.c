#include "legato.h"
#include "interfaces.h"

static float last_latitude = 0;
static float last_longtitude = 0;

double gpsSensor_get_LastLatitude(){
    return (double)last_latitude;
}

double gpsSensor_get_LastLongtitude(){
    return (double)last_longtitude;
}

static void NavigationHandler(
    le_pos_SampleRef_t positionSampleRef,
    void *contextPtr)
{
    le_pos_ConnectService();

    int32_t latitude, longitude, accuracy;

    if (NULL == positionSampleRef)
    {
        LE_ERROR("New Position sample is NULL!");
    }
    else
    {
        LE_DEBUG("New Position sample %p", positionSampleRef);
    }

    le_pos_sample_Get2DLocation(positionSampleRef, &latitude, &longitude, &accuracy);
    last_latitude = (float)latitude / 1000000;
    last_longtitude = (float)longitude / 1000000;
    LE_INFO("Get2DLocation: lat.%lf, long.%lf, accuracy.%d",
            last_latitude,
            last_longtitude,
            accuracy);
}

COMPONENT_INIT
{
    le_posCtrl_ActivationRef_t activationRef;
    le_pos_MovementHandlerRef_t NavigationHandlerRef;
    LE_INFO("GPS sensor begin");

    LE_INFO("Request activation of the positioning service");
    LE_ASSERT((activationRef = le_posCtrl_Request()) != NULL);
    // Test the registration of an handler for movement notifications with horizontal or vertical
    // magnitude of 0 meters. (It will set an acquisition rate of 1sec).
    NavigationHandlerRef = le_pos_AddMovementHandler(0, 0, NavigationHandler, NULL);
    LE_ASSERT(NULL != NavigationHandlerRef);
}