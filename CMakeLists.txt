cmake_minimum_required(VERSION 3.16)
project(niva_modbus C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -pedantic -Wswitch-enum -Wcast-qual -Woverflow")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0 -g")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3 -g0")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Define BUILD_SHARED_LIBS=ON to build a dynamic library instead
add_library(niva_modbus)

target_sources(niva_modbus PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/niva_modbus.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/modbus/crc16.c
)

target_include_directories(niva_modbus PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

if(BUILD_NATIVE_EXAMPLES)
    add_executable(read_coils examples/native/read_coils.c)
    target_link_libraries(read_coils niva_modbus)
    target_compile_definitions(read_coils PUBLIC 
    )
    target_compile_definitions(niva_modbus PUBLIC 
        -DNIVA_MODBUS_DEBUG
    )
elseif(BUILD_STM32_EXAMPLES)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/examples/stm32)
endif()



if(BUILD_TESTS)
    add_library(unity STATIC)
    target_sources(unity PRIVATE tests/unity/src/unity.c)
    target_include_directories(unity PUBLIC tests/unity/src)

    enable_testing()
    
    add_executable(crc16_tests tests/crc16_tests.c src/modbus/crc16.c)
    target_include_directories(crc16_tests PRIVATE src/modbus)
    target_link_libraries(crc16_tests PRIVATE unity)
    add_test(NAME Crc16Tests COMMAND crc16_tests)
    
    add_executable(endianess_tests tests/endianess_tests.c)
    target_include_directories(endianess_tests PRIVATE src/modbus)
    target_link_libraries(endianess_tests PRIVATE unity)
    add_test(NAME EndianessTests COMMAND endianess_tests)

    add_executable(broken_packets_tests tests/broken_packets_tests.c)
    target_link_libraries(broken_packets_tests niva_modbus unity)
    add_test(NAME BrokenPacketsTests COMMAND broken_packets_tests)

    add_executable(read_coils_and_discretes_tests tests/read_coils_and_discretes_tests.c)
    target_link_libraries(read_coils_and_discretes_tests PRIVATE niva_modbus unity)
    add_test(NAME ReadCoilsAndDiscretes COMMAND read_coils_and_discretes_tests)
endif()