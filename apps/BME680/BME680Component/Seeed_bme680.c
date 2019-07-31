/**
 * @file seeed_bme680.cpp
 * Library for BME680
 *
 * Copyright (c) 2013 Seeed Technology Co., Ltd.
 * @author        :   downey
 * @date Create Time   :   2017/12/08
 * Change Log    :
 */

/**
 * @brief The main driver file of BME680 sensor.
 *
 * BME680 support for temperature,humidity,indoor-air_quality(gas) and pressure value measurement,
 * The result of measurement is stored in class  Seeed_BME680->sensor_result_value.
 * BME680 support for two communication protocol-SPI and IIC,The different communication  protocol
 * corresponding different constructor.Furthermore,you can customize the pin when your development board's SPI
 * interface is mismatch with official.All you have to do is choose different ways to instantiate object.
 */
#include "Seeed_bme680.h"
#include "interfaces.h"

#define dhubIO_DataType_t io_DataType_t
#define TEMPERATURE_NAME "bme680/value/temperature"
#define PRESSURE_NAME    "bme680/value/pressure"
#define HUMIDITY_NAME    "bme680/value/humidity"
#define GAS_NAME         "bme680/value/gas"

#define PERIOD_NAME      "bme680/period"


/*Global variable */
static const char bme680_i2c_bus[20] = "/dev/i2c-5";	    //< I2C bus
uint8_t buf[32]={0,};										//< Buffer store data
size_t  buf_len = sizeof(buf);								//< Lenght of this buffer

static le_timer_Ref_t Reader;

sensor_result_t sensor_result_value;
static void delay_msec(uint32_t ms);
bme680_dev_t sensor_param;      /**< Official LIB structure.*/

/**@brief constructor of IIC interface.
 * @param addr The BME680 device IIC address.
 * @return NONE.
 */
void Seeed_BME680(uint8_t addr)
{
    sensor_param.dev_id = addr;
    sensor_param.intf = BME680_I2C_INTF;
    
    sensor_param.read  = iic_read;
    sensor_param.write = iic_write;
    sensor_param.delay_ms = delay_msec;
}

///@brief This function implements the temperature value of the read sensor
/// @param  NONE.
/// @return sensor_result_value.temperature  The result of temperature value.
float read_temperature(void)
{
    int result;

    result = read_sensor_data();

    if (result)
    {
        return 0;
    }
    
    return sensor_result_value.temperature;
}

///@brief This function implements the pressure value of the read sensor
/// @param  NONE.
/// @return sensor_result_value.pressure  The result of pressure value.
///
float read_pressure(void)
{
    if (read_sensor_data())
    {
        return 0;
    }
    return sensor_result_value.pressure/1000;
}

/**@brief This function implements the humidity value of the read sensor
 * @param  NONE.
 * @return sensor_result_value.humidity  The result of humidity value.
 */
float read_humidity(void)
{
    if (read_sensor_data())
    {
        return 0;
    }
    return sensor_result_value.humidity;
}

/**@brief This function implements the gas value of the read sensor
 * @param  NONE.
 * @return sensor_result_value.gas  The result of gas value.
 */
float read_gas(void)
{
    if (read_sensor_data())
    {
        return 0;
    }
    return sensor_result_value.gas/1000;
}

/**@brief Getting four kinds of result value from the sensor.
 * @param NONE.
 * @return Result of function excution. The normal exit is only when it returns BME680_OK(0).
 */
