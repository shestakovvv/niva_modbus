#pragma once

#include "niva_modbus_stm32_config.h"
#include "./dma.h"

void modbus_usart_start_receive(uint8_t* data, size_t len);

static inline void modbus_usart_stop_receive() {
    LL_DMA_DisableChannel(MODBUS_USART_DMA_RX, MODBUS_USART_DMA_RX_CH_NUM);
    LL_USART_DisableIT_RXNE(MODBUS_USART);
    LL_USART_DisableDMAReq_RX(MODBUS_USART);
}

uint32_t modbus_usart_get_received_len();

static inline void modbus_usart_transfer_coplete_irq_enable(void) {
    LL_USART_EnableIT_TC(MODBUS_USART);
}
static inline void modbus_usart_transfer_coplete_irq_disable(void) {
    LL_USART_DisableIT_TC(MODBUS_USART);
}

void modbus_usart_transmit(uint8_t* data, size_t len);
