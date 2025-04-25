#include "./usart.h"

volatile uint32_t DMA_RECEIVE_LEN;

inline void modbus_usart_start_receive(uint8_t* data, size_t len) {
    #if defined(MODBUS_DE_PORT) && defined(MODBUS_DE_PIN)
    modbus_gpio_off(MODBUS_DE_PORT, MODBUS_DE_PIN);
    #endif

    MODBUS_USART_DMA_RX_CH->CCR &= ~DMA_CCR_EN; 
    MODBUS_USART_DMA_RX_CH->CPAR = (uint32_t)&MODBUS_USART->RDR;
    MODBUS_USART_DMA_RX_CH->CMAR = (uint32_t)data;
    MODBUS_USART_DMA_RX_CH->CNDTR = len;
    DMA_RECEIVE_LEN = len;
    MODBUS_USART_DMA_RX_CH->CCR |= DMA_CCR_TCIE;
    MODBUS_USART_DMA_RX_CH->CCR |= DMA_CCR_EN;

    MODBUS_USART->CR3 |= USART_CR3_DMAR;
    MODBUS_USART->CR1 |= USART_CR1_RE | USART_CR1_RXNEIE;
}

inline uint32_t modbus_usart_get_received_len() {
    return DMA_RECEIVE_LEN - MODBUS_USART_DMA_RX_CH->CNDTR;
}

inline void modbus_usart_transmit(uint8_t* data, size_t len) {
    #if defined(MODBUS_DE_PORT) && defined(MODBUS_DE_PIN)
    modbus_gpio_on(MODBUS_DE_PORT, MODBUS_DE_PIN);
    #endif

    MODBUS_USART_DMA_TX_CH->CCR &= ~DMA_CCR_EN;
    MODBUS_USART_DMA_TX_CH->CPAR = (uint32_t)&MODBUS_USART->TDR;
    MODBUS_USART_DMA_TX_CH->CMAR = (uint32_t)data;
    MODBUS_USART_DMA_TX_CH->CNDTR = len;
    MODBUS_USART_DMA_TX_CH->CCR |= DMA_CCR_TCIE;
    MODBUS_USART_DMA_TX_CH->CCR |= DMA_CCR_EN;

    MODBUS_USART->CR3 |= USART_CR3_DMAT;
    MODBUS_USART->CR1 |= USART_CR1_TE;
}