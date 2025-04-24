cmake_minimum_required(VERSION 3.22)

set(LIB_NAME freemodbus)
project(${LIB_NAME})
add_library(${LIB_NAME} INTERFACE)

# Enable CMake support for ASM and C languages
enable_language(C ASM)

target_compile_definitions(${LIB_NAME} INTERFACE 
	USE_HAL_DRIVER 
	STM32F303xC
    $<$<CONFIG:Debug>:DEBUG>
)

target_include_directories(${LIB_NAME} INTERFACE
    ./modbus/include
    ./modbus/rtu
    ./port
)

target_sources(${LIB_NAME} INTERFACE
    ./port/portserial.c
    ./port/porttimer.c
    ./port/portevent.c
    ./port/port.c

    ./modbus/mb.c

    ./modbus/functions/mbfunccoils.c
    ./modbus/functions/mbfuncdiag.c
    ./modbus/functions/mbfuncdisc.c
    ./modbus/functions/mbfuncholding.c
    ./modbus/functions/mbfuncinput.c
    ./modbus/functions/mbfuncother.c
    ./modbus/functions/mbutils.c

    ./modbus/rtu/mbcrc.c
    ./modbus/rtu/mbrtu.c
)

target_link_directories(${LIB_NAME} INTERFACE
)

target_link_libraries(${LIB_NAME} INTERFACE
    stm32cubemx
)

# Validate that ${LIB_NAME} code is compatible with C standard
if(CMAKE_C_STANDARD LESS 11)
    message(ERROR "Generated code requires C11 or higher")
endif()


