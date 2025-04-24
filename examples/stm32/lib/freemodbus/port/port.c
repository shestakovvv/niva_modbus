#include "port.h"
#include <stdint.h>

static uint32_t MB_LOCK_CNT = 0;

UART_HandleTypeDef* MB_UART;
TIM_HandleTypeDef* MB_TIMER;
GPIO_TypeDef* MB_UART_DE_PORT;
uint16_t MB_UART_DE_PIN;

void EnterCriticalSection(void)
{
  __disable_irq();
  MB_LOCK_CNT++;
}

void ExitCriticalSection(void)
{
  MB_LOCK_CNT--;
  if (MB_LOCK_CNT == 0)
  {
    __enable_irq();
  }
}