#include "port.h"

#include "mb.h"
#include "mbport.h"

extern UART_HandleTypeDef* MB_UART;
extern GPIO_TypeDef* MB_UART_DE_PORT;
extern uint16_t MB_UART_DE_PIN;

uint8_t txByte = 0x00;
uint8_t rxByte = 0x00;

static void prvvUARTTxReadyISR( void );
static void prvvUARTRxISR( void );

void vMBPortSerialEnable(BOOL xRxEnable, BOOL xTxEnable)
{
  if (xRxEnable == FALSE)
  {
    HAL_UART_AbortReceive_IT(MB_UART);
  }
  else
  {
    HAL_GPIO_WritePin(MB_UART_DE_PORT, MB_UART_DE_PIN, GPIO_PIN_RESET);
    ATOMIC_SET_BIT(MB_UART->Instance->CR1, (USART_CR1_RXNEIE));
    HAL_UART_Receive_IT(MB_UART, &rxByte, 1);
  }

  if (xTxEnable == FALSE)
  {
    HAL_UART_AbortTransmit_IT(MB_UART);
  }
  else
  {
    HAL_GPIO_WritePin(MB_UART_DE_PORT, MB_UART_DE_PIN, GPIO_PIN_SET);
    ATOMIC_SET_BIT(MB_UART->Instance->CR1, (USART_CR1_TXEIE | USART_CR1_TCIE));
    if (MB_UART->gState == HAL_UART_STATE_READY)
    {
      prvvUARTTxReadyISR();
    }
  }
}

BOOL xMBPortSerialInit(UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity)
{
    (void)ucPORT;
    (void)ulBaudRate;
    (void)ucDataBits;
    (void)eParity;
    return TRUE;
}

BOOL xMBPortSerialPutByte(CHAR ucByte)
{
  txByte = ucByte;
  HAL_UART_Transmit_IT(MB_UART, &txByte, 1);
  return TRUE;
}

BOOL xMBPortSerialGetByte(CHAR * pucByte)
{
  *pucByte = rxByte;
  HAL_UART_Receive_IT(MB_UART, &rxByte, 1);
  return TRUE;
}

static void prvvUARTTxReadyISR(void)
{
  pxMBFrameCBTransmitterEmpty();
}

static void prvvUARTRxISR(void)
{
  pxMBFrameCBByteReceived();
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance == MB_UART->Instance)
  {
    prvvUARTTxReadyISR();
  }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance == MB_UART->Instance)
  {
    prvvUARTRxISR();
  }
}
