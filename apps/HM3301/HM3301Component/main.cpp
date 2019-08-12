/*
 * Example for Seeed PM2.5 Sensor(HM300)
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

static le_timer_Ref_t Reader;

#define dhubIO_DataType_t io_DataType_t

HM330X sensor;
uint8_t buff_t[30];

#define SENSORNUM_VALUE_NAME "hm3301/value/sensorNum"
#define PM1CF1_VALUE_NAME "hm3301/value/pm1_cf1"
#define PM25CF1_VALUE_NAME "hm3301/value/pm25_cf1"
#define PM10CF1_VALUE_NAME "hm3301/value/pm10_cf1"
#define PM1_VALUE_NAME "hm3301/value/pm1"
#define PM25_VALUE_NAME "hm3301/value/pm25"
#define PM10_VALUE_NAME "hm3301/value/pm10"

static void ReaderHandler(
    le_timer_Ref_t timer ///< Led state timer
)
{
    uint16_t value[8] = {
        0,
    };
    sensor.read_sensor_value(buff_t, 29);
    for (int8_t i = 0; i < 8; i++)
    {
        value[i] = (uint16_t)buff_t[i * 2] << 8 | buff_t[i * 2 + 1];
    }
    io_PushNumeric(SENSORNUM_VALUE_NAME, IO_NOW, (double_t)value[0]);
    io_PushNumeric(PM1CF1_VALUE_NAME,    IO_NOW, (double_t)value[1]);
    io_PushNumeric(PM25CF1_VALUE_NAME,   IO_NOW, (double_t)value[2]);
    io_PushNumeric(PM10CF1_VALUE_NAME,   IO_NOW, (double_t)value[3]);
    io_PushNumeric(PM1_VALUE_NAME,       IO_NOW, (double_t)value[4]);
    io_PushNumeric(PM25_VALUE_NAME,      IO_NOW, (double_t)value[5]);
    io_PushNumeric(PM10_VALUE_NAME,      IO_NOW, (double_t)value[6]);
}

COMPONENT_INIT
{
    LE_INFO("HM3301 sensor stated!!!");
    sensor.init();

    le_result_t result;
    result = io_CreateInput(SENSORNUM_VALUE_NAME, IO_DATA_TYPE_NUMERIC, "");
    LE_ASSERT(result == LE_OK);
    result = io_CreateInput(PM1CF1_VALUE_NAME,    IO_DATA_TYPE_NUMERIC, "ug/m3");
    LE_ASSERT(result == LE_OK);
    result = io_CreateInput(PM25CF1_VALUE_NAME,   IO_DATA_TYPE_NUMERIC, "ug/m3");
    LE_ASSERT(result == LE_OK);
    result = io_CreateInput(PM10CF1_VALUE_NAME,   IO_DATA_TYPE_NUMERIC, "ug/m3");
    LE_ASSERT(result == LE_OK);
    result = io_CreateInput(PM1_VALUE_NAME,       IO_DATA_TYPE_NUMERIC, "ug/m3");
    LE_ASSERT(result == LE_OK);
    result = io_CreateInput(PM25_VALUE_NAME,      IO_DATA_TYPE_NUMERIC, "ug/m3");
    LE_ASSERT(result == LE_OK);
    result = io_CreateInput(PM10_VALUE_NAME,      IO_DATA_TYPE_NUMERIC, "ug/m3");
    LE_ASSERT(result == LE_OK);

    /*Create timer to read data from sensor */
    Reader = le_timer_Create("Get Sample AK9753");
    LE_ASSERT_OK(le_timer_SetRepeat(Reader, 0));
    LE_ASSERT_OK(le_timer_SetMsInterval(Reader, 1000));
    LE_ASSERT_OK(le_timer_SetHandler(Reader, ReaderHandler));
    le_timer_Start(Reader);
}