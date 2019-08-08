#include "legato.h"
#include "interfaces.h"

void RGBdriver_init();
void begin(void);
void end(void);
void ClkRise(void);
void Send32Zero(void);
uint8_t TakeAntiCode(uint8_t dat);
void DatSend(uint32_t dx);
void SetColor(uint8_t Red,uint8_t Green,uint8_t Blue);