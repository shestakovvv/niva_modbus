cmake_minimum_required(VERSION 3.22)

set(LIB_NAME niva_modbus_stm32)

project(${LIB_NAME})
add_library(${LIB_NAME} INTERFACE)

# Enable CMake support for ASM and C languages
enable_language(C ASM)

if(NIVA_MODBUS_STM32_USE_DEFAULT_CONFIG)
set(NIVA_MODBUS_STM32_CONFIG_DIR src/template)
set(NIVA_MODBUS_STM32_SOURCE src/template/niva_modbus_stm32_config.h)
endif()

target_include_directories(${LIB_NAME} INTERFACE
    src/
    ${NIVA_MODBUS_STM32_CONFIG_DIR}
)

target_sources(${LIB_NAME} INTERFACE
    src/niva_modbus_stm32.c
    src/niva_modbus_stm32/usart.c
    src/niva_modbus_stm32/usart.h
    src/niva_modbus_stm32/tim.h
    src/niva_modbus_stm32/dma.h
    src/niva_modbus_stm32/gpio.h
    src/niva_modbus_stm32/rx_packet.h
    ${NIVA_MODBUS_STM32_SOURCE}
)

target_link_directories(${LIB_NAME} INTERFACE
)

target_link_libraries(${LIB_NAME} INTERFACE
)

# Validate that ${LIB_NAME} code is compatible with C standard
if(CMAKE_C_STANDARD LESS 11)
    message(ERROR "Generated code requires C11 or higher")
endif()


