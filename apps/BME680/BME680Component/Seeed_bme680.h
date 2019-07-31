#include "legato.h"
#include "interfaces.h"
#include "i2c-utils.h"
#include "bme680.h"

/**@brief Result of sensor's value.
 *
 */
typedef struct Result
{
    float temperature;  
    
    float pressure;
    
    float humidity;
    
    float gas;
	
}sensor_result_t;

void Seeed_BME680(uint8_t iic_addr);
bool  init();

int8_t read_sensor_data(void);
float  read_temperature(void);
float  read_pressure(void);
float  read_humidity(void);
float  read_gas(void);

int8_t iic_write(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len);
int8_t iic_read(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len);
static void delay_msec(uint32_t ms);