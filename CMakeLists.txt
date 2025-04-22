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



# if(BUILD_TESTS)
#     add_executable(nanomodbus_tests nanomodbus.c tests/nanomodbus_tests.c)
#     target_link_libraries(nanomodbus_tests pthread)

#     add_executable(server_disabled nanomodbus.c tests/server_disabled.c)
#     target_compile_definitions(server_disabled PUBLIC NMBS_SERVER_DISABLED)

#     add_executable(client_disabled nanomodbus.c tests/client_disabled.c)
#     target_compile_definitions(client_disabled PUBLIC NMBS_CLIENT_DISABLED)

#     add_executable(multi_server_rtu nanomodbus.c tests/multi_server_rtu.c)
#     target_compile_definitions(multi_server_rtu PUBLIC NMBS_DEBUG)
#     target_link_libraries(multi_server_rtu pthread)

#     enable_testing()
#     add_test(NAME test_general COMMAND $<TARGET_FILE:nanomodbus_tests>)
#     add_test(NAME test_server_disabled COMMAND $<TARGET_FILE:server_disabled>)
#     add_test(NAME test_client_disabled COMMAND $<TARGET_FILE:client_disabled>)
#     add_test(NAME test_multi_server_rtu COMMAND $<TARGET_FILE:multi_server_rtu>)
# endif()