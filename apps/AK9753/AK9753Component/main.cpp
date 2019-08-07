#include "AK9753.h"

static le_timer_Ref_t Reader;

#define dhubIO_DataType_t io_DataType_t

#define IR1_VALUE_NAME "ak9753/value/ic1"
#define IR2_VALUE_NAME "ak9753/value/ic2"
#define IR3_VALUE_NAME "ak9753/value/ic3"
#define IR4_VALUE_NAME "ak9753/value/ic4"

#define IR1_STATE_NAME "ak9753/state/ic1"
#define IR2_STATE_NAME "ak9753/state/ic2"
#define IR3_STATE_NAME "ak9753/state/ic3"
#define IR4_STATE_NAME "ak9753/state/ic4"
#define MILLISECS_NAME "ak9753/state/millisecs"

/* Declare classe to read data */
AK9753 movementSensor;
// need to adjust these sensitivities lower if you want to detect more far
// but will introduce error detection
float sensitivity_presence = 6.0;
float sensitivity_movement = 10.0;
int detect_interval = 30; //milliseconds
PresenceDetector detector(  movementSensor, 
                            sensitivity_presence, 
                            sensitivity_movement, 
                            detect_interval);

le_result_t setup_module(){
    //Turn on sensor
    if (movementSensor.initialize() == false)
    {
        return LE_FAULT;
    }
    return LE_OK;
}

static void ReaderHandler
(
    le_timer_Ref_t timer  ///< Led state timer
)
{
    detector.loop();
    io_PushNumeric(IR1_VALUE_NAME, IO_NOW, (double_t)detector.getDerivativeOfIR1());
    io_PushNumeric(IR2_VALUE_NAME, IO_NOW, (double_t)detector.getDerivativeOfIR1());
    io_PushNumeric(IR3_VALUE_NAME, IO_NOW, (double_t)detector.getDerivativeOfIR1());
    io_PushNumeric(IR4_VALUE_NAME, IO_NOW, (double_t)detector.getDerivativeOfIR1());

    io_PushBoolean(IR1_STATE_NAME, IO_NOW, (double_t)detector.presentField1());
    io_PushBoolean(IR2_STATE_NAME, IO_NOW, (double_t)detector.presentField2());
    io_PushBoolean(IR3_STATE_NAME, IO_NOW, (double_t)detector.presentField3());
    io_PushBoolean(IR4_STATE_NAME, IO_NOW, (double_t)detector.presentField4());
    io_PushNumeric(MILLISECS_NAME, IO_NOW, (double_t)millis());
}

COMPONENT_INIT
{
    LE_INFO("AK9753 Sensor started!!!");

    if(setup_module() != LE_OK){
        LE_INFO("Device not found. Check wiring.");
    }
    else{
        le_result_t result;
        result = io_CreateInput(IR1_VALUE_NAME, IO_DATA_TYPE_NUMERIC, "");
        LE_ASSERT(result == LE_OK);
        result = io_CreateInput(IR2_VALUE_NAME, IO_DATA_TYPE_NUMERIC, "");
        LE_ASSERT(result == LE_OK);
        result = io_CreateInput(IR3_VALUE_NAME, IO_DATA_TYPE_NUMERIC, "");
        LE_ASSERT(result == LE_OK);
        result = io_CreateInput(IR4_VALUE_NAME, IO_DATA_TYPE_NUMERIC, "");
        LE_ASSERT(result == LE_OK);

        result = io_CreateInput(IR1_STATE_NAME, IO_DATA_TYPE_BOOLEAN, "");
        LE_ASSERT(result == LE_OK);
        result = io_CreateInput(IR2_STATE_NAME, IO_DATA_TYPE_BOOLEAN, "");
        LE_ASSERT(result == LE_OK);
        result = io_CreateInput(IR3_STATE_NAME, IO_DATA_TYPE_BOOLEAN, "");
        LE_ASSERT(result == LE_OK);
        result = io_CreateInput(IR4_STATE_NAME, IO_DATA_TYPE_BOOLEAN, "");
        LE_ASSERT(result == LE_OK);
        result = io_CreateInput(MILLISECS_NAME, IO_DATA_TYPE_NUMERIC, "millisecs");
        LE_ASSERT(result == LE_OK);

        /*Create timer to read data from sensor */
        Reader = le_timer_Create("Get Sample AK9753");
        LE_ASSERT_OK(le_timer_SetRepeat(Reader, 0));
        LE_ASSERT_OK(le_timer_SetMsInterval(Reader, 10));
        LE_ASSERT_OK(le_timer_SetHandler(Reader, ReaderHandler));
        le_timer_Start(Reader);
    }
}