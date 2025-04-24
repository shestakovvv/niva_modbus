#pragma once

#include <assert.h>
#include <inttypes.h>
#include "stm32f3xx_hal.h"

#define	INLINE                      inline
#define PR_BEGIN_EXTERN_C           extern "C" {
#define	PR_END_EXTERN_C             }

void EnterCriticalSection(void);
void ExitCriticalSection(void);

// управление прерываниями
#define ENTER_CRITICAL_SECTION( )	EnterCriticalSection()
#define EXIT_CRITICAL_SECTION( )	ExitCriticalSection()

// подменим вызовы по всей библиотеки
#define assert(val) assert_param(val)

typedef uint8_t BOOL;

typedef unsigned char UCHAR;
typedef char CHAR;

typedef uint16_t USHORT;
typedef int16_t SHORT;

typedef uint32_t ULONG;
typedef int32_t LONG;

#ifndef TRUE
#define TRUE            1
#endif

#ifndef FALSE
#define FALSE           0
#endif

extern UART_HandleTypeDef* MB_UART;
extern TIM_HandleTypeDef* MB_TIMER;
extern GPIO_TypeDef* MB_UART_DE_PORT;
extern uint16_t MB_UART_DE_PIN;

static inline void MT_PORT_SetTimerModule(TIM_HandleTypeDef* timer)
{
  MB_TIMER= timer;
}
/*----------------------------------------------------------------------------*/\
static inline void MT_PORT_SetUartModule(UART_HandleTypeDef* uart, GPIO_TypeDef* de_port, uint16_t de_pin)
{
  MB_UART = uart;
  MB_UART_DE_PORT = de_port;
  MB_UART_DE_PIN = de_pin;
}

// callback для uart
BOOL freemodbus_uart_callback(void);