/*
 * Seeed_HM330X.h
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

#ifndef _SEEED_HM330X_H
#define _SEEED_HM330X_H

#include "legato.h"
#include "interfaces.h"
#include "i2c-utils.h"

#define DEFAULT_IIC_ADDR 0x40
#define SELECT_COMM_CMD 0X88

class IIC_OPRTS
{
public:
    uint8_t IIC_write_byte(uint8_t reg_address, uint8_t byte);
    uint8_t IIC_read_byte(uint8_t reg_address, uint8_t *byte);
    void  set_iic_addr(uint8_t IIC_ADDR);
    uint8_t IIC_read_16bit(uint8_t start_reg, uint16_t *value);
    uint8_t IIC_write_16bit(uint8_t reg_address, uint16_t value);
    uint8_t IIC_read_bytes(uint8_t start_reg, uint8_t *reg_data, uint32_t data_len);
    uint8_t IIC_SEND_CMD(uint8_t CMD);


    uint8_t _IIC_ADDR;
};

class HM330X : public IIC_OPRTS
{
public:
    HM330X(uint8_t IIC_ADDR = DEFAULT_IIC_ADDR);
    uint8_t init();
    uint8_t select_comm();
    uint8_t read_sensor_value(uint8_t *reg_data, uint32_t data_len);

private:
};

#endif