#include "./dma.h"

void modbus_dma_clear_TC_flag(DMA_TypeDef* dma, uint32_t channel) {
    switch(channel) {
        #if defined(STM32F1) || defined(STM32F3)
        case 1: LL_DMA_ClearFlag_TC1(dma); break;
        case 2: LL_DMA_ClearFlag_TC2(dma); break;
        case 3: LL_DMA_ClearFlag_TC3(dma); break;
        case 4: LL_DMA_ClearFlag_TC4(dma); break;
        case 5: LL_DMA_ClearFlag_TC5(dma); break;
        case 6: LL_DMA_ClearFlag_TC6(dma); break;
        case 7: LL_DMA_ClearFlag_TC7(dma); break;
        #else
        case 0: LL_DMA_ClearFlag_TC0(dma); break;
        case 1: LL_DMA_ClearFlag_TC1(dma); break;
        case 2: LL_DMA_ClearFlag_TC2(dma); break;
        case 3: LL_DMA_ClearFlag_TC3(dma); break;
        case 4: LL_DMA_ClearFlag_TC4(dma); break;
        case 5: LL_DMA_ClearFlag_TC5(dma); break;
        case 6: LL_DMA_ClearFlag_TC6(dma); break;
        case 7: LL_DMA_ClearFlag_TC7(dma); break;
        #endif
        default:
            break;
    }
}