#pragma once

#define MODBUS_MIN_FRAME_SIZE               7
#define MODBUS_DISCRETE_INPUTS_MAX_COUNT    0x7D0 // TODO: add assert on this in init
#define MODBUS_COILS_MAX_COUNT              0x7D0
#define MODBUS_HOLDING_REGISTERS_MAX_COUNT  0x7D
#define MODBUS_INPUT_REGISTERS_MAX_COUNT    0x7D

typedef enum ModbusDataModelType {
    MODBUS_TYPE_DISCRETE_INPUT = 0,
    MODBUS_TYPE_COIL = 1,
    MODBUS_TYPE_INPUT_REGISTER = 2,
    MODBUS_TYPE_HOLDING_REGISTER = 3,
} ModbusDataModelType;

typedef struct ModbusPdu {
    uint8_t function_code;
    uint8_t* data;
    size_t data_len;
} ModbusPdu;

typedef struct ModbusRequestRead {
    ModbusDataModelType type;
    uint16_t starting_address;
    uint16_t quantity;
} ModbusRequestRead;   

typedef struct ModbusRequestWrite {
    ModbusDataModelType type;
    uint16_t address;
    uint16_t value;
} ModbusRequestWrite;  

typedef struct ModbusRequestWriteMultiple {
    ModbusDataModelType type;
    uint16_t starting_address;
    uint16_t quantity;
    uint8_t byte_count;
    uint8_t* values;
} ModbusRequestWriteMultiple;

typedef struct ModbusRequestReadWrite {
    ModbusDataModelType type;
    uint16_t read_starting_address;
    uint16_t read_quantity;
    uint16_t write_starting_address;
    uint16_t write_quantity;
    uint8_t write_byte_count;
    uint8_t* write_values;
} ModbusRequestReadWrite;

typedef struct ModbusAdu {
    uint8_t server_address;
    ModbusPdu pdu;
} ModbusAdu;


static inline size_t get_registers_max_count(ModbusDataModelType type) {
    if (type == MODBUS_TYPE_DISCRETE_INPUT) {
        return MODBUS_DISCRETE_INPUTS_MAX_COUNT;
    } else if (type == MODBUS_TYPE_COIL) {
        return MODBUS_COILS_MAX_COUNT;
    } else if (type == MODBUS_TYPE_INPUT_REGISTER) {
        return MODBUS_INPUT_REGISTERS_MAX_COUNT;
    } else if (type == MODBUS_TYPE_HOLDING_REGISTER) {
        return MODBUS_HOLDING_REGISTERS_MAX_COUNT;
    } else {
        return 0;
    }
}