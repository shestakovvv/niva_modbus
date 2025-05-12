#include "niva_modbus_stm32.h"

#include "niva_modbus_stm32_config.h"
#include "niva_modbus_stm32/tim.h"
#include "niva_modbus_stm32/usart.h"
#if (defined(MODBUS_LED_PORT) && defined(MODBUS_LED_PIN)) || (defined(MODBUS_DE_PORT) && defined(MODBUS_DE_PIN))
#include "niva_modbus_stm32/gpio.h"
#endif

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#define DISCRETE_INPUTS_COUNT   100
#define COILS_COUNT             100
#define INPUT_REGISTERS_COUNT   100
#define HOLDING_REGISTERS_COUNT 2000
DiscreteInput DISCRETE_INPUTS[DISCRETE_INPUTS_COUNT] = {0};
Coil COILS[COILS_COUNT] = {0};
InputRegister INPUT_REGISTERS[INPUT_REGISTERS_COUNT] = {0};
HoldingRegister HOLDING_REGISTERS[HOLDING_REGISTERS_COUNT] = {0};

TaskHandle_t MODBUS_TASK_HANDLE = NULL;
ModbusServer MODBUS_SERVER;
SemaphoreHandle_t MODBUS_SERVER_MUTEX;

#define MODBUS_EVENT_NEW_PACKET_RECEIVED    (1UL << 0)
#define MODBUS_EVENT_PACKET_TRANSMITTED     (1UL << 1)

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

uint8_t RX_BUFFER[MODBUS_RX_BUFFER_LEN];
uint8_t* CURRENT_RX_BUFFER_POS = RX_BUFFER;
uint8_t TX_BUFFER[MODBUS_PACKET_MAX_LEN];

void modbus_server_init(void) {
    MODBUS_SERVER = (ModbusServer) {
        .address = 1,
        .discrete_inputs = DISCRETE_INPUTS,
        .discrete_inputs_count = DISCRETE_INPUTS_COUNT,
        .coils = COILS,
        .coils_count = COILS_COUNT,
        .input_registers = INPUT_REGISTERS,
        .input_registers_count = INPUT_REGISTERS_COUNT,
        .holding_registers = HOLDING_REGISTERS,
        .holding_registers_count = HOLDING_REGISTERS_COUNT,
    };
    MODBUS_SERVER_MUTEX = xSemaphoreCreateMutex();
}

static inline void modbus_start_receive(void) {
    modbus_usart_start_receive(CURRENT_RX_BUFFER_POS, MODBUS_PACKET_MAX_LEN);
    modbus_tim_init(MODBUS_TIM);
}

void modbus_stop(void) {
    uint32_t transfer_dir = LL_USART_GetTransferDirection(MODBUS_USART);
    if (LL_TIM_IsEnabledCounter(MODBUS_TIM)) {
        modbus_tim_stop(MODBUS_TIM);
    }
    if (transfer_dir == LL_USART_DIRECTION_TX_RX) {
        modbus_usart_stop_receive();
        modbus_usart_stop_transmit();
    } else if (LL_USART_DIRECTION_TX) {
        modbus_usart_stop_transmit();
    } else if (LL_USART_DIRECTION_RX) {
        modbus_usart_stop_receive();
    }
}

static inline void modbus_stop_receive(void) {
    modbus_tim_stop(MODBUS_TIM);
    modbus_usart_stop_receive();
}

static inline size_t modbus_update_buffer_pos(void) {
    size_t received_len = modbus_usart_get_received_len();

    CURRENT_RX_BUFFER_POS += received_len;
    if (CURRENT_RX_BUFFER_POS + MODBUS_PACKET_MAX_LEN > RX_BUFFER + sizeof(RX_BUFFER)) {
        CURRENT_RX_BUFFER_POS = RX_BUFFER;
    }
    return received_len;
}

void modbus_restart(void) {
    modbus_stop();
    modbus_start_receive();
}

