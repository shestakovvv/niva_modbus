#pragma once

#include "main.h"

#ifndef MODBUS_TIM
#define MODBUS_TIM TIM3
#endif

#ifndef MODBUS_USART
#define MODBUS_USART USART2
#endif
#ifndef MODBUS_USART_DMA_RX
#define MODBUS_USART_DMA_RX DMA1
#endif
#ifndef MODBUS_USART_DMA_RX_CH_NUM
#define MODBUS_USART_DMA_RX_CH_NUM 6
#endif
#ifndef MODBUS_USART_DMA_TX
#define MODBUS_USART_DMA_TX DMA1
#endif
#ifndef MODBUS_USART_DMA_TX_CH_NUM
#define MODBUS_USART_DMA_TX_CH_NUM 7
#endif

#ifndef MODBUS_RX_BUFFER_LEN
#define MODBUS_RX_BUFFER_LEN 2048
#endif
#ifndef MODBUS_RX_PACKETS_COUNT
#define MODBUS_RX_PACKETS_COUNT 1
#endif

// Перезаписывать буффер, если modbus_update еще не успел его обработать
#define MODBUS_REWRITE_NOT_PROCESSED_PACKETS false

// comment if don't wan't to use
#define MODBUS_LED_PORT LD3_GPIO_Port
#define MODBUS_LED_PIN LD3_Pin

// comment if don't wan't to use
#define MODBUS_DE_PORT USART2_DE_GPIO_Port
#define MODBUS_DE_PIN USART2_DE_Pin

#define MOSBUS_SERVER_STATS true

#define MODBUS_TX_TIMEOUT 250
#define MODBUS_RX_TIMEOUT 1000 // TODO: implement modbus rx timeout

#if (defined(MODBUS_LED_PORT) && defined(MODBUS_LED_PIN)) || (defined(MODBUS_DE_PORT) && defined(MODBUS_DE_PIN))
#include "./gpio.h"
#endif

