/**
 * @file gpioPin.c
 *
 * This is sample Legato CF3 GPIO app by using le_gpio.api.
 *
 * <HR>
 *
 * Copyright (C) Sierra Wireless, Inc.
 */

/* Legato Framework */
#include "legato.h"
#include "interfaces.h"
//--------------------------------------------------------------------------------------------------
/**
 * Because range of sensor is 3 meters, speed of sonic wave is 343.2 Mps
 * So timeout = 3.5 / 343.2 = 0.010198 second = 10198 micro second
 */
//--------------------------------------------------------------------------------------------------
static uint64_t timeout = 12000L;
static le_timer_Ref_t Reader;

//--------------------------------------------------------------------------------------------------
/**
 * Convenience function to get current time as uint64_t.
 *
 * @return
 *      Current time as a uint64_t
 */
//--------------------------------------------------------------------------------------------------
static uint64_t GetCurrentTimestamp(void)
{
    struct timeval tv;
    uint64_t utcMilliSec;
    if(gettimeofday(&tv, NULL) == 0){
        utcMilliSec = tv.tv_sec * 1000000 + tv.tv_usec;
    }
    else{
        LE_INFO("Can't get time!");
    }
    return utcMilliSec;
}

static uint64_t MicrosDiff(uint64_t begin, uint64_t end)
{
	return end - begin;
}

static uint64_t pulseIn(uint64_t state)
{
	uint64_t begin = GetCurrentTimestamp();

	// wait for any previous pulse to end
	while (le_gpioPin8_Read()) if (MicrosDiff(begin, GetCurrentTimestamp()) >= timeout) return 0;
	
    // wait for the pulse to start
	while (!le_gpioPin8_Read()) if (MicrosDiff(begin, GetCurrentTimestamp()) >= timeout) return 0;
	uint64_t pulseBegin = GetCurrentTimestamp();
	
	// wait for the pulse to stop
	while (le_gpioPin8_Read()) if (MicrosDiff(begin, GetCurrentTimestamp()) >= timeout) return 0;
	uint64_t pulseEnd = GetCurrentTimestamp();
	
	return MicrosDiff(pulseBegin, pulseEnd);
}

/*The measured distance from the range 0 to 400 Centimeters*/
long MeasureInCentimeters(void)
{
	
    le_gpioPin8_SetPushPullOutput(LE_GPIOPIN8_ACTIVE_HIGH, true);
    le_gpioPin8_EnablePullUp();

    le_gpioPin8_Deactivate();
    usleep(2);
    le_gpioPin8_Activate();
    usleep(10);
    le_gpioPin8_Deactivate();

    le_gpioPin8_SetInput(LE_GPIOPIN8_ACTIVE_HIGH);

	long duration;
	duration = pulseIn(1);
	long RangeInCentimeters;
	RangeInCentimeters = duration/29/2;
    LE_INFO("Duration: %lu microSecond ",duration);
	return RangeInCentimeters;
    return 0;
}

/*The measured distance from the range 0 to 400 Centimeters*/
float MeasureInCentimeters_v2(void)
{
    int _TIMEOUT1 = 1000;
    int _TIMEOUT2 = 10000;

    le_gpioPin8_EnablePullUp();
    le_gpioPin8_SetPushPullOutput(LE_GPIOPIN8_ACTIVE_HIGH, true);
    le_gpioPin8_EnablePullUp();

    le_gpioPin8_Deactivate();
    usleep(2);
    le_gpioPin8_Activate();
    usleep(10);
    le_gpioPin8_Deactivate();

    le_gpioPin8_SetInput(LE_GPIOPIN8_ACTIVE_HIGH);

    uint64_t t0 = GetCurrentTimestamp();
    int count = 0;

    while(count < _TIMEOUT1){
        if(le_gpioPin8_Read()) break;
        count++;
    }
    if(count >= _TIMEOUT1) {
        //LE_INFO("Read timeout 1");
        return 0;
    }

    count = 0;
    uint64_t t1 = GetCurrentTimestamp();
    while(count < _TIMEOUT2){
        if(!le_gpioPin8_Read()) break;
        count++;
    }
    if(count >= _TIMEOUT2){
        //LE_INFO("Read timeout 2");
        return 1;
    }

    uint64_t t2 = GetCurrentTimestamp();
    if((t1-t0)*1000000 > 530) {
        //LE_INFO("Can't read");
        return 1;
    }

    return (((float)t2-(float)t1)/29/2);
}

/*The measured distance from the range 0 to 157 Inches*/
long MeasureInInches(void)
{
    le_gpioPin8_EnablePullUp();
    le_gpioPin8_SetPushPullOutput(LE_GPIOPIN8_ACTIVE_HIGH, true);

    le_gpioPin8_Deactivate();
    usleep(2);
    le_gpioPin8_Activate();
    usleep(10);
    le_gpioPin8_Deactivate();
    le_gpioPin8_SetInput(LE_GPIOPIN8_ACTIVE_HIGH);

	long duration;
	duration = pulseIn(1);
	long RangeInInches;
	RangeInInches = duration/74/2;
	return RangeInInches;
    return 0;
}

static void ReaderHandler
(
    le_timer_Ref_t timer  ///< Reader timer
)
{
    float distant = 0;
    distant = MeasureInCentimeters_v2();
    if(distant == 1){
	    LE_INFO("Read fail 1!");
    }else if(distant > 1){
        LE_INFO("Distant is : %f",distant);
    }
    else{
        LE_INFO("Read fail !");
    }
}


COMPONENT_INIT
{
    LE_INFO("Start Ultra Sonic!");
    
    //Setup timer to read data
    Reader = le_timer_Create("Get Sample");
    LE_ASSERT_OK(le_timer_SetRepeat (Reader, 0));
    LE_ASSERT_OK(le_timer_SetMsInterval(Reader, 1000));
    LE_ASSERT_OK(le_timer_SetHandler(Reader, ReaderHandler));
    le_timer_Start(Reader);
}
