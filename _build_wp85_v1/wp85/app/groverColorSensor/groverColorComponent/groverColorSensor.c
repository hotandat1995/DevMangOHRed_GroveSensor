#include "legato.h"
#include "interfaces.h"
#include "i2c-utils.h"
#include <math.h>

#define TCS34725_ADDRESS          (0x29)

#define TCS34725_COMMAND_BIT      (0x80)

#define TCS34725_ENABLE           (0x00)
#define TCS34725_ENABLE_AIEN      (0x10)    /* RGBC Interrupt Enable */
#define TCS34725_ENABLE_WEN       (0x08)    /* Wait enable - Writing 1 activates the wait timer */
#define TCS34725_ENABLE_AEN       (0x02)    /* RGBC Enable - Writing 1 actives the ADC, 0 disables it */
#define TCS34725_ENABLE_PON       (0x01)    /* Power on - Writing 1 activates the internal oscillator, 0 disables it */
#define TCS34725_ATIME            (0x01)    /* Integration time */
#define TCS34725_WTIME            (0x03)    /* Wait time (if TCS34725_ENABLE_WEN is asserted) */
#define TCS34725_WTIME_2_4MS      (0xFF)    /* WLONG0 = 2.4ms   WLONG1 = 0.029s */
#define TCS34725_WTIME_204MS      (0xAB)    /* WLONG0 = 204ms   WLONG1 = 2.45s  */
#define TCS34725_WTIME_614MS      (0x00)    /* WLONG0 = 614ms   WLONG1 = 7.4s   */
#define TCS34725_AILTL            (0x04)    /* Clear channel lower interrupt threshold */
#define TCS34725_AILTH            (0x05)
#define TCS34725_AIHTL            (0x06)    /* Clear channel upper interrupt threshold */
#define TCS34725_AIHTH            (0x07)
#define TCS34725_PERS             (0x0C)    /* Persistence register - basic SW filtering mechanism for interrupts */
#define TCS34725_PERS_NONE        (0b0000)  /* Every RGBC cycle generates an interrupt                                */
#define TCS34725_PERS_1_CYCLE     (0b0001)  /* 1 clean channel value outside threshold range generates an interrupt   */
#define TCS34725_PERS_2_CYCLE     (0b0010)  /* 2 clean channel values outside threshold range generates an interrupt  */
#define TCS34725_PERS_3_CYCLE     (0b0011)  /* 3 clean channel values outside threshold range generates an interrupt  */
#define TCS34725_PERS_5_CYCLE     (0b0100)  /* 5 clean channel values outside threshold range generates an interrupt  */
#define TCS34725_PERS_10_CYCLE    (0b0101)  /* 10 clean channel values outside threshold range generates an interrupt */
#define TCS34725_PERS_15_CYCLE    (0b0110)  /* 15 clean channel values outside threshold range generates an interrupt */
#define TCS34725_PERS_20_CYCLE    (0b0111)  /* 20 clean channel values outside threshold range generates an interrupt */
#define TCS34725_PERS_25_CYCLE    (0b1000)  /* 25 clean channel values outside threshold range generates an interrupt */
#define TCS34725_PERS_30_CYCLE    (0b1001)  /* 30 clean channel values outside threshold range generates an interrupt */
#define TCS34725_PERS_35_CYCLE    (0b1010)  /* 35 clean channel values outside threshold range generates an interrupt */
#define TCS34725_PERS_40_CYCLE    (0b1011)  /* 40 clean channel values outside threshold range generates an interrupt */
#define TCS34725_PERS_45_CYCLE    (0b1100)  /* 45 clean channel values outside threshold range generates an interrupt */
#define TCS34725_PERS_50_CYCLE    (0b1101)  /* 50 clean channel values outside threshold range generates an interrupt */
#define TCS34725_PERS_55_CYCLE    (0b1110)  /* 55 clean channel values outside threshold range generates an interrupt */
#define TCS34725_PERS_60_CYCLE    (0b1111)  /* 60 clean channel values outside threshold range generates an interrupt */
#define TCS34725_CONFIG           (0x0D)
#define TCS34725_CONFIG_WLONG     (0x02)    /* Choose between short and long (12x) wait times via TCS34725_WTIME */
#define TCS34725_CONTROL          (0x0F)    /* Set the gain level for the sensor */
#define TCS34725_ID               (0x12)    /* 0x44 = TCS34721/TCS34725, 0x4D = TCS34723/TCS34727 */
#define TCS34725_STATUS           (0x13)
#define TCS34725_STATUS_AINT      (0x10)    /* RGBC Clean channel interrupt */
#define TCS34725_STATUS_AVALID    (0x01)    /* Indicates that the RGBC channels have completed an integration cycle */
#define TCS34725_CDATAL           (0x14)    /* Clear channel data */
#define TCS34725_CDATAH           (0x15)
#define TCS34725_RDATAL           (0x16)    /* Red channel data */
#define TCS34725_RDATAH           (0x17)
#define TCS34725_GDATAL           (0x18)    /* Green channel data */
#define TCS34725_GDATAH           (0x19)
#define TCS34725_BDATAL           (0x1A)    /* Blue channel data */
#define TCS34725_BDATAH           (0x1B)

