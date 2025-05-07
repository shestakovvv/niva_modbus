#pragma once

#include "niva_modbus_stm32_config.h"

#define CONCAT4(a, b, c, d) a ## b ## c ## d
#define CONCAT3(a, b, c) a ## b ## c
#define CONCAT(a, b) a ## b

#define DMAx(dma_num) CONCAT(DMA, dma_num)
#define DMAx_Channely(dma_num, ch_num) CONCAT4(DMA, dma_num, _Channel, ch_num)

#define LL_DMA_IsActiveFlag_TCX(dma, ch_num) CONCAT(LL_DMA_IsActiveFlag_TC, ch_num)(dma)
#define LL_DMA_ClearFlag_TCX(dma, ch_num) CONCAT(LL_DMA_ClearFlag_TC, ch_num)(dma)

#define MODBUS_USART_DMA_RX DMAx(MODBUS_USART_DMA_RX_NUM)
#define MODBUS_USART_DMA_RX_CH DMAx_Channely(MODBUS_USART_DMA_RX_NUM, MODBUS_USART_DMA_RX_CH_NUM)
#define MODBUS_USART_DMA_TX DMAx(MODBUS_USART_DMA_TX_NUM)
#define MODBUS_USART_DMA_TX_CH DMAx_Channely(MODBUS_USART_DMA_TX_NUM, MODBUS_USART_DMA_TX_CH_NUM)


static inline void modbus_dma_transmit(DMA_TypeDef* dma, uint32_t channel, uint32_t peripheral_addr, uint32_t data_addr, uint32_t len) {
    LL_DMA_DisableChannel(dma, channel);  // Disable channel before configuring

    LL_DMA_SetPeriphAddress(dma, channel, peripheral_addr);  // Set peripheral address
    LL_DMA_SetMemoryAddress(dma, channel, data_addr);        // Set memory address
    LL_DMA_SetDataLength(dma, channel, len);                 // Set transfer length

    LL_DMA_EnableIT_TC(dma, channel);  // Enable transfer complete interrupt
    LL_DMA_EnableChannel(dma, channel); // Enable DMA channel
}