int8_t read_sensor_data(void)
{

    struct bme680_field_data data;

    int8_t ret;

    sensor_param.power_mode = BME680_FORCED_MODE;

    uint16_t settings_sel;

    sensor_param.tph_sett.os_hum  = BME680_OS_1X;
    sensor_param.tph_sett.os_pres = BME680_OS_16X;
    sensor_param.tph_sett.os_temp = BME680_OS_2X;
    
    sensor_param.gas_sett.run_gas = BME680_ENABLE_GAS_MEAS;
    sensor_param.gas_sett.heatr_dur = 100;
    sensor_param.gas_sett.heatr_temp = 300;

    settings_sel =  BME680_OST_SEL | 
                    BME680_OSH_SEL | 
                    BME680_OSP_SEL | 
                    BME680_GAS_MEAS_SEL |
                    BME680_HCNTRL_SEL |
                    BME680_RUN_GAS_SEL |
                    BME680_NBCONV_SEL |
                    BME680_FILTER_SEL | 
                    BME680_GAS_SENSOR_SEL
                    ;

    /*Set sensor's registers*/
    ret = bme680_set_sensor_settings(settings_sel, &sensor_param);
    if (ret != 0){
        return -1;
    }

    /*Set sensor's mode ,activate sensor*/
    ret = bme680_set_sensor_mode(&sensor_param);
    if(ret != 0){
        return -2;
    }

    uint16_t meas_period;
    bme680_get_profile_dur(&meas_period, &sensor_param);

    delay_msec(meas_period); /**<It is necessary to delay for a duration time */

    /*Get sensor's result value from registers*/
    ret = bme680_get_sensor_data(&data, &sensor_param);
    
    if (ret != 0)
    {
        return -3;
    }

    sensor_result_value.temperature = data.temperature / 100.0;
    sensor_result_value.humidity = data.humidity / 1000.0;
    sensor_result_value.pressure = data.pressure;
    if (data.status & BME680_HEAT_STAB_MSK)
    {
        sensor_result_value.gas = data.gas_resistance;
    }
    else
    {
        sensor_result_value.gas = 0;
    }
    return BME680_OK;
}

/**@brief  IIC reading interface
 * @param  dev_id   IIC device address
 * @param  reg_addr The register address for operated.
 * @param  reg_data Storing data read from  registers.
 * @param  len      Length of data.
 * @return The normal exit is only when it returns 0.
 */
int8_t iic_read(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len)
{
    int8_t result;
	result = i2cReceiveBytes_v2(bme680_i2c_bus, dev_id , reg_addr, buf, len);
    if(result != LE_OK)
        LE_INFO("Can't read: %d",result);
    for (int i = 0; i < len; i++)
    {
        reg_data[i] = (uint8_t)buf[i];
    }
    return 0;
}

/**@brief  IIC wrting interface
 * @param  dev_id   IIC device address
 * @param  reg_addr The register address for operated.
 * @param  reg_data The data need to be transmitted.
 * @param  len      Length of data.
 * @return The normal exit is only when it returns 0.
 */
int8_t iic_write(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len)
{
    int8_t result;
    uint8_t cmd[len+1];
    cmd[0] = reg_addr;
    for(int i=0;i<len;i++){
        cmd[i+1] = reg_data[i];
    }
    result = i2cSendBytes(bme680_i2c_bus, dev_id,cmd,len+1);
    if(result != LE_OK)
        LE_INFO("Can't send");
    return 0;
}

/**@brief delay milliseconds
 * @param ms    milliseconds
 * @return NONE
 */
static void delay_msec(uint32_t ms)
{
    usleep(ms*1000);
}

/**@brief Initialization of sensor
 * @param NONE
 * @return ture or false
 */
bool Seeed_BME680_init()
{
    int8_t ret = BME680_OK;
    // Check the wiring,Check whether the protocol stack is normal and 
    // Read the calibrated value from sensor
    ret = bme680_init(&sensor_param);
    if (ret < BME680_OK)
    {
        LE_INFO("Init fail, ret = %d",ret);
        return false;
    }
    LE_INFO("Init success, ressult is %d",ret);
    return true;
}
/*If you don't have mux i2c kernel use this to open i2c-hub (to see this sensor address)
 */
void openI2CHub(){
	i2cSendByte(bme680_i2c_bus,0x71,0x0f);
}