void modbus_task(void *args)
{
    uint32_t irq_notification = 0;

    modbus_restart();
    for (;;)
    {
        BaseType_t result = xTaskNotifyWait(
            0x00,
            0xFFFFFFFF,
            &irq_notification,
            pdMS_TO_TICKS(MODBUS_ERROR_TIMEOUT_VALUE)
        );

        if (result != pdPASS) {
            modbus_restart();
        } else {
            if (irq_notification & MODBUS_EVENT_PACKET_TRANSMITTED)
            {
                modbus_usart_start_receive(CURRENT_RX_BUFFER_POS, MODBUS_PACKET_MAX_LEN);
    
                #if MOSBUS_SERVER_STATS == true
                SERVER_STATS.response_time = HAL_GetTick() - SERVER_STATS._response_start_time;
                #endif
            }
            if (irq_notification & MODBUS_EVENT_NEW_PACKET_RECEIVED)
            {
                bool is_response_required = false;
                do {
                    modbus_stop_receive();
    
                    uint8_t* new_packet = CURRENT_RX_BUFFER_POS;
                    size_t new_packet_len = modbus_update_buffer_pos();
    
                    if (new_packet_len == 0) {
                        break;
                    }
        
                    size_t tx_buffer_len = 0;
                    WITH_MODBUS_SERVER_LOCK(server, MODBUS_ERROR_TIMEOUT_VALUE) {
                        #if MOSBUS_SERVER_STATS == true
                        if (server->address == new_packet[0]) {
                            SERVER_STATS._response_start_time = HAL_GetTick();
                        }
                        #endif
            
                        // FIXME: add result usage
                        // int8_t result = modbus_server_poll(
                        modbus_server_poll(
                            server, 
                            new_packet, 
                            new_packet_len, 
                            TX_BUFFER, 
                            &tx_buffer_len);
            
                        is_response_required = server->response_required;
                    }
                    
                    if (!is_response_required) {
                        break;
                    }
        
                    modbus_usart_transmit(TX_BUFFER, tx_buffer_len);

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
                } while(false);
                if (!is_response_required) {
                    modbus_usart_start_receive(CURRENT_RX_BUFFER_POS, MODBUS_PACKET_MAX_LEN);
                }
            }
        }
    }
}

inline void modbus_on_tim_irq(void) {
    if (LL_TIM_IsActiveFlag_UPDATE(MODBUS_TIM)) {
        LL_TIM_ClearFlag_UPDATE(MODBUS_TIM);

        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xTaskNotifyFromISR(
            MODBUS_TASK_HANDLE,
            MODBUS_EVENT_NEW_PACKET_RECEIVED,
            eSetBits,
            &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

inline void modbus_on_usart_irq(void) {
    if (LL_USART_IsActiveFlag_TC(MODBUS_USART)) {
        LL_USART_ClearFlag_TC(MODBUS_USART);

        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xTaskNotifyFromISR(
            MODBUS_TASK_HANDLE,
            MODBUS_EVENT_PACKET_TRANSMITTED,
            eSetBits,
            &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    } else {
        modbus_tim_restart(MODBUS_TIM);
    }
}

inline void modbus_on_dma_rx_irq(void) {
    if (LL_DMA_IsActiveFlag_TCX(MODBUS_USART_DMA_RX, MODBUS_USART_DMA_RX_CH_NUM)) {
        LL_DMA_ClearFlag_TCX(MODBUS_USART_DMA_RX, MODBUS_USART_DMA_RX_CH_NUM);

        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xTaskNotifyFromISR(
            MODBUS_TASK_HANDLE,
            MODBUS_EVENT_NEW_PACKET_RECEIVED,
            eSetBits,
            &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

inline void modbus_on_dma_tx_irq(void) {
    if (LL_DMA_IsActiveFlag_TCX(MODBUS_USART_DMA_TX, MODBUS_USART_DMA_TX_CH_NUM)) {
        LL_DMA_ClearFlag_TCX(MODBUS_USART_DMA_TX, MODBUS_USART_DMA_TX_CH_NUM);
    }
}

// FIXME: стоило бы добавить проверку на случай, если записи в буффере перекроются и один пакет перезапишет другой
