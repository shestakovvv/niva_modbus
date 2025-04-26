#include "niva_modbus_stm32.h"

#include "niva_modbus_stm32_config.h"
#include "niva_modbus_stm32/tim.h"
#include "niva_modbus_stm32/usart.h"
#include "niva_modbus_stm32/rx_packet.h"
#if (defined(MODBUS_LED_PORT) && defined(MODBUS_LED_PIN)) || (defined(MODBUS_DE_PORT) && defined(MODBUS_DE_PIN))
#include "niva_modbus_stm32/gpio.h"
#endif

typedef enum ModbusState {
    MODBUS_STATE_OFF,
    MODBUS_STATE_RX,
    MODBUS_STATE_TX,
} ModbusState;

ModbusState MODBUS_STATE = MODBUS_STATE_OFF;
size_t MODBUS_STATE_CHANGE_TIME = 0;

#if MOSBUS_SERVER_STATS == true
typedef struct ModbusServerStatistic {
    size_t ok_count;
    size_t exceptions_count;
    size_t errors_count;
    size_t response_time;
    size_t _response_start_time;
} ModbusServerStatistic;
ModbusServerStatistic SERVER_STATS;
#endif

#define MODBUS_PACKET_MAX_LEN 256

RxPacket RX_PACKET;
uint8_t RX_BUFFER[MODBUS_RX_BUFFER_LEN];
uint8_t* CURRENT_RX_BUFFER_POS = RX_BUFFER;
uint8_t TX_BUFFER[MODBUS_PACKET_MAX_LEN];

static inline void modbus_start_receive(uint8_t* data, size_t len) {
    modbus_usart_start_receive(data, len);
    MODBUS_STATE = MODBUS_STATE_RX;
    MODBUS_STATE_CHANGE_TIME = HAL_GetTick();
}

static inline void modbus_start_transmit(uint8_t* data, size_t len) {
    modbus_usart_transmit(data, len);
    MODBUS_STATE = MODBUS_STATE_TX;
    MODBUS_STATE_CHANGE_TIME = HAL_GetTick();
}

static inline void modbus_check_state(void) {
    if (MODBUS_STATE == MODBUS_STATE_OFF) {
        modbus_start_receive(CURRENT_RX_BUFFER_POS, MODBUS_PACKET_MAX_LEN);
        modbus_tim_init();
    } else if (MODBUS_STATE == MODBUS_STATE_TX) {
        if (HAL_GetTick() - MODBUS_STATE_CHANGE_TIME > MODBUS_TX_TIMEOUT) {
            modbus_start_receive(CURRENT_RX_BUFFER_POS, MODBUS_PACKET_MAX_LEN);
        }
    }
}

int8_t modbus_update(ModbusServer* server) {
    RxPacket* packet = &RX_PACKET;

    modbus_check_state();
  
    if (!packet->is_new) {
        return MODBUS_OK;
    } else {
        packet->is_new = false;
    }

    #if MOSBUS_SERVER_STATS == true
    if (server->address == packet->data[0]) {
        SERVER_STATS._response_start_time = HAL_GetTick();
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
        modbus_start_transmit(TX_BUFFER, tx_buffer_len);
        #if defined(MODBUS_LED_PORT) && defined(MODBUS_LED_PIN)
        modbus_gpio_toggle(MODBUS_LED_PORT, MODBUS_LED_PIN);
        #endif
        #if MOSBUS_SERVER_STATS == true
        if (result == MODBUS_OK) {
            SERVER_STATS.ok_count++;
        } else if (result > MODBUS_OK) {
            SERVER_STATS.exceptions_count++;
        } else if (result < MODBUS_OK) {
            SERVER_STATS.errors_count++;
        }
        #endif
    } else {
        modbus_start_receive(CURRENT_RX_BUFFER_POS, MODBUS_PACKET_MAX_LEN);
    }
    return result;
}

static void modbus_on_new_packet_received(void) {
    modbus_usart_stop_receive();

    RxPacket new_packet = {
        .data = CURRENT_RX_BUFFER_POS,
        .len = modbus_usart_get_received_len(),
        .is_new = true
    };

    CURRENT_RX_BUFFER_POS += new_packet.len;
    if (CURRENT_RX_BUFFER_POS + MODBUS_PACKET_MAX_LEN > RX_BUFFER + sizeof(RX_BUFFER)) {
        CURRENT_RX_BUFFER_POS = RX_BUFFER;
    }

    RX_PACKET = new_packet;
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
    
        modbus_start_receive(CURRENT_RX_BUFFER_POS, MODBUS_PACKET_MAX_LEN);

        #if MOSBUS_SERVER_STATS == true
        SERVER_STATS.response_time = HAL_GetTick() - SERVER_STATS._response_start_time;
        #endif
    } else {
        // обнуления таймера конца пакета по приему байта
        modbus_tim_restart();
    }
}

inline void modbus_on_dma_rx_irq(void) {
    if (MODBUS_USART_DMA_RX->ISR & DMA_ISR_TCIF_X(MODBUS_USART_DMA_RX_CH_NUM)) {
        MODBUS_USART_DMA_RX->IFCR |= DMA_IFCR_CTCIF_X(MODBUS_USART_DMA_RX_CH_NUM);
        modbus_tim_stop();
        modbus_on_new_packet_received();
    }
}

inline void modbus_on_dma_tx_irq(void) {
    if (MODBUS_USART_DMA_TX->ISR & DMA_ISR_TCIF_X(MODBUS_USART_DMA_TX_CH_NUM)) {
        MODBUS_USART_DMA_TX->IFCR |= DMA_IFCR_CTCIF_X(MODBUS_USART_DMA_TX_CH_NUM);
    }
}

// FIXME: стоило бы добавить проверку на случай, если записи в буффере перекроются и один пакет перезапишет другой