typedef enum
{
  TCS34725_INTEGRATIONTIME_2_4MS  = 0xFF,   /**<  2.4ms - 1 cycle    - Max Count: 1024  */
  TCS34725_INTEGRATIONTIME_24MS   = 0xF6,   /**<  24ms  - 10 cycles  - Max Count: 10240 */
  TCS34725_INTEGRATIONTIME_50MS   = 0xEB,   /**<  50ms  - 20 cycles  - Max Count: 20480 */
  TCS34725_INTEGRATIONTIME_101MS  = 0xD5,   /**<  101ms - 42 cycles  - Max Count: 43008 */
  TCS34725_INTEGRATIONTIME_154MS  = 0xC0,   /**<  154ms - 64 cycles  - Max Count: 65535 */
  TCS34725_INTEGRATIONTIME_700MS  = 0x00    /**<  700ms - 256 cycles - Max Count: 65535 */
}
tcs34725IntegrationTime_t;

typedef enum
{
  TCS34725_GAIN_1X                = 0x00,   /**<  No gain  */
  TCS34725_GAIN_4X                = 0x01,   /**<  4x gain  */
  TCS34725_GAIN_16X               = 0x02,   /**<  16x gain */
  TCS34725_GAIN_60X               = 0x03    /**<  60x gain */
}
tcs34725Gain_t;

/*Global variable */
char color_sensor_i2c_bus[256] = "/dev/i2c-1";	//< I2C bus
uint8_t buf[29]={0,};							//< Buffer store data
size_t  buf_len = sizeof(buf);					//< Lenght of this buffer

bool 					  _tcs34725Initialised     = false;
tcs34725IntegrationTime_t _tcs34725IntegrationTime = TCS34725_INTEGRATIONTIME_2_4MS;
tcs34725Gain_t 		      _tcs34725Gain            = TCS34725_GAIN_1X;

/*Prototype */
bool begin(void);
le_result_t clearInterrupt(void);

/**************************************************************************************************/
/*!
    @brief  Implements missing powf function
*/
/**************************************************************************************************/
float powf(const float x, const float y)
{
  	return (float)(pow((double)x, (double)y));
}
/**************************************************************************************************/
/*!
    @brief  Implements missing max function
*/
/**************************************************************************************************/
uint16_t max(uint16_t a,uint16_t b){
	return (a>b)? a:b;
}
/**************************************************************************************************/
/*!
    @brief  Implements missing min function
*/
/**************************************************************************************************/
uint16_t min(uint16_t a,uint16_t b){
	return (a>b)? b:a;
}
/**************************************************************************************************/
/*!
    @brief  Writes a register and an 8 bit value over I2C
*/
/**************************************************************************************************/
le_result_t write8 
(
	uint8_t reg, 		//< Registry
	uint32_t value		//< Data
)
/**************************************************************************************************/
{	
	__u8 buffer[2] = {0,0};
	buffer[0] = TCS34725_COMMAND_BIT | reg;
	buffer[1] = value & 0xFF;
	i2cSendBytes(color_sensor_i2c_bus, TCS34725_ADDRESS,buffer,2);
	return LE_OK;
}

