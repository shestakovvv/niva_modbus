# Библиотека для построения modbus пакетов.

Библиотека построена на основе [документации](https://modbus.org/docs/Modbus_Application_Protocol_V1_1b.pdf) с сайта [modbus.org](https://modbus.org/).

## Принцип работы:

**Входной буфер** -> Modbus ADU -> Modbus PDU -> Modbus response (exception response) -> **Выходной буфер**

Таким образом, библиотека не включает в себя сервер и транспортный уровень. Библиотека только обрабатывает запросы и работает с котекстом Modbus.

## Пример работы:

```c
#include "niva_modbus.h"
#include "string.h"
#include "stdio.h"

#define DISCRETE_INPUTS_COUNT   100
#define COILS_COUNT             100
#define INPUT_REGISTERS_COUNT   100
#define HOLDING_REGISTERS_COUNT 100
DiscreteInput discrete_inputs[DISCRETE_INPUTS_COUNT] = {0};
Coil coils[COILS_COUNT] = {0};
InputRegister input_registers[INPUT_REGISTERS_COUNT] = {0};
HoldingRegister holding_registers[HOLDING_REGISTERS_COUNT] = {0};

int main() {
    ModbusServer server = {
        .address = 1,
        .discrete_inputs = discrete_inputs,
        .discrete_inputs_count = DISCRETE_INPUTS_COUNT,
        .coils = coils,
        .coils_count = COILS_COUNT,
        .input_registers = input_registers,
        .input_registers_count = INPUT_REGISTERS_COUNT,
        .holding_registers = holding_registers,
        .holding_registers_count = HOLDING_REGISTERS_COUNT,
    };
    server.coils[24] = 1;
    server.coils[32] = 1;
    uint8_t request_buffer[256] = {
        0x01, 0x03, 0x00, 0x00, 0x00, 0x0a, 0xC5, 0xCD
    };
    uint8_t response_buffer[256] = {0};
    size_t response_len = 0;
    int8_t result = modbus_server_poll(&server, request_buffer, 8, response_buffer, &response_len);
    if (!server.response_required) {
        if (result != MODBUS_OK) {
            printf("Error: %d\n", result);
        }
        printf("Nothing to do.");
    } else {
        if (result != MODBUS_OK) {
            printf("Error: %d", result);
        }
        printf("result: [");
        for (size_t i = 0; i < response_len; i++) {
            printf("%02X ", response_buffer[i]);
        }
        printf("]\n");
    }
    return 0;
}
```

### Другие примеры работы можно посмотреть в папке examples и tests


## Подключение в проект CMake

```CMake
include(FetchContent)
FetchContent_Declare(
        niva_modbus
        GIT_REPOSITORY https://github.com/shestakovvv/niva_modbus.git
        GIT_TAG main # or the version you want
        GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(niva_modbus)

...

add_executable(your_program source_codes)
target_link_libraries(your_program niva_modbus)
```

## ToDo list:

- [ ] Добавить функционал для modbus клиента
- [ ] Добавить assert'ы где необходимо