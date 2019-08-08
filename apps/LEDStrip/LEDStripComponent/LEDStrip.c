
#include "LEDStip.h"

static le_timer_Ref_t Reader;


void RGBdriver_init()
{
    le_clk_SetPushPullOutput(LE_CLK_ACTIVE_HIGH, true);
    le_sdin_SetPushPullOutput(LE_SDIN_ACTIVE_HIGH, true);
}

void begin(void)
{
    Send32Zero();
}

void end(void)
{
    Send32Zero();
}

void ClkRise(void)
{
    le_clk_Deactivate();
    usleep(20);
    le_clk_Activate();
    usleep(20);
}

void Send32Zero(void)
{
    unsigned char i;

    for (i = 0; i < 32; i++)
    {
        le_sdin_Deactivate();
        ClkRise();
    }
}

uint8_t TakeAntiCode(uint8_t dat)
{
    uint8_t tmp = 0;

    if ((dat & 0x80) == 0)
    {
        tmp |= 0x02;
    }

    if ((dat & 0x40) == 0)
    {
        tmp |= 0x01;
    }

    return tmp;
}

// gray data
void DatSend(uint32_t dx)
{
    uint8_t i;

    for (i = 0; i < 32; i++)
    {
        if ((dx & 0x80000000) != 0)
        {
            le_sdin_Activate();
        }
        else
        {
            le_sdin_Deactivate();
        }

        dx <<= 1;
        ClkRise();
    }
}

// Set color
void SetColor(uint8_t Red, uint8_t Green, uint8_t Blue)
{
    uint32_t dx = 0;

    dx |= (uint32_t)0x03 << 30; // highest two bits 1，flag bits
    dx |= (uint32_t)TakeAntiCode(Blue) << 28;
    dx |= (uint32_t)TakeAntiCode(Green) << 26;
    dx |= (uint32_t)TakeAntiCode(Red) << 24;

    dx |= (uint32_t)Blue << 16;
    dx |= (uint32_t)Green << 8;
    dx |= Red;

    DatSend(dx);
}
/*Delay millisecond */
static delay(uint8_t ms){
    usleep(ms*1000);
}

static void ReaderHandler(
    le_timer_Ref_t timer ///< Reader timer
)
{
    for (int i = 0; i < 256; i++)
    {
        begin();           // begin
        SetColor(0, 0, i); //Blue. First node data. SetColor(R,G,B)
        end();
        delay(10);
    }
    for (int i = 255; i > 0; i--)
    {
        begin();           // begin
        SetColor(0, 0, i); //Blue. first node data
        end();
        delay(10);
    }
}

COMPONENT_INIT
{
    LE_INFO("LED strip control startedd");

    RGBdriver_init();

    //Setup timer to read data
    Reader = le_timer_Create("Get Sample 4");
    LE_ASSERT_OK(le_timer_SetRepeat(Reader, 0));
    LE_ASSERT_OK(le_timer_SetMsInterval(Reader, 5000));
    LE_ASSERT_OK(le_timer_SetHandler(Reader, ReaderHandler));
    le_timer_Start(Reader);
}
