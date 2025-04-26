#pragma once

#include "niva_modbus.h"

int8_t modbus_update(ModbusServer* server);

void modbus_on_tim_irq(void);
void modbus_on_usart_irq(void);
void modbus_on_dma_rx_irq(void);
void modbus_on_dma_tx_irq(void);
    
// TODO: добавить документацию как этим пользоваться и как генерировать CubeMX