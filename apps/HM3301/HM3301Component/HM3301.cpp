/*
 * Seeed_HM330X.cpp
 * Driver for Seeed PM2.5 Sensor(HM300)
 *  
 * Copyright (c) 2018 Seeed Technology Co., Ltd.
 * Website    : www.seeed.cc
 * Author     : downey
 * Create Time: August 2018
 * Change Log :
 *
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "HM3301.h"

/*Global variable */
static const char hm3301_i2c_bus[20] = "/dev/i2c-5"; //< I2C bus
uint8_t buf[32] = {
    0,
};                            //< Buffer store data
size_t buf_len = sizeof(buf); //< Lenght of this buffer

void delay(uint32_t ms){
  usleep(ms*1000);
}

HM330X::HM330X(uint8_t IIC_ADDR)
{
    set_iic_addr(IIC_ADDR);
}

uint8_t HM330X::select_comm()
{
    return IIC_SEND_CMD(SELECT_COMM_CMD);
}

uint8_t HM330X::init()
{
    int8_t result = LE_OK;
    return result;
}

uint8_t HM330X::read_sensor_value(uint8_t *reg_data, uint32_t data_len)
{
    int8_t result = LE_OK;
	result = i2cReceiveBytes_v2(hm3301_i2c_bus, _IIC_ADDR, 0, buf, data_len);
    if(result != LE_OK)
        LE_INFO("Can't read: %d",result);
    for (uint32_t i = 0; i < data_len; i++)
    {
        reg_data[i] = (uint8_t)buf[i];
    }
    return result;
}

/**@brief I2C write byte
 * @param reg :Register address of operation object
 * @param byte :The byte to be wrote.
 * @return result of operation,non-zero if failed.
 * */
uint8_t IIC_OPRTS::IIC_write_byte(uint8_t reg_address, uint8_t byte)
{
    int32_t result = 0;
    uint8_t buffer[2] = {reg_address,byte};
    result = i2cSendBytes(hm3301_i2c_bus, _IIC_ADDR, buffer,2);
    if (!result)
        return LE_OK;
    else
        return LE_FAULT;
}

/**@brief I2C write 16bit value
 * @param reg: Register address of operation object
 * @param value: The 16bit value to be wrote .
 * @return result of operation,non-zero if failed.
 * */
uint8_t IIC_OPRTS::IIC_write_16bit(uint8_t reg_address, uint16_t value)
{
    int32_t result = 0;

    uint8_t buffer[3] = {reg_address,(uint8_t)(value>>8),(uint8_t)(value & 0xff)};
    result = i2cSendBytes(hm3301_i2c_bus, _IIC_ADDR, buffer,3);

    if (!result)
        return LE_OK;
    else
        return LE_FAULT;
}

/**@brief I2C read byte
 * @param reg: Register address of operation object
 * @param byte: The byte to be read in.
 * @return result of operation,non-zero if failed.
 * */
uint8_t IIC_OPRTS::IIC_read_byte(uint8_t reg_address, uint8_t *byte)
{
    int8_t result = LE_OK;

    result = i2cReceiveBytes_v2(hm3301_i2c_bus,_IIC_ADDR,reg_address,buf,1);
    if(result != LE_OK){
        LE_INFO("Can't read!");
        return LE_FAULT;
    }
    *byte = buf[0];
    return LE_OK;
}

/**@brief I2C read 16bit value
 * @param reg: Register address of operation object
 * @param byte: The 16bit value to be read in.
 * @return result of operation,non-zero if failed.
 * */
uint8_t IIC_OPRTS::IIC_read_16bit(uint8_t start_reg, uint16_t *value)
{
    int8_t result = LE_OK;
    *value = 0;

    result = i2cReceiveBytes_v2(hm3301_i2c_bus,_IIC_ADDR,start_reg,buf,2);
    if(result != LE_OK){
        LE_INFO("Can't read!");
        return LE_FAULT;
    }

    *value = (uint16_t)(buf[0]<<8) | buf[1];
    return LE_OK;
}

/**@brief I2C read some bytes
 * @param reg: Register address of operation object
 * @param data: The buf  to be read in.
 * @param data_len: The length of buf need to read in.
 * @return result of operation,non-zero if failed.
 * */
uint8_t IIC_OPRTS::IIC_read_bytes(uint8_t start_reg, uint8_t *reg_data, uint32_t data_len)
{
    int8_t result = LE_OK;

    result = i2cReceiveBytes_v2(hm3301_i2c_bus, _IIC_ADDR, start_reg, buf, data_len);
    if(result != LE_OK)
        LE_INFO("Can't read: %d",result);

    for (uint32_t i = 0; i < data_len; i++)
    {
        reg_data[i] = (uint8_t)buf[i];
    }
    return result;
}

/**@brief change the I2C address from default.
 * @param IIC_ADDR: I2C address to be set 
 * */
void IIC_OPRTS::set_iic_addr(uint8_t IIC_ADDR)
{
    _IIC_ADDR = IIC_ADDR;
}

uint8_t IIC_OPRTS::IIC_SEND_CMD(uint8_t CMD)
{
    int result = 0;
    
    result = i2cSendByte(hm3301_i2c_bus,_IIC_ADDR,CMD);
    if (!result)
        return LE_OK;
    else
        return LE_FAULT;
}
