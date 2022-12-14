#include "bsp.h"

static volatile uint32_t systick_counter = 0;

void SysTick_Handler(void)
{
  systick_counter++;
}

void bspInit(void)
{
  BOARD_InitBootPins();
  BOARD_InitBootClocks();
  BOARD_InitBootPeripherals();

  SysTick_Config(SystemCoreClock / 1000U);
}

void bspDeInit(void)
{

}

void delay(uint32_t ms)
{
  uint32_t pre_time = systick_counter;

  while(systick_counter - pre_time < ms);
}

uint32_t millis(void)
{
  return systick_counter;
}
