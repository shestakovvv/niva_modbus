#pragma once

#include "niva_modbus_stm32_config.h"

static inline void modbus_tim_init(void) {
    MODBUS_TIM->CR1 &= ~TIM_CR1_CEN;
    MODBUS_TIM->SR &= ~TIM_SR_UIF;
    MODBUS_TIM->DIER |= TIM_DIER_UIE;
}

static inline void modbus_tim_start(void) {
    MODBUS_TIM->CR1 |= TIM_CR1_CEN;
}

static inline void modbus_tim_stop(void) {
    MODBUS_TIM->CR1 &= ~TIM_CR1_CEN;
}

static inline void modbus_tim_restart(void) {
    MODBUS_TIM->CNT = 0;
    MODBUS_TIM->CR1 |= TIM_CR1_CEN;
}