static void ReaderHandler
(
    le_timer_Ref_t timer  ///< Led state timer
)
{
    float temp = 0;

    temp = read_gas();
    //LE_INFO("Gas is: %lf",temp);
    io_PushNumeric(GAS_NAME, IO_NOW, (double_t)temp);

    temp = read_temperature();
    //LE_INFO("Temperature is: %lf",temp);
    io_PushNumeric(TEMPERATURE_NAME, IO_NOW, (double_t)temp);

    temp = read_humidity();
    //LE_INFO("Humidity is: %lf",temp);
    io_PushNumeric(HUMIDITY_NAME, IO_NOW, (double_t)temp);

    temp = read_pressure();
    //LE_INFO("Pressure is: %lf",temp);
    io_PushNumeric(PRESSURE_NAME, IO_NOW, (double_t)temp);
}

//--------------------------------------------------------------------------------------------------
/**
 * Call-back function called when an update is received from the Data Hub for the "period"
 * config setting.
 */
//--------------------------------------------------------------------------------------------------
static void PeriodUpdateHandler(
    double timestamp, ///< time stamp
    double value,     ///< period value, seconds
    void *contextPtr  ///< not used
)
//--------------------------------------------------------------------------------------------------
{
    LE_INFO("Received update to 'period' setting: %lf (timestamped %lf)", value, timestamp);

    uint32_t ms = (uint32_t)(value * 1000);

    if (ms == 0)
    {
        le_timer_Stop(Reader);
    }
    else
    {
        le_timer_SetMsInterval(Reader, ms);

        // If the sensor is enabled and the timer is not already running, start it now.
        if (!le_timer_IsRunning(Reader))
        {
            le_timer_Start(Reader);
        }
    }
}


//--------------------------------------------------------------------------------------------------
/**
 * Call-back for receiving notification that an update is happening.
 */
//--------------------------------------------------------------------------------------------------
static void UpdateStartEndHandler(
    bool isStarting, //< input is starting
    void *contextPtr //< Not used.
)
//--------------------------------------------------------------------------------------------------
{
    LE_INFO("Configuration update %s.", isStarting ? "starting" : "finished");
}


COMPONENT_INIT
{	
	LE_INFO("BME680 API started !!!");
    Seeed_BME680(BME680_I2C_ADDR_SECONDARY);

    le_result_t result;
    result = Seeed_BME680_init();
    LE_INFO("Init reusult : %d",result);

    Reader = le_timer_Create("Get Sample");
    LE_ASSERT_OK(le_timer_SetRepeat (Reader, 0));
    LE_ASSERT_OK(le_timer_SetMsInterval(Reader, 3 * 1000));
    LE_ASSERT_OK(le_timer_SetHandler(Reader, ReaderHandler));

    le_timer_Start(Reader);

    // Configure DataHub
    io_AddUpdateStartEndHandler(UpdateStartEndHandler, NULL);

     // This will be provided to the Data Hub.
    result = io_CreateInput(TEMPERATURE_NAME, IO_DATA_TYPE_NUMERIC, "C");
    LE_ASSERT(result == LE_OK);
    result = io_CreateInput(PRESSURE_NAME,    IO_DATA_TYPE_NUMERIC, "KPa");
    LE_ASSERT(result == LE_OK);
    result = io_CreateInput(HUMIDITY_NAME,    IO_DATA_TYPE_NUMERIC, "%");
    LE_ASSERT(result == LE_OK);
    result = io_CreateInput(GAS_NAME,         IO_DATA_TYPE_NUMERIC, "Kohms");
    LE_ASSERT(result == LE_OK);
    
    // This is my configuration setting.
    result = io_CreateOutput(PERIOD_NAME, IO_DATA_TYPE_NUMERIC, "s");
    LE_ASSERT(result == LE_OK);

    // Register for notification of updates to our configuration setting.
    io_AddNumericPushHandler(PERIOD_NAME, PeriodUpdateHandler, NULL);

    // Set the defaults: enable the sensor, set period to 1s
    io_SetNumericDefault(PERIOD_NAME, 2);

    LE_INFO("Init done !!!");
}