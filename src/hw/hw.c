#include "hw.h"

void hwInit(void)
{
  bspInit();

  ledInit();
  uartInit();
  uartOpen(_DEF_UART1, 115200);
}
