cmake_minimum_required(VERSION 3.22)

project(stm32cubemx)
add_library(stm32cubemx INTERFACE)

# Enable CMake support for ASM and C languages
enable_language(C ASM)

target_compile_definitions(stm32cubemx INTERFACE 
	USE_FULL_LL_DRIVER 
	USE_HAL_DRIVER 
	STM32F303xC 
	STM32_THREAD_SAFE_STRATEGY=4
    $<$<CONFIG:Debug>:DEBUG>
)

target_include_directories(stm32cubemx INTERFACE
    ../../Core/Inc
    ../../Drivers/STM32F3xx_HAL_Driver/Inc
    ../../Drivers/STM32F3xx_HAL_Driver/Inc/Legacy
    ../../Middlewares/Third_Party/FreeRTOS/Source/include
    ../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS
    ../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F
    ../../Drivers/CMSIS/Device/ST/STM32F3xx/Include
    ../../Drivers/CMSIS/Include
)

target_sources(stm32cubemx INTERFACE
    ../../Core/Src/main.c
    ../../Core/Src/gpio.c
    ../../Core/Src/freertos.c
    ../../Core/Src/dma.c
    ../../Core/Src/i2c.c
    ../../Core/Src/spi.c
    ../../Core/Src/tim.c
    ../../Core/Src/usart.c
    ../../Core/Src/usb.c
    ../../Core/Src/stm32f3xx_it.c
    ../../Core/Src/stm32f3xx_hal_msp.c
    ../../Core/Src/stm32f3xx_hal_timebase_tim.c
    ../../Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_ll_utils.c
    ../../Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_ll_exti.c
    ../../Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_ll_usb.c
    ../../Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_tim.c
    ../../Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_tim_ex.c
    ../../Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal.c
    ../../Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_rcc.c
    ../../Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_rcc_ex.c
    ../../Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_gpio.c
    ../../Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_dma.c
    ../../Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_cortex.c
    ../../Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_pwr.c
    ../../Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_pwr_ex.c
    ../../Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_flash.c
    ../../Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_flash_ex.c
    ../../Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_i2c.c
    ../../Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_i2c_ex.c
    ../../Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_exti.c
    ../../Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_ll_rcc.c
    ../../Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_spi.c
    ../../Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_spi_ex.c
    ../../Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_ll_tim.c
    ../../Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_ll_gpio.c
    ../../Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_ll_dma.c
    ../../Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_ll_usart.c
    ../../Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_pcd.c
    ../../Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_pcd_ex.c
    ../../Core/Src/system_stm32f3xx.c
    ../../Middlewares/Third_Party/FreeRTOS/Source/croutine.c
    ../../Middlewares/Third_Party/FreeRTOS/Source/event_groups.c
    ../../Middlewares/Third_Party/FreeRTOS/Source/list.c
    ../../Middlewares/Third_Party/FreeRTOS/Source/queue.c
    ../../Middlewares/Third_Party/FreeRTOS/Source/stream_buffer.c
    ../../Middlewares/Third_Party/FreeRTOS/Source/tasks.c
    ../../Middlewares/Third_Party/FreeRTOS/Source/timers.c
    ../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS/cmsis_os.c
    ../../Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/heap_4.c
    ../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F/port.c
    ../../Core/Src/sysmem.c
    ../../Core/Src/syscalls.c
    ../../startup_stm32f303xc.s
)

target_link_directories(stm32cubemx INTERFACE
)

target_link_libraries(stm32cubemx INTERFACE
)

# Validate that STM32CubeMX code is compatible with C standard
if(CMAKE_C_STANDARD LESS 11)
    message(ERROR "Generated code requires C11 or higher")
endif()


