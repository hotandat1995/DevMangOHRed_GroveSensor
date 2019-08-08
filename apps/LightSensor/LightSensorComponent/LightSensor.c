#include "legato.h"
#include "interfaces.h"

static le_timer_Ref_t Reader;

static void ReaderHandler
(
    le_timer_Ref_t timer  ///< Reader timer
)
{
    int32_t value;

    const le_result_t result = le_adc_ReadValue("EXT_ADC0", &value);
    if(result == LE_OK){
        LE_INFO("ADC value: %d, in voltage: %f mV",value,(float)value/1788*3.5*1000);
    }
    else{
        LE_INFO("Can't read ADC, check wire!");
    }
}

COMPONENT_INIT
{
    LE_INFO("PIR Motion sensor started");

    //Setup timer to read data
    Reader = le_timer_Create("Get Sample");
    LE_ASSERT_OK(le_timer_SetRepeat (Reader, 0));
    LE_ASSERT_OK(le_timer_SetMsInterval(Reader, 1000));
    LE_ASSERT_OK(le_timer_SetHandler(Reader, ReaderHandler));
    le_timer_Start(Reader);
}
