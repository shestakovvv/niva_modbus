#pragma once

#include "niva_modbus_stm32_config.h"

#define DMA1_ChannelX(x) ((DMA_Channel_TypeDef *)((uint32_t)DMA1_Channel1_BASE + (x - 1) * (DMA1_Channel2_BASE - DMA1_Channel1_BASE)))
#define DMA2_ChannelX(x) ((DMA_Channel_TypeDef *)((uint32_t)DMA2_Channel1_BASE + (x - 1) * (DMA2_Channel2_BASE - DMA2_Channel1_BASE)))

#define DMA_ISR_TCIF_X(x) (DMA_ISR_TCIF1 << ((x - 1) * 4))
#define DMA_IFCR_CTCIF_X(x) (DMA_IFCR_CTCIF1 << ((x - 1) * 4))

static inline void modbus_dma_transmit(DMA_Channel_TypeDef* dma_channel, uint32_t peripheral_addr, uint32_t data_addr, uint32_t len) {
    dma_channel->CCR &= ~DMA_CCR_EN;
    dma_channel->CPAR = peripheral_addr;
    dma_channel->CMAR = data_addr;
    dma_channel->CNDTR = len;
    dma_channel->CCR |= DMA_CCR_TCIE;
    dma_channel->CCR |= DMA_CCR_EN;
}
