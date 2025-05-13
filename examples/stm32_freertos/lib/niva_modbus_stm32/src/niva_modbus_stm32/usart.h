#pragma once

#include "niva_modbus_stm32_config.h"
#include "./dma.h"

void modbus_usart_start_receive(uint8_t* data, size_t len);

static inline void modbus_usart_stop_receive() {
    modbus_dma_disable_blocking(MODBUS_USART_DMA_RX, MODBUS_USART_DMA_RX_CH_NUM);
    LL_USART_DisableIT_RXNE(MODBUS_USART);
    LL_USART_DisableDirectionRx(MODBUS_USART);
    LL_USART_DisableDMAReq_RX(MODBUS_USART);
}

static inline void modbus_usart_stop_transmit() {
    modbus_dma_disable_blocking(MODBUS_USART_DMA_TX, MODBUS_USART_DMA_TX_CH_NUM);
    LL_USART_DisableDirectionTx(MODBUS_USART);
    LL_USART_DisableIT_TC(MODBUS_USART);
    LL_USART_DisableDMAReq_TX(MODBUS_USART);
}

uint32_t modbus_usart_get_received_len();

void modbus_usart_transmit(uint8_t* data, size_t len);
