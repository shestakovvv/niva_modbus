#include "port.h"

#include "mb.h"
#include "mbport.h"

extern TIM_HandleTypeDef* MB_TIMER;

uint16_t timerPeriod = 0;
uint16_t timerCounter = 0;

static void prvvTIMERExpiredISR(void);

BOOL xMBPortTimersInit(USHORT usTim1Timerout50us)
{
  timerPeriod = usTim1Timerout50us;
  return TRUE;
}

inline void vMBPortTimersEnable()
{
  timerCounter = 0;
  HAL_TIM_Base_Start_IT(MB_TIMER);
}

inline void vMBPortTimersDisable()
{
  HAL_TIM_Base_Stop_IT(MB_TIMER);
}

static void prvvTIMERExpiredISR(void)
{
    (void)pxMBPortCBTimerExpired();
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == MB_TIMER->Instance)
  {
    timerCounter++;
    if (timerCounter == timerPeriod)
    {
      prvvTIMERExpiredISR();
    }
  }
}