/**************************************************************************************************/
/*!
    @brief  Reads an 8 bit value over I2C
*/
/**************************************************************************************************/
uint8_t read8(uint8_t reg)
{
	i2cSendByte(color_sensor_i2c_bus, TCS34725_ADDRESS, TCS34725_COMMAND_BIT | reg);
	i2cReceiveBytes(color_sensor_i2c_bus, TCS34725_ADDRESS, buf, buf_len);
	return buf[0];
}

/**************************************************************************************************/
/*!
    @brief  Reads a 16 bit values over I2C
*/
/**************************************************************************************************/
int16_t read16(uint8_t reg)
{
	uint16_t x; uint16_t t;
	
	int res = 0;
	res = i2cSendByte(color_sensor_i2c_bus, TCS34725_ADDRESS, TCS34725_COMMAND_BIT | reg);
	if (res != 0){
		LE_INFO("Sent Fail!");
        return LE_FAULT;
	}
	else
		LE_INFO("Sent Done!");

	res = i2cReceiveBytes(color_sensor_i2c_bus, TCS34725_ADDRESS, buf, buf_len);

	t = buf[0];
	x = buf[1];
	x <<= 8;
	x |= t;
	return x;
}

/**************************************************************************************************/
/*!
    Enables the device
*/
/**************************************************************************************************/
void enable(void)
{
	write8(TCS34725_ENABLE, TCS34725_ENABLE_PON);
	usleep(3);
	write8(TCS34725_ENABLE, TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN);  
}

/**************************************************************************************************/
/*!
    Disables the device (putting it in lower power sleep mode)
*/
/**************************************************************************************************/
void disable(void)
{
	/* Turn the device off to save power */
	uint8_t reg = 0;
	reg = read8(TCS34725_ENABLE);
	write8(TCS34725_ENABLE, reg & ~(TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN));
}

/**************************************************************************************************/
/*!
    Setup when begin collect data
*/
/**************************************************************************************************/
void initialize(tcs34725IntegrationTime_t it, tcs34725Gain_t gain) 
{	
	_tcs34725Initialised     = false;
	_tcs34725IntegrationTime = it;
	_tcs34725Gain            = gain;
	LE_INFO("Init status: %d %d %d\n",_tcs34725Initialised,_tcs34725IntegrationTime,_tcs34725Gain);
}

/**************************************************************************************************/
/*!
    Sets the integration time for the TC34725
*/
/**************************************************************************************************/
void setIntegrationTime(tcs34725IntegrationTime_t it)
{
	if (!_tcs34725Initialised) 
	begin();

	/* Update the timing register */
	write8(TCS34725_ATIME, it);

	/* Update value placeholders */
	_tcs34725IntegrationTime = it;
}

/**************************************************************************************************/
/*!
    Adjusts the gain on the TCS34725 (adjusts the sensitivity to light)
*/
/**************************************************************************************************/
void setGain(tcs34725Gain_t gain)
{
	if (!_tcs34725Initialised) 
	begin();

	/* Update the timing register */
	write8(TCS34725_CONTROL, gain);

	/* Update value placeholders */
	_tcs34725Gain = gain;
}


/**************************************************************************************************/
/*!
    @brief  Reads the raw red, green, blue and clear channel values
*/
/**************************************************************************************************/
void getRawData (
	uint16_t *r, 		//< Red value
	uint16_t *g, 		//< Green value
	uint16_t *b, 		//< Blue value
	uint16_t *c			//< Clear value
)
{
	if (!_tcs34725Initialised) 
	begin();

	*c = read16(TCS34725_CDATAL);
	*r = read16(TCS34725_RDATAL);
	*g = read16(TCS34725_GDATAL);
	*b = read16(TCS34725_BDATAL);
	LE_INFO("Raw data is:");
	LE_INFO("RC-%d RR-%d RG-%d RB-%d",(int)*c,(int)*r,(int)*g,(int)*b);
	
	/* Set a sleep for the integration time */
	switch (_tcs34725IntegrationTime)
	{
		case TCS34725_INTEGRATIONTIME_2_4MS:
		usleep(3);
		break;
		case TCS34725_INTEGRATIONTIME_24MS:
		usleep(24);
		break;
		case TCS34725_INTEGRATIONTIME_50MS:
		usleep(50);
		break;
		case TCS34725_INTEGRATIONTIME_101MS:
		usleep(101);
		break;
		case TCS34725_INTEGRATIONTIME_154MS:
		usleep(154);
		break;
		case TCS34725_INTEGRATIONTIME_700MS:
		usleep(700);
		break;
	}
	//disable();
	LE_INFO("Done get raw!!!\n");
}

