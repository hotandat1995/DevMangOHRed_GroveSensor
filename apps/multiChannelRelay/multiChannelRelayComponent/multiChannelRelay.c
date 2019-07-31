#include "legato.h"
#include "interfaces.h"
#include "i2c-utils.h"

#define GROVE_DEFAULT_ADDR	0x11

#define CHANNLE1_BIT  0x01
#define CHANNLE2_BIT  0x02
#define CHANNLE3_BIT  0x04
#define CHANNLE4_BIT  0x08
#define CHANNLE5_BIT  0x10
#define CHANNLE6_BIT  0x20
#define CHANNLE7_BIT  0x40
#define CHANNLE8_BIT  0x80

#define CMD_CHANNEL_CTRL					0x10
#define CMD_SAVE_I2C_ADDR					0x11
#define CMD_READ_I2C_ADDR					0x12
#define CMD_READ_FIRMWARE_VER				0x13

/*Global variable */
static const char multi_relay_i2c_bus[256] = "/dev/i2c-4";	//< I2C bus
uint8_t buf[29]={0,};										//< Buffer store data
size_t  buf_len = sizeof(buf);								//< Lenght of this buffer

uint8_t channel_state = 0;
uint8_t cur_channel_state = 0;
static int multi_relayAddress = GROVE_DEFAULT_ADDR;				//Set defaut address


/**************************************************************************************************/
/*!
    Reads an 8 bit value over I2C
*/
/**************************************************************************************************/
uint8_t read8
(
	uint8_t address,	//< address you want to sent data
	uint8_t CMD_grove	//< command data
)
{
	i2cSendByte    (multi_relay_i2c_bus, address, CMD_grove);
	i2cReceiveBytes(multi_relay_i2c_bus, address, buf, buf_len);
	return buf[0];
}

/**************************************************************************************************/
/*!
    Write an 8 bit value over I2C to address
*/
/**************************************************************************************************/
le_result_t write8 
(
	uint8_t  address,	//< Address of device in I2C bus
	uint8_t  reg, 		//< Registry
	uint32_t value		//< Data
)
/**************************************************************************************************/
{	
	__u8 buffer[2] = {0,0};
	buffer[0] = reg;
	buffer[1] = value & 0xFF;
	i2cSendBytes(multi_relay_i2c_bus, address,buffer,2);
	return LE_OK;
}
/**************************************************************************************************/
/*!
    Get firmware version from device
*/
/**************************************************************************************************/
uint8_t getFirmwareVersion(void){
	read8(multi_relayAddress,CMD_READ_FIRMWARE_VER);
	return buf[0];
}

/**************************************************************************************************/
/*!
    Change device address in I2C bus
*/
/**************************************************************************************************/
void changeI2CAddress
(
	uint8_t old_addr, 	//< Old address
	uint8_t new_addr	//< New address
)
/**************************************************************************************************/
{  
	write8(multi_relayAddress,CMD_SAVE_I2C_ADDR,new_addr);
	multi_relayAddress = new_addr;
}

/**************************************************************************************************/
/*!
    Get all channel state
*/
/**************************************************************************************************/
uint8_t multiChannelRelay_getChannelState()
{
	return cur_channel_state & 0x0f;
}

/**************************************************************************************************/
/*!
    Turn ON channel (can turn ON all multi channel at the same time by use operator " | ")
*/
/**************************************************************************************************/
void multiChannelRelay_turn_on_channel
(
	uint8_t channel		//< Channel need to turn ON
)
/**************************************************************************************************/
{
	cur_channel_state = channel;
	channel_state |= (1 << ((channel &0x0f)-1));
	write8(multi_relayAddress,CMD_CHANNEL_CTRL,channel);
	//LE_INFO("Channel state: %x",channel_state);
}

/**************************************************************************************************/
/*!
    Turn OFF channel (can turn Off all multi channel at the same time by use operator " | ")
*/
/**************************************************************************************************/
void multiChannelRelay_turn_off_channel
(
	uint8_t channel		//< Channel need to turn OFF
)
/**************************************************************************************************/
{
	cur_channel_state = channel;
	channel_state &= ~(1 << (channel-1));
	write8(multi_relayAddress,CMD_CHANNEL_CTRL,channel);
	//LE_INFO("Channel state: %x",channel_state);
}

/* 	
	On going because it this code is not good for this case:
	It use when in I2C bus is only one multi-relay device connected in this.
	When more than one device it not true (if multi-relay address is > another divice address)
*/
uint8_t scan_device(void){
	int error = 0;
	int result = 0;
	int nDevice = 0;
	for(int i=0x03;i<0x77;i++){
		error = i2cReceiveBytes(multi_relay_i2c_bus, i, buf, buf_len);
		if(error == 0) {
			result = i;
			LE_INFO("Address found at 0x%2x",i);
			nDevice++;
		}
		if(error == 4)
			LE_INFO("Unknow error at address 0x%2x",i);
	}
	if(nDevice == 0){
		LE_INFO("No devices found in I2C bus!");
	}
	else{
		LE_INFO("Number of device found: %d",nDevice);
		if(nDevice != 1) return 0x00;
	}
	return result;
}

void openI2CHub(){
	i2cSendByte(multi_relay_i2c_bus,0x71,0x0f);
}

COMPONENT_INIT
{	
	openI2CHub();
	LE_INFO("Multi-channel relay API started!");
}