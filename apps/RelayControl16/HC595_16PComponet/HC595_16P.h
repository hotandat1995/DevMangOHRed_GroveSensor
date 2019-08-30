#include "legato.h"
#include "interfaces.h"

#ifndef __HC595_16P_H__
#define __GROVE_HUMAN_PRESENCE_SENSOR_H__



static void init_HC595Pin(void);
static void HC595_SetBit(uint16_t bit);
static void HC595_SetData(uint16_t new_16ChannelState);
uint16_t HC595_CurrentState(void);

#endif