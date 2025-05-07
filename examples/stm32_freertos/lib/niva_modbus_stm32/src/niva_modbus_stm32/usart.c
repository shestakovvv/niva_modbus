#include "./usart.h"

#if defined(MODBUS_DE_PORT) && defined(MODBUS_DE_PIN)
#include "./gpio.h"
#endif

#if defined(STM32F1)
#define MODBUS_USART_RDR MODBUS_USART->DR
#define MODBUS_USART_TDR MODBUS_USART->DR
#else
#define MODBUS_USART_RDR MODBUS_USART->RDR
#define MODBUS_USART_TDR MODBUS_USART->TDR
#endif

volatile uint32_t DMA_RECEIVE_LEN;

inline void modbus_usart_start_receive(uint8_t* data, size_t len) {
    #if defined(MODBUS_DE_PORT) && defined(MODBUS_DE_PIN)
    modbus_gpio_off(MODBUS_DE_PORT, MODBUS_DE_PIN);
    #endif

    modbus_dma_transmit(MODBUS_USART_DMA_RX, MODBUS_USART_DMA_RX_CH_NUM, (uint32_t)&MODBUS_USART_RDR, (uint32_t)data, (uint32_t)len);
    DMA_RECEIVE_LEN = len;

    LL_USART_EnableDMAReq_RX(MODBUS_USART);
    LL_USART_EnableDirectionRx(MODBUS_USART);
    LL_USART_EnableIT_RXNE(MODBUS_USART);
}

inline uint32_t modbus_usart_get_received_len() {
    return DMA_RECEIVE_LEN - LL_DMA_GetDataLength(MODBUS_USART_DMA_RX, MODBUS_USART_DMA_RX_CH_NUM);
}

inline void modbus_usart_transmit(uint8_t* data, size_t len) {
    #if defined(MODBUS_DE_PORT) && defined(MODBUS_DE_PIN)
    modbus_gpio_on(MODBUS_DE_PORT, MODBUS_DE_PIN);
    #endif

    modbus_dma_transmit(MODBUS_USART_DMA_TX, MODBUS_USART_DMA_TX_CH_NUM, (uint32_t)&MODBUS_USART_TDR, (uint32_t)data, (uint32_t)len);

    LL_USART_EnableDMAReq_TX(MODBUS_USART);
    LL_USART_EnableDirectionTx(MODBUS_USART);
    LL_USART_EnableIT_TC(MODBUS_USART);
}