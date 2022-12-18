#include "ap.h"

void apInit(void)
{
  hwInit();
}

void apMain(void)
{
  uint32_t pre_time;

  pre_time = millis();

  while(1)
  {
    if(millis()-pre_time >= 500)
    {
      pre_time = millis();

      ledToggle(_DEF_LED1);
    }

   if(uartAvailable(_DEF_UART1) > 0)
   {
     uartPrintf(_DEF_UART1, "rx : 0x%X\r\n", uartRead(_DEF_UART1));
   }
  }
}
