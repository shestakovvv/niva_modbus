#pragma once

#include "niva_modbus.h"

#include "FreeRTOS.h"
#include "task.h"

#include "niva_modbus_stm32/lock.h"

void modbus_server_init(void);

extern TaskHandle_t MODBUS_TASK_HANDLE;
void modbus_task(void *server);

/// @brief необходимо вызвать в прерывании по таймеру указанному в `niva_modbus_stm32_config.h`
void modbus_on_tim_irq(void);
/// @brief необходимо вызвать в прерывании по uart'у указанному в `niva_modbus_stm32_config.h`
void modbus_on_usart_irq(void);
/// @brief необходимо вызвать в прерывании по dma uart'a приема указанному в `niva_modbus_stm32_config.h`
void modbus_on_dma_rx_irq(void);
/// @brief необходимо вызвать в прерывании по dma uart'a отправки указанному в `niva_modbus_stm32_config.h`
void modbus_on_dma_tx_irq(void);