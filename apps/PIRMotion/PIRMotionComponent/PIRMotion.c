#include "legato.h"
#include "interfaces.h"

static le_timer_Ref_t Reader;

static void PIRMotion_ChangeHandler
(
    bool state, 
    void *ctx
)
{
    if(state){
        LE_INFO("Handler Detected");
    }
}

le_result_t setting_PIRMotionPin(void){
    le_gpioPin8_SetInput(LE_GPIOPIN8_ACTIVE_HIGH
    );
    return LE_OK;
}


static void ReaderHandler
(
    le_timer_Ref_t timer  ///< Reader timer
)
{
    uint8_t state = le_gpioPin8_Read();
    if(state){
        LE_INFO("Detected");
    }
    else{
        LE_INFO("Watching");
    }
}

COMPONENT_INIT
{
    LE_INFO("PIR Motion sensor started");
    setting_PIRMotionPin();
    //Setup timer to read data
    le_gpioPin8_AddChangeEventHandler(  LE_GPIOPIN8_EDGE_BOTH,
                                        PIRMotion_ChangeHandler,
                                        NULL,
                                        0);

    //Setup timer to read data
    Reader = le_timer_Create("Get Sample");
    LE_ASSERT_OK(le_timer_SetRepeat (Reader, 0));
    LE_ASSERT_OK(le_timer_SetMsInterval(Reader, 1000));
    LE_ASSERT_OK(le_timer_SetHandler(Reader, ReaderHandler));
    le_timer_Start(Reader);
}
