#include "niva_modbus_stm32.h"

#include "niva_modbus_stm32/config.h"
#include "niva_modbus_stm32/tim.h"
#include "niva_modbus_stm32/usart.h"

#define MODBUS_PACKET_MAX_LEN 256

typedef struct RxPacket {
    bool is_new;
    uint8_t *data;
    size_t len;
} RxPacket;

static inline void rx_packet_next(size_t* num) {
    (*num)++;
    if (*num >= MODBUS_RX_PACKETS_COUNT) {
        *num = 0;
    }
}

RxPacket RX_PACKETS_BUFFER[MODBUS_RX_PACKETS_COUNT];

uint8_t RX_BUFFER[MODBUS_RX_BUFFER_LEN];
uint8_t TX_BUFFER[MODBUS_PACKET_MAX_LEN];

#if MOSBUS_RESPONSE_TIME_TEST == true
uint32_t response_start_time = 0;
uint32_t response_time = 0;
#endif

void modbus_init(void) {
    modbus_usart_start_receive(RX_BUFFER, MODBUS_PACKET_MAX_LEN);
    modbus_tim_init();
}

int8_t modbus_update(ModbusServer* server) {
    static size_t CURRENT_PACKET_NUM = 0;

    RxPacket* packet = &RX_PACKETS_BUFFER[CURRENT_PACKET_NUM];

    if (!packet->is_new) {
        return MODBUS_OK;
    }

    #if MOSBUS_RESPONSE_TIME_TEST == true
    if (server->address == packet->data[0]) {
        response_start_time = HAL_GetTick();
    }
    #endif

    size_t tx_buffer_len = 0;
    int8_t result = modbus_server_poll(
        server, 
        packet->data, 
        packet->len, 
        TX_BUFFER, 
        &tx_buffer_len);
        
    if (server->response_required) {
        modbus_usart_transmit(TX_BUFFER, tx_buffer_len);
        #if defined(MODBUS_LED_PORT) && defined(MODBUS_LED_PIN)
        modbus_gpio_toggle(MODBUS_LED_PORT, MODBUS_LED_PIN);
        #endif
    }

    packet->is_new = false;
    rx_packet_next(&CURRENT_PACKET_NUM);
    return result;
}

static void modbus_on_new_packet_received(void) {
    static uint8_t* CURRENT_BUFFER_POS = RX_BUFFER;
    static size_t CURRENT_PACKET_NUM = 0;

    modbus_usart_stop_receive();

    RxPacket new_packet = {
        .data = CURRENT_BUFFER_POS,
        .len = modbus_usart_get_received_len(),
        .is_new = true
    };

    CURRENT_BUFFER_POS += new_packet.len;
    if (CURRENT_BUFFER_POS + MODBUS_PACKET_MAX_LEN > RX_BUFFER + sizeof(RX_BUFFER)) {
        CURRENT_BUFFER_POS = RX_BUFFER;
    }
    modbus_usart_start_receive(CURRENT_BUFFER_POS, MODBUS_PACKET_MAX_LEN);

    #if MODBUS_REWRITE_NOT_PROCESSED_PACKETS == false
    if (RX_PACKETS_BUFFER[CURRENT_PACKET_NUM].is_new) {
        return;
    }
    #endif

    RX_PACKETS_BUFFER[CURRENT_PACKET_NUM] = new_packet;
    rx_packet_next(&CURRENT_PACKET_NUM);
}

inline void modbus_on_tim_irq(void) {
    if (MODBUS_TIM->SR & TIM_SR_UIF) {
        MODBUS_TIM->SR &= ~TIM_SR_UIF;
        modbus_tim_stop();
        modbus_on_new_packet_received();
    }
}

inline void modbus_on_usart_irq(void) {
    if (MODBUS_USART->ISR & USART_ISR_TC) {
        MODBUS_USART->ICR |= USART_ICR_TCCF;
    }
    modbus_tim_restart();
}

inline void modbus_on_dma_rx_irq(void) {
    if (MODBUS_USART_DMA_RX->ISR & MODBUS_CONCAT2(DMA_ISR_TCIF, MODBUS_USART_DMA_RX_CH_NUM)) {
        MODBUS_USART_DMA_RX->IFCR |= MODBUS_CONCAT2(DMA_IFCR_CTCIF, MODBUS_USART_DMA_RX_CH_NUM);
        // FIXME: здесь есть вероятность, что у отправителя не выдержана задержка между пакетами
        modbus_tim_stop();
        modbus_on_new_packet_received();
    }
}

inline void modbus_on_dma_tx_irq(void) {
    if (MODBUS_USART_DMA_TX->ISR & MODBUS_CONCAT2(DMA_ISR_TCIF, MODBUS_USART_DMA_TX_CH_NUM)) {
        MODBUS_USART_DMA_TX->IFCR |= MODBUS_CONCAT2(DMA_IFCR_CTCIF, MODBUS_USART_DMA_TX_CH_NUM);
        #if defined(MODBUS_DE_PORT) && defined(MODBUS_DE_PIN)
        modbus_gpio_off(MODBUS_DE_PORT, MODBUS_DE_PIN);
        #endif

        #if MOSBUS_RESPONSE_TIME_TEST == true
        response_time = HAL_GetTick() - response_start_time;
        #endif
    }
}

// FIXME: стоило бы добавить проверку на случай, если записи в буффере перекроются и один пакет перезапишет другой