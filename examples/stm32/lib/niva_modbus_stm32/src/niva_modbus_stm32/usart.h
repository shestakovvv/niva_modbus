#pragma once

#include "niva_modbus_stm32_config.h"
#include "./dma.h"

void modbus_usart_start_receive(uint8_t* data, size_t len);

static inline void modbus_usart_stop_receive() {
    DMA1_ChannelX(MODBUS_USART_DMA_RX_CH_NUM)->CCR &= ~DMA_CCR_EN;
    MODBUS_USART->CR1 &= ~USART_CR1_RXNEIE;
    MODBUS_USART->CR3 &= ~USART_CR3_DMAR;
}

uint32_t modbus_usart_get_received_len();

static inline void modbus_usart_transfer_coplete_irq_enable(void) {
    MODBUS_USART->CR1 |= USART_CR1_TCIE;
}
static inline void modbus_usart_transfer_coplete_irq_disable(void) {
    MODBUS_USART->CR1 &= ~USART_CR1_TCIE;
}

void modbus_usart_transmit(uint8_t* data, size_t len);
