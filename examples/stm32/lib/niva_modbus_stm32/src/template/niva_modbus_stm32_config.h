#pragma once

#include "main.h"

#ifndef MODBUS_TIM
/// таймер используемый для отслеживания конца пакета.
/// Настройка должна быть вызвана в CubeMx перед использованием.
/// По стандарту должен отсчитывать интервал в 3.5 символа юарта.
/// Пример для 115200: 3.5 * 10 / 115200 = 304 мкс ( 10 = bit number with No Parity, 11 = Parity)
#define MODBUS_TIM TIM3
#endif

#ifndef MODBUS_USART
/// юарт используемый для приема/передачи данных.
/// Должен быть настрое CubeMx и вызван до использования
#define MODBUS_USART USART2
#endif
#ifndef MODBUS_USART_DMA_RX_NUM
/// DMA используемый для приема данных по юарту.
#define MODBUS_USART_DMA_RX_NUM 1
#endif
#ifndef MODBUS_USART_DMA_RX_CH_NUM
/// номер канала dma uart на прием.
#define MODBUS_USART_DMA_RX_CH_NUM 6
#endif
#ifndef MODBUS_USART_DMA_TX_NUM
/// DMA используемый для передачи данных по юарту.
#define MODBUS_USART_DMA_TX_NUM 1
#endif
#ifndef MODBUS_USART_DMA_TX_CH_NUM
/// номер канала dma uart на передачу.
#define MODBUS_USART_DMA_TX_CH_NUM 7
#endif

#ifndef MODBUS_RX_BUFFER_LEN
/// Длинна непосредственного юарт буффера.
/// Весь прием осуществляется сюда пакет за пакетом.
/// Указатель возвращается в 0, когда `старт. адрес + длина пакета > конца буффера`
#define MODBUS_RX_BUFFER_LEN 2048
#endif

// Закомменть эти строчки, если нет возможности или не хочешь использовать
/// GPIO порт лампочки, которая будет вкл/выкл при приеме пакетов
#define MODBUS_LED_PORT LD3_GPIO_Port
/// GPIO пин лампочки, которая будет вкл/выкл при приеме пакетов
#define MODBUS_LED_PIN LD3_Pin

// Закомменть эти строчки, если нет возможности или не хочешь использовать
/// GPIO порт ножки DE для отправки данных по RS485 (low - прием, high - передача)
#define MODBUS_DE_PORT USART2_DE_GPIO_Port
/// GPIO пин ножки DE для отправки данных по RS485 (low - прием, high - передача)
#define MODBUS_DE_PIN USART2_DE_Pin

/// Включает структуру для просмотра количества ошибок, пропущенных сообщений, времени ответа
#define MOSBUS_SERVER_STATS false

/// Таймаут отправки сообщений по юарт. Если за это время ответ не отправился - сервер перейдет в состояния приема
/// время должно быть как можно ближе к времени отправки 256 байт.
#ifndef MODBUS_TX_TIMEOUT
#define MODBUS_TX_TIMEOUT 50
#endif

/// Таймаут приема сообщающий ошибку пользователю (не реализовано)
#define MODBUS_RX_TIMEOUT 1000 // TODO: implement modbus rx timeout
