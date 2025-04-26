#include "./usart.h"

#if defined(MODBUS_DE_PORT) && defined(MODBUS_DE_PIN)
#include "./gpio.h"
#endif


volatile uint32_t DMA_RECEIVE_LEN;

inline void modbus_usart_start_receive(uint8_t* data, size_t len) {
    #if defined(MODBUS_DE_PORT) && defined(MODBUS_DE_PIN)
    modbus_gpio_off(MODBUS_DE_PORT, MODBUS_DE_PIN);
    #endif

    modbus_dma_transmit(DMA1_ChannelX(MODBUS_USART_DMA_RX_CH_NUM), (uint32_t)&MODBUS_USART->RDR, (uint32_t)data, (uint32_t)len);
    DMA_RECEIVE_LEN = len;

    MODBUS_USART->CR3 |= USART_CR3_DMAR;
    MODBUS_USART->CR1 |= USART_CR1_RE | USART_CR1_RXNEIE;
}

inline uint32_t modbus_usart_get_received_len() {
    return DMA_RECEIVE_LEN - DMA1_ChannelX(MODBUS_USART_DMA_RX_CH_NUM)->CNDTR;
}

inline void modbus_usart_transmit(uint8_t* data, size_t len) {
    #if defined(MODBUS_DE_PORT) && defined(MODBUS_DE_PIN)
    modbus_gpio_on(MODBUS_DE_PORT, MODBUS_DE_PIN);
    #endif

    modbus_dma_transmit(DMA1_ChannelX(MODBUS_USART_DMA_TX_CH_NUM), (uint32_t)&MODBUS_USART->TDR, (uint32_t)data, (uint32_t)len);

    MODBUS_USART->CR3 |= USART_CR3_DMAT;
    MODBUS_USART->CR1 |= USART_CR1_TE | USART_CR1_TCIE;
}