#include "uart.h"
#include "qbuffer.h"
#ifdef  _USE_HW_VCP
#include  "vcp.h"
#endif

#define UART_MODE_POLLING   0
#define UART_MODE_INTERRUPT 1
#define UART_MODE_DMA       2
#define UART_MODE_VCP       3

#define UART_HW_NONE        0
#define UART_HW_VCP         1
#define UART_HW_UART        2

#define UART_RX_BUF_LENGTH  1024

typedef struct 
{
  bool      is_open;
  uint8_t   ch;
  uint32_t  baud;
  uint8_t   tx_mode;
  uint8_t   rx_mode;
  uint8_t   hw_driver;

  uint8_t   rx_buf[UART_RX_BUF_LENGTH];

  qbuffer_t   qbuffer_rx;
  LPUART_Type *handle;
}uart_t;

static  uart_t uart_tbl[UART_MAX_CH];

static void uartStartRx(uint8_t channel);

bool uartInit(void)
{
  uint8_t i;

  for(i=0; i<UART_MAX_CH; i++)
  {
    uart_tbl[i].is_open = false;
    uart_tbl[i].rx_mode = UART_MODE_POLLING;
    uart_tbl[i].tx_mode = UART_MODE_POLLING;
    uart_tbl[i].hw_driver = UART_HW_NONE;
  }

  return true;
}

bool uartOpen(uint8_t channel, uint32_t baud)
{
  bool ret = false;
  uart_t *p_uart;

  if(channel >= UART_MAX_CH)
  {
    return false;
  }

  switch(channel)
  {
    case _DEF_UART1 :
    {
      p_uart = &uart_tbl[channel];

      p_uart->baud      = baud;
      p_uart->hw_driver = UART_HW_UART;
      p_uart->rx_mode   = UART_MODE_INTERRUPT;
      p_uart->tx_mode   = UART_MODE_POLLING;
      
      p_uart->handle    = LPUART1_PERIPHERAL;

      qbufferCreate(&p_uart->qbuffer_rx, p_uart->rx_buf, UART_RX_BUF_LENGTH);

      p_uart->is_open = true;

      LPUART_SetBaudRate(p_uart->handle, baud, LPUART1_CLOCK_SOURCE);

      uartStartRx(channel);
      ret = true;
    }
    break;
  }

  return ret;
}

bool uartClose(uint8_t channel)
{
  bool ret = false;

  if(channel >= UART_MAX_CH)
  {
    return false;
  }

  if(uart_tbl[channel].is_open == true && uart_tbl[channel].hw_driver == UART_HW_UART)
  {

  }

  return ret;
}

void uartStartRx(uint8_t channel)
{
  uart_t *p_uart = &uart_tbl[channel];

  if(p_uart->rx_mode == UART_MODE_INTERRUPT)
  {

  }
  if(p_uart->rx_mode == UART_MODE_DMA)
  {

  }
}

uint32_t uartGetBaud(uint8_t channel)
{
  if(channel >= UART_MAX_CH)
  {
    return 0;
  }

  return uart_tbl[channel].baud;
}

uint32_t uartAvailable(uint8_t channel)
{
  uint32_t ret;
  uart_t *p_uart = &uart_tbl[channel];

  if(channel >= UART_MAX_CH)
  {
    return 0;
  }
#ifdef _USE_HW_VCP
  if(p_uart->rx_mode == UART_MODE_VCP)
  {
    ret = vcpAvailable();
  }
#endif
  if(p_uart->rx_mode == UART_MODE_INTERRUPT)
  {
    ret = qbufferAvailable(&uart_tbl[channel].qbuffer_rx);
  }
  if(p_uart->rx_mode == UART_MODE_DMA)
  {

  }

  return ret;
}

void uartFlush(uint8_t channel)
{
#ifdef _USE_HW_VCP
  if(uart_tbl[channel].rx_mode == UART_MODE_VCP)
  {
    vcpFlush();
  }
#endif
  if(uart_tbl[channel].rx_mode == UART_MODE_INTERRUPT)
  {
    qbufferFlush(&uart_tbl[channel].qbuffer_rx);
  }
  if(uart_tbl[channel].rx_mode == UART_MODE_DMA)
  {

  }
}

void uartPutch(uint8_t channel, uint8_t ch)
{
  uartWrite(channel, &ch, 1);
}

uint8_t uartGetch(uint8_t channel)
{
  uint8_t ret = 0;

  while (1)
  {
    if(uartAvailable(channel) > 0)
    {
      ret = uartRead(channel);
      break;
    }
  }
  
  return ret;
}

int32_t uartWrite(uint8_t channel, uint8_t *p_data, uint32_t length)
{
  int32_t ret = 0;
  uart_t *p_uart = &uart_tbl[channel];

#ifdef _USE_HW_VCP
  if(p_uart->tx_mode == UART_MODE_VCP)
  {
    vcpWrite(p_data, length);
  }
#endif
  if(p_uart->tx_mode == UART_MODE_POLLING)
  {
    LPUART_WriteBlocking(p_uart->handle, (const uint8_t *)p_data, length);
    ret = length;
  }
  if(p_uart->tx_mode == UART_MODE_DMA)
  {

  }

  return ret;
}

uint8_t uartRead(uint8_t channel)
{
  uint8_t ret = 0;
  uart_t *p_uart = &uart_tbl[channel];

#ifdef _USE_HW_VCP
  if(p_uart->rx_modex == UART_MODE_VCP)
  {
    ret = vcpRead();
  }
#endif
  if(p_uart->rx_mode == UART_MODE_INTERRUPT)
  {
    qbufferRead(&p_uart->qbuffer_rx, &ret, 1);
  }
  if(p_uart->rx_mode == UART_MODE_DMA)
  {
    qbufferRead(&p_uart->qbuffer_rx, &ret, 1);
  }

  return ret;
}

int32_t uartPrintf(uint8_t channel, const char *fmt, ...)
{
  int32_t ret = 0;
  va_list arg;
  va_start (arg, fmt);
  int32_t len;
  char print_buffer[256];

  len = vsnprintf(print_buffer, 255, fmt, arg);
  va_end (arg);

  ret = uartWrite(channel, (uint8_t *)print_buffer, len);

  return ret;
}

void uartRxHandler(uint8_t channel)
{
  uart_t *p_uart = &uart_tbl[channel];

  if(p_uart->rx_mode == UART_MODE_INTERRUPT)
  {
    uartStartRx(channel);  
  }
}

void LPUART1_SERIAL_RX_TX_IRQHANDLER(void)
{
  uint8_t data;
  uart_t *p_uart = &uart_tbl[_DEF_UART1];

  if((kLPUART_RxDataRegFullFlag)&LPUART_GetStatusFlags(p_uart->handle))
  {
    data = LPUART_ReadByte(p_uart->handle);
    qbufferWrite(&p_uart->qbuffer_rx, &data, 1);
  }
}