/**************************************************************************************************/
/*!
    Initializes I2C and configures the sensor (call this function before
    doing anything else)
*/
/**************************************************************************************************/
bool begin(void) 
{	
	/* Make sure we're actually connected */
	uint8_t x = read8(TCS34725_ID);
	LE_INFO("Add:%x ;ID: %x",TCS34725_ADDRESS,x);
	if ((x != 0x44) && (x != 0x4D))
	{
		LE_INFO("Sensor not found!!!");
		return false;
	}else{
		LE_INFO("Begining!!!");
	}
	_tcs34725Initialised = true;

	/* Set default integration time and gain */
	setIntegrationTime(_tcs34725IntegrationTime);
	setGain(_tcs34725Gain);

	/* Note: by default, the device is in power down mode on bootup */
	enable();

	return true;
}

/**************************************************************************************************/
/*!
    @brief  Converts the raw R/G/B values to color temperature in degrees Kelvin
*/
/**************************************************************************************************/
uint16_t calculateColorTemperature
(
	uint16_t r, //<red value
	uint16_t g, //<green value
	uint16_t b	//<blue value
)
{
	float X, Y, Z;      /* RGB to XYZ correlation      */
	float xc, yc;       /* Chromaticity co-ordinates   */
	float n;            /* McCamy's formula            */
	float cct;

	/* 1. Map RGB values to their XYZ counterparts.    */
	/* Based on 6500K fluorescent, 3000K fluorescent   */
	/* and 60W incandescent values for a wide range.   */
	/* Note: Y = Illuminance or lux                    */
	X = (-0.14282F * r) + (1.54924F * g) + (-0.95641F * b);
	Y = (-0.32466F * r) + (1.57837F * g) + (-0.73191F * b);
	Z = (-0.68202F * r) + (0.77073F * g) + ( 0.56332F * b);

	/* 2. Calculate the chromaticity co-ordinates      */
	xc = (X) / (X + Y + Z);
	yc = (Y) / (X + Y + Z);
	
	LE_INFO("2-D Cordinates: X= %f, Y= %f",(double)xc,(double)yc);
	/* 3. Use McCamy's formula to determine the CCT    */
	n = (xc - 0.3320F) / (0.1858F - yc);

	/* Calculate the final CCT */
	cct = (449.0F * powf(n, 3)) + (3525.0F * powf(n, 2)) + (6823.3F * n) + 5520.33F;

	/* Return the results in degrees Kelvin */
	return (uint16_t)cct;
}

/**************************************************************************************************/
/*!
    @brief  Converts the raw R/G/B values to lux
*/
/**************************************************************************************************/
uint16_t calculateLux(uint16_t r, uint16_t g, uint16_t b)
{
	float illuminance;

	/* This only uses RGB ... how can we integrate clear or calculate lux */
	/* based exclusively on clear since this might be more reliable?      */
	illuminance = (-0.32466F * r) + (1.57837F * g) + (-0.73191F * b);

	return (uint16_t)illuminance;
}
/**************************************************************************************************/
/*!
    @brief  Set interupt for module
*/
/**************************************************************************************************/
void setInterrupt(bool i) {
  uint8_t r = read8(TCS34725_ENABLE);
  if (i) {
    r |= TCS34725_ENABLE_AIEN;
  } else {
    r &= ~TCS34725_ENABLE_AIEN;
  }
  write8(TCS34725_ENABLE, r);
}
/**************************************************************************************************/
/*!
    @brief  Clear interupt for module
*/
/**************************************************************************************************/
le_result_t clearInterrupt(void) {
	int res = 0;
	res = i2cSendByte(color_sensor_i2c_bus, TCS34725_ADDRESS, TCS34725_COMMAND_BIT | 0x66);
	if(res != 0) return LE_FAULT;
	return LE_OK;
}
/**************************************************************************************************/
/*!
    @brief  Set interupt limit for module
*/
/**************************************************************************************************/
void setIntLimits(uint16_t low, uint16_t high) {
	write8(0x04, low & 0xFF);
	write8(0x05, low >> 8);
	write8(0x06, high & 0xFF);
	write8(0x07, high >> 8);
	LE_INFO("Set limit done!!!");
}

