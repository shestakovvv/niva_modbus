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
            printf("%02X ", response_buffer[i]); // Print each byte in hexadecimal
        }
        printf("]\n");
    }
    return 0;
}