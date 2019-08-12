#include "legato.h"
#include "interfaces.h"

static le_timer_Ref_t Reader;


le_result_t setting_LEDPin(void){
    le_gpioPin8_SetPushPullOutput(LE_GPIOPIN8_ACTIVE_HIGH,true);
    return LE_OK;
}


static void ReaderHandler
(
    le_timer_Ref_t timer  ///< Reader timer
)
{
    if(le_gpioPin8_IsActive() == false){
        le_gpioPin8_Activate();
    }
    else{
        le_gpioPin8_Deactivate();
    }
}

COMPONENT_INIT
{
    LE_INFO("LED control sensor started");
    setting_LEDPin();

    //Setup timer to read data
    Reader = le_timer_Create("Get Sample");
    LE_ASSERT_OK(le_timer_SetRepeat (Reader, 0));
    LE_ASSERT_OK(le_timer_SetMsInterval(Reader, 1000));
    LE_ASSERT_OK(le_timer_SetHandler(Reader, ReaderHandler));
    le_timer_Start(Reader);
}