/**************************************************************************************************/
/*!
    @brief  Converts the raw R/G/B values to hex code
*/
/**************************************************************************************************/
void convertToHexCode
(
	uint16_t *red_,		//< Raw red data
	uint16_t *green_,	//< Raw green data
	uint16_t *blue_		//< Raw blue data
)
/**************************************************************************************************/
{
	double tmp;
	int maxColor;

	uint16_t red   = *red_;
	uint16_t green = *green_; 
	uint16_t blue  = *blue_;
	
	red  = red  * 1.70;
	blue = blue * 1.35;

	maxColor = max(red, green);
	maxColor = max(maxColor, blue);
	
	if(maxColor > 255)
	{
		tmp = 250.0/maxColor;
		green	*= tmp;
		red 	*= tmp;
		blue	*= tmp;
	}
	
	int minColor = min(red, green);
	minColor     = min(minColor, blue);
	maxColor     = max(red, green);
	maxColor     = max(maxColor, blue);
	
	int greenTmp = green;
	int redTmp 	 = red;
	int blueTmp	 = blue;
	
	//when turn on LED, need to adjust the RGB data,otherwise it is almost the white color
	if(red < 0.8*maxColor && red >= 0.6*maxColor)
	{
		red *= 0.4;
    }
	else if(red < 0.6*maxColor)
	{
		red *= 0.2;
    }
	
	if(green < 0.8*maxColor && green >= 0.6*maxColor)
	{
		green *= 0.4;
    }
	else if(green < 0.6*maxColor)
	{
		if (maxColor == redTmp && greenTmp >= 2*blueTmp && greenTmp >= 0.2*redTmp)		//orange
		{
			green *= 5;
		}
		green *= 0.2;
    }
	
	if(blue < 0.8*maxColor && blue >= 0.6*maxColor)
	{
		blue *= 0.4;
    }
	else if(blue < 0.6*maxColor)
	{
		if (maxColor == redTmp && greenTmp >= 2*blueTmp && greenTmp >= 0.2*redTmp)		//orange
		{
			blue *= 0.5;
		}
		if (maxColor == redTmp && greenTmp <= blueTmp && blueTmp >= 0.2*redTmp)			//pink
		{
			blue  *= 5;
		}
		blue *= 0.2;
    }
	
	minColor = min(red, green);
	minColor = min(minColor, blue);
	if(maxColor == green && red >= 0.85*maxColor && minColor == blue)					//yellow
	{
		red = maxColor;
		blue *= 0.4;
    }
	*red_   = red;
	*green_ = green;
	*blue_  = blue;
	LE_INFO("After adjust the RGB raw data");
	LE_INFO("Red: %d, Green: %d, Blue: %d",(int)red,(int)green,(int)blue);
}

COMPONENT_INIT
{
	LE_INFO("Color sensor begining!!!\n");
    uint16_t r, g, b, c, colorTemp, lux;
	initialize(TCS34725_INTEGRATIONTIME_154MS,TCS34725_GAIN_16X);
	getRawData(&r, &g, &b, &c);
	LE_INFO("Red: %d, Green: %d, Blue: %d, Clear: %d\n",r,g,b,c);
	
	convertToHexCode(&r,&g,&b);

	colorTemp = calculateColorTemperature(r, g, b);
	LE_INFO("colorTemp: %d\n",colorTemp);

	lux = calculateLux(r, g, b);
	LE_INFO("lux: %d\n",lux);
}