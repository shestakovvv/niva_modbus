#pragma once

#include <stdint.h>
#include <stdbool.h>

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

#if defined(STM32F1) || defined(STM32F3)
#define DMA_UNIT_FUN(fun, dma, unit) fun##Channel(dma, unit)
#else
#define DMA_UNIT_FUN(fun, dma, unit) fun##Stream(dma, unit)
#endif

void modbus_dma_clear_TC_flag(DMA_TypeDef* dma, uint32_t channel);

static inline void modbus_dma_disable(DMA_TypeDef* dma, uint32_t stream) {
    LL_DMA_DisableIT_TC(dma, stream);
    DMA_UNIT_FUN(LL_DMA_Disable, dma, stream);
}

static inline bool modbus_dma_is_enabled(DMA_TypeDef* dma, uint32_t stream) {
    return (DMA_UNIT_FUN(LL_DMA_IsEnabled, dma, stream) == 1);
}

static inline void modbus_dma_disable_blocking(DMA_TypeDef* dma, uint32_t stream) {
    modbus_dma_disable(dma, stream);
    while (modbus_dma_is_enabled(dma, stream));
}

static inline void modbus_dma_enable(DMA_TypeDef* dma, uint32_t stream) {
    DMA_UNIT_FUN(LL_DMA_Enable, dma, stream);
}

static inline void modbus_dma_config_transmit(DMA_TypeDef* dma, uint32_t stream, uint32_t peripheral_addr, uint32_t data_addr, uint32_t len) {
    LL_DMA_SetPeriphAddress(dma, stream, peripheral_addr);
    LL_DMA_SetMemoryAddress(dma, stream, data_addr);
    LL_DMA_SetDataLength(dma, stream, len);

    modbus_dma_clear_TC_flag(dma, stream);
    LL_DMA_EnableIT_TC(dma, stream);
}

static inline uint32_t modbus_dma_get_received_length(DMA_TypeDef* dma, uint32_t stream) {
    return LL_DMA_GetDataLength(dma, stream);
}
