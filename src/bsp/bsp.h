#ifdef __cplusplus
extern "C"{
#endif

#include "def.h"
#include "config/board/clock_config.h"
#include "config/board/pin_mux.h"
#include "config/board/peripherals.h"
#include "fsl_gpio.h"
#include "fsl_lpuart.h"

#define logPrintf(...) printf(__VA_ARGS__)

void bspInit(void);
void bspDeInit(void);

extern void delay(uint32_t delay_ms);
extern uint32_t millis(void);
extern uint32_t mecros(void);

#ifdef __cplusplus
}
#endif
