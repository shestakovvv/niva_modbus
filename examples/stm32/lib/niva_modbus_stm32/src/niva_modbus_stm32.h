#pragma once

#include "niva_modbus.h"



/// @brief необходимо вызвать в основном цикле программы (main.c -> while(1) или в таске freertos)
/// чем меньше задержка вызовов - тем быстрее обрабатываются пакеты и отправляются ответы 
/// @param server 
int8_t modbus_update(ModbusServer* server);

/// @brief необходимо вызвать в прерывании по таймеру указанному в `niva_modbus_stm32_config.h`
void modbus_on_tim_irq(void);
/// @brief необходимо вызвать в прерывании по uart'у указанному в `niva_modbus_stm32_config.h`
void modbus_on_usart_irq(void);
/// @brief необходимо вызвать в прерывании по dma uart'a приема указанному в `niva_modbus_stm32_config.h`
void modbus_on_dma_rx_irq(void);
/// @brief необходимо вызвать в прерывании по dma uart'a отправки указанному в `niva_modbus_stm32_config.h`
void modbus_on_dma_tx_irq(void);