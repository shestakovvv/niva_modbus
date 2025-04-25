#pragma once

#include "main.h"

#ifndef MODBUS_TIM
#define MODBUS_TIM TIM3
#endif

#define MODBUS_CONCAT2_HELP(a, b) a ## b
#define MODBUS_CONCAT2(a, b) MODBUS_CONCAT2_HELP(a, b)

#ifndef MODBUS_USART
#define MODBUS_USART USART1
#endif
#ifndef MODBUS_USART_DMA_RX
#define MODBUS_USART_DMA_RX DMA1
#endif
#ifndef MODBUS_USART_DMA_RX_CH
#define MODBUS_USART_DMA_RX_CH_NUM 5
// FIXME: не смог сгенерировать макрос DMA1_Channel5 из MODBUS_USART_DMA_RX и MODBUS_USART_DMA_RX_CH_NUM
#define MODBUS_USART_DMA_RX_CH DMA1_Channel5
#endif
#ifndef MODBUS_USART_DMA_TX
#define MODBUS_USART_DMA_TX DMA1
#endif
#ifndef MODBUS_USART_DMA_TX_CH
#define MODBUS_USART_DMA_TX_CH_NUM 4
// FIXME: не смог сгенерировать макрос DMA1_Channel5 из MODBUS_USART_DMA_TX и MODBUS_USART_DMA_TX_CH_NUM
#define MODBUS_USART_DMA_TX_CH DMA1_Channel4
#endif

#ifndef MODBUS_RX_BUFFER_LEN
#define MODBUS_RX_BUFFER_LEN 2048
#endif
#ifndef MODBUS_RX_PACKETS_COUNT
#define MODBUS_RX_PACKETS_COUNT 5
#endif

// Перезаписывать буффер, если modbus_update еще не успел его обработать
#define MODBUS_REWRITE_NOT_PROCESSED_PACKETS false

// uncomment for use
#define MODBUS_LED_PORT LD3_GPIO_Port
#define MODBUS_LED_PIN LD3_Pin

// uncomment for use
// #define MODBUS_DE_PORT LD3_GPIO_Port
// #define MODBUS_DE_PIN LD3_Pin

#define MOSBUS_RESPONSE_TIME_TEST false

#if (defined(MODBUS_LED_PORT) && defined(MODBUS_LED_PIN)) || (defined(MODBUS_DE_PORT) && defined(MODBUS_DE_PIN))
#include "./gpio.h"
#endif

