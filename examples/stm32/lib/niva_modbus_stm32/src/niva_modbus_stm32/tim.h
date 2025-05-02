#pragma once

#include "niva_modbus_stm32_config.h"

static inline void modbus_tim_init(TIM_TypeDef* TIMx) {
    LL_TIM_DisableCounter(TIMx);
    LL_TIM_ClearFlag_UPDATE(TIMx);
    LL_TIM_EnableIT_UPDATE(TIMx);
}

static inline void modbus_tim_start(TIM_TypeDef* TIMx) {
    LL_TIM_EnableCounter(TIMx);
}

static inline void modbus_tim_stop(TIM_TypeDef* TIMx) {
    LL_TIM_DisableCounter(TIMx);
}

static inline void modbus_tim_restart(TIM_TypeDef* TIMx) {
    LL_TIM_SetCounter(TIMx, 0);
    LL_TIM_EnableCounter(TIMx);
}
