#pragma once

/// https://modbus.org/docs/Modbus_Application_Protocol_V1_1b.pdf

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "modbus/data_model.h"
#include "modbus/exception_codes.h"
#include "modbus/function_codes.h"

typedef bool DiscreteInput;
typedef bool Coil;
typedef uint16_t InputRegister;
typedef uint16_t HoldingRegister;

typedef struct ModbusServer {
    uint8_t address;
    bool response_required;
    DiscreteInput* discrete_inputs;
    size_t discrete_inputs_count;
    Coil* coils;
    size_t coils_count;
    InputRegister* input_registers;
    size_t input_registers_count;
    HoldingRegister* holding_registers;
    size_t holding_registers_count;

#ifdef NIVA_MODBUS_USE_BEFORE_RESPONSE_HOOK
    int8_t (*before_response_hook)(ModbusRequest*);
#endif
} ModbusServer;

int8_t modbus_server_poll(
    ModbusServer* server,
    uint8_t* data, 
    size_t data_len, 
    uint8_t* response_data, 
    size_t* response_data_len);

int8_t modbus_buffer_to_adu(uint8_t* data, size_t data_len, ModbusAdu* adu);
