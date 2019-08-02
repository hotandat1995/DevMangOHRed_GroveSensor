#include "legato.h"
#include "interfaces.h"

static le_timer_Ref_t Reader;

static void PIRMotion_ChangeHandler
(
    bool state, 
    void *ctx
)
{
    le_gpioPin8_SetEdgeSense(LE_GPIOPIN8_EDGE_BOTH);
    if(state){
        LE_INFO("Detected");
    }
    else{
        LE_INFO("Not Detected");
    }
}

le_result_t setting_PIRMotionPin(void){
    le_gpioPin8_SetInput(LE_GPIOPIN8_ACTIVE_HIGH);
    //le_gpioPin8_EnablePullDown();
    //le_gpioPin8_DisableResistors();
    return LE_OK;
}


static void ReaderHandler
(
    le_timer_Ref_t timer  ///< Reader timer
)
{
    uint8_t state = le_gpioPin8_Read();
    if(state){
        //LE_INFO("Detected");
    }
    else{
        //LE_INFO("Watching");
    }

    int32_t value;

    const le_result_t result = le_adc_ReadValue("EXT_ADC0", &value);

    if (result == LE_OK)
    {
        LE_INFO("EXT_ADC0 value is: %d", value);
    }
    else
    {
        LE_INFO("Couldn't get ADC value");
    }
}

COMPONENT_INIT
{
    LE_INFO("PIR Motion sensor started");
    setting_PIRMotionPin();
    //Setup timer to read data
    le_gpioPin8_AddChangeEventHandler( LE_GPIOPIN8_EDGE_BOTH,
                                        PIRMotion_ChangeHandler,
                                        NULL,
                                        0);

    //Setup timer to read data
    Reader = le_timer_Create("Get Sample");
    LE_ASSERT_OK(le_timer_SetRepeat (Reader, 0));
    LE_ASSERT_OK(le_timer_SetMsInterval(Reader, 10));
    LE_ASSERT_OK(le_timer_SetHandler(Reader, ReaderHandler));
    le_timer_Start(Reader);
}
