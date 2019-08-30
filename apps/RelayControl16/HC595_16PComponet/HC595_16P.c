#include "HC595_16P.h"

static uint16_t State_16_Channel = 0x0000;

static void init_HC595Pin
(
    void
)
{
    LE_FATAL_IF(le_HC595_clock_SetPushPullOutput(LE_HC595_CLOCK_ACTIVE_LOW,true) != LE_OK,
                "Couldn't configure HC595 clock pin as a push pull output");
    LE_FATAL_IF(le_HC595_data_SetPushPullOutput (LE_HC595_DATA_ACTIVE_LOW,true) != LE_OK,
                "Couldn't configure HC595 data pin as a push pull output");
    LE_FATAL_IF(le_HC595_latch_SetPushPullOutput(LE_HC595_LATCH_ACTIVE_LOW,true) != LE_OK,
                "Couldn't configure HC595 latch pin as a push pull output");
    // If you have reset pin
    // LE_FATAL_IF(le_HC595_reset_SetPushPullOutput(LE_HC595_RESET_ACTIVE_LOW,true) != LE_OK,
    //             "Couldn't configure HC595 reset pin as a push pull output");
   
    //Set default state off for all channel
    HC595_SetData(State_16_Channel);
    LE_INFO("Init pin succesful");
}

static void HC595_SetBit
(
    uint16_t bit
)
{
    if(bit == 0)
    {
        le_HC595_data_Deactivate();
        le_HC595_clock_Activate();
        usleep(5);
        le_HC595_clock_Deactivate();
    }
    else if (bit == 1)
    {
        le_HC595_data_Activate();
        le_HC595_clock_Activate();
        usleep(5);
        le_HC595_clock_Deactivate();
    }
    else
    {
        LE_ERROR("Wrong data!");
    }
    
}

static void HC595_SetData
(
    uint16_t new_16ChannelState
)
{
    State_16_Channel = new_16ChannelState;
    for(int i=0;i<16;i++)
    {
        HC595_SetBit((new_16ChannelState >> i) & 0x0001);
    }
    le_HC595_latch_Activate();
    usleep(5);
    le_HC595_latch_Deactivate();
}

uint16_t HC595_CurrentState
(
    void
)
{
    return State_16_Channel;
}

