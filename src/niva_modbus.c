#include "niva_modbus.h"

#include "modbus/crc16.h"
#include "modbus/function_codes.h"
#include "modbus/data_model.h"
#include "modbus/endianess.h"

#include <string.h>

#ifdef NIVA_MODBUS_DEBUG
#include <stdio.h>
#define NIVA_MODBUS_DEBUG_PRINT(...) printf(__VA_ARGS__)
#else
#define NIVA_MODBUS_DEBUG_PRINT(...) (void) (0)
#endif


static inline size_t get_registers_count(ModbusServer* server, ModbusDataModelType type);
int8_t request_buffer_to_modbus_adu(uint8_t* data, size_t len, ModbusAdu* adu);
void finalize_response(ModbusAdu* adu, int8_t exception, uint8_t* data, size_t* data_len);


static int8_t parse_read_request(ModbusServer* server, ModbusPdu* pdu, ModbusRequestRead* request);
static int8_t parse_write_single_request(ModbusServer* server, ModbusPdu* pdu, ModbusRequestWrite* request);
static int8_t parse_write_multiple_request(ModbusServer* server, ModbusPdu* pdu, ModbusRequestWriteMultiple* request);


static int8_t build_read_coils_response(ModbusServer* server, ModbusRequestRead* request, ModbusPdu* pdu);
static int8_t build_read_discrete_inputs_response(ModbusServer* server, ModbusRequestRead* request, ModbusPdu* pdu);
static int8_t build_read_input_registers_response(ModbusServer* server, ModbusRequestRead* request, ModbusPdu* pdu);
static int8_t build_read_holding_registers_response(ModbusServer* server, ModbusRequestRead* request, ModbusPdu* pdu);
static int8_t build_echo_response(ModbusPdu* pdu, ModbusPdu* response_pdu);
static int8_t build_write_multiple_response(ModbusRequestWriteMultiple* request, ModbusPdu* pdu);


int8_t process_pdu(ModbusServer* server, ModbusPdu* request_pdu, ModbusPdu* response_pdu);
static int8_t process_read_coils(ModbusServer* server, ModbusPdu* request_pdu, ModbusPdu* response_pdu);
static int8_t process_read_discrete_inputs(ModbusServer* server, ModbusPdu* request_pdu, ModbusPdu* response_pdu);
static int8_t process_read_input_registers(ModbusServer* server, ModbusPdu* request_pdu, ModbusPdu* response_pdu);
static int8_t process_read_holding_registers(ModbusServer* server, ModbusPdu* request_pdu, ModbusPdu* response_pdu);

static int8_t process_write_coil(ModbusServer* server, ModbusPdu* pdu, ModbusPdu* response_pdu);
static int8_t process_write_register(ModbusServer* server, ModbusPdu* pdu, ModbusPdu* response_pdu);

static int8_t process_write_multiple_coils(ModbusServer* server, ModbusPdu* pdu, ModbusPdu* response_pdu);
static int8_t process_write_multiple_registers(ModbusServer* server, ModbusPdu* pdu, ModbusPdu* response_pdu);


int8_t modbus_server_poll(
    ModbusServer* server,
    uint8_t* data, 
    size_t data_len, 
    uint8_t* response_data, 
    size_t* response_data_len) {
        ModbusAdu adu = {0};
        int8_t result = request_buffer_to_modbus_adu(data, data_len, &adu);
        if (result != MODBUS_OK) {
            return result;
        }
        
        if (server->address != adu.server_address) {
            server->response_required = false;
            return MODBUS_OK;
        }

        server->response_required = true;
        ModbusAdu response_adu = {
            .server_address = server->address,
            .pdu = (ModbusPdu) {
                .function_code = adu.pdu.function_code,
                .data = &response_data[2],
                .data_len = *response_data_len - 4
            }
        };
        result = process_pdu(server, &adu.pdu, &response_adu.pdu);
        finalize_response(&response_adu, result, response_data, response_data_len);
        return result;
}

/// @brief Get count of registers of specified type
/// @param server ModbusServer*
/// @param type ModbusDataModelType
/// @return count of registers
static inline size_t get_registers_count(ModbusServer* server, ModbusDataModelType type) {
    if (type == MODBUS_TYPE_DISCRETE_INPUT) {
        return server->discrete_inputs_count;
    } else if (type == MODBUS_TYPE_COIL) {
        return server->coils_count;
    } else if (type == MODBUS_TYPE_INPUT_REGISTER) {
        return server->input_registers_count;
    } else if (type == MODBUS_TYPE_HOLDING_REGISTER) {
        return server->holding_registers_count;
    } else {
        return 0;
    }
}

/// @brief Converts input buffer of modbus server to ModbusAdu structure
/// @param data - request buffer
/// @param len - request buffer length
/// @param adu - ModbusAdu output structure
/// @return exception codes < 0 or 0 if all Ok
int8_t request_buffer_to_modbus_adu(uint8_t* data, size_t len, ModbusAdu* adu) {
    // TODO: add asserts

    if (len < MODBUS_MIN_FRAME_SIZE) {
        return MODBUS_ERR_FRAME_TO_SHORT;
    }

    uint16_t crc = uint8_to_uint16_little_endian(&data[len-2]);
    uint16_t calc_crc = crc16(data, len-2);
    
    if (crc != calc_crc) {
        return MODBUS_ERR_CRC_CHECK_FAILED;
    }

    adu->server_address = data[0];
    adu->pdu = (ModbusPdu) {
        .function_code = data[1],
        .data = &data[2],
        .data_len = len-4,
    };

    return MODBUS_OK;
}

/// @brief Creates final response modbus buffer from ModbusAdu structure
/// @param adu response modbus adu from process
/// @param exception exception code received in process
/// @param data output buffer
/// @param data_len output buffer length
void finalize_response(ModbusAdu* adu, int8_t exception, uint8_t* data, size_t* data_len) {
    data[0] = adu->server_address;
    if (exception == MODBUS_OK) {
        data[1] = adu->pdu.function_code;
        *data_len = adu->pdu.data_len + 4;
    } else {
        data[1] = adu->pdu.function_code + 0x80;
        data[2] = exception;
        *data_len = 5;
    }
    uint16_t crc = crc16(data, *data_len - 2);
    data[*data_len-2] = uint16_little_endian_hi(crc);
    data[*data_len-1] = uint16_little_endian_lo(crc);
}

/// @brief Desides which function should process PDU
/// @param server ModbusServer structure
/// @param pdu request PDU
/// @param response_pdu response PDU
/// @return exception code 0 - MODBUS_OK; >0 - Modbus exception
int8_t process_pdu(ModbusServer* server, ModbusPdu* pdu, ModbusPdu* response_pdu) {
    // TODO: assert pdu != null

    switch (pdu->function_code) {
        case MODBUS_FUN_R_COILS:
            return process_read_coils(server, pdu, response_pdu);
        case MODBUS_FUN_R_DISCRETE_INPUTS:
            return process_read_discrete_inputs(server, pdu, response_pdu);
        case MODBUS_FUN_W_SINGLE_COIL:
            return process_write_coil(server, pdu, response_pdu);
        case MODBUS_FUN_W_MULTIPLE_COILS:
            return process_write_multiple_coils(server, pdu, response_pdu);
        case MODBUS_FUN_R_INPUT_REGISTERS:
            return process_read_input_registers(server, pdu, response_pdu);
        case MODBUS_FUN_R_HOLDING_REGISTERS:
            return process_read_holding_registers(server, pdu, response_pdu);
        case MODBUS_FUN_W_SINGLE_REGISTER:
            return process_write_register(server, pdu, response_pdu);
        case MODBUS_FUN_W_MULTIPLE_REGISTERS:
            return process_write_multiple_registers(server, pdu, response_pdu);
        case MODBUS_FUN_RW_MULTIPLE_REGISTERS:
            return MODBUS_EXC_ILLEGAL_FUNCTION;           
        case MODBUS_FUN_MASK_WRITE_REGISTER:
            return MODBUS_EXC_ILLEGAL_FUNCTION;             
        case MODBUS_FUN_R_FIFO_QUEUE:
            return MODBUS_EXC_ILLEGAL_FUNCTION;                    
        case MODBUS_FUN_R_FILE_RECORD:
            return MODBUS_EXC_ILLEGAL_FUNCTION;                   
        case MODBUS_FUN_W_FILE_RECORD:
            return MODBUS_EXC_ILLEGAL_FUNCTION;                   
        case MODBUS_FUN_R_EXCEPTION_STATUS:
            return MODBUS_EXC_ILLEGAL_FUNCTION;              
        case MODBUS_FUN_DIAGNOSTIC:
            return MODBUS_EXC_ILLEGAL_FUNCTION;                      
        case MODBUS_FUN_GET_COM_EVENT_COUNTER:
            return MODBUS_EXC_ILLEGAL_FUNCTION;           
        case MODBUS_FUN_GET_COM_EVENT_LOG:
            return MODBUS_EXC_ILLEGAL_FUNCTION;               
        case MODBUS_FUN_REPORT_SLAVE_ID:
            return MODBUS_EXC_ILLEGAL_FUNCTION;                 
        case MODBUS_FUN_ENCAPSULATED_INTERFACE_TRANSPORT:
            return MODBUS_EXC_ILLEGAL_FUNCTION;
        default:
            return MODBUS_EXC_ILLEGAL_FUNCTION;
    }
}

// MARK: Parse

static int8_t parse_read_request(ModbusServer* server, ModbusPdu* pdu, ModbusRequestRead* request) {
    if (pdu->data_len != 4) {
        return MODBUS_EXC_ILLEGAL_DATA_VALUE;
    }

    request->starting_address = uint8_to_uint16_big_endian(&pdu->data[0]);
    request->quantity = uint8_to_uint16_big_endian(&pdu->data[2]);
    
    if (
        request->quantity < 1 ||
        request->quantity > get_registers_max_count(request->type) ||
        // TODO: возможно тут лучше возвращать доступный результат, а не ошибку
        request->starting_address + request->quantity > get_registers_count(server, request->type)
    ) {
        return MODBUS_EXC_ILLEGAL_DATA_ADDRESS;
    }
    
    return MODBUS_OK;
}

static int8_t parse_write_single_request(ModbusServer* server, ModbusPdu* pdu, ModbusRequestWrite* request) {
    if (pdu->data_len != 4) {
        return MODBUS_EXC_ILLEGAL_DATA_VALUE;
    }

    request->address = uint8_to_uint16_big_endian(&pdu->data[0]);
    uint16_t value = uint8_to_uint16_big_endian(&pdu->data[2]);

    if (request->type == MODBUS_TYPE_COIL) {
        if (value == 0xFF00) {
            request->value = 1;
        } else if (value == 0x0000) {
            request->value = 0;
        } else {
            return MODBUS_EXC_ILLEGAL_DATA_VALUE;
        }
    } else if (request->type == MODBUS_TYPE_HOLDING_REGISTER) {
        request->value = value;
    }

    if (
        request->address > get_registers_count(server, request->type)
    ) {
        return MODBUS_EXC_ILLEGAL_DATA_ADDRESS;
    }
    
    return MODBUS_OK;
}

static int8_t parse_write_multiple_request(ModbusServer* server, ModbusPdu* pdu, ModbusRequestWriteMultiple* request) {
    if (pdu->data_len < 5) {
        return MODBUS_EXC_ILLEGAL_DATA_VALUE;
    }

    request->starting_address = uint8_to_uint16_big_endian(&pdu->data[0]);
    request->quantity = uint8_to_uint16_big_endian(&pdu->data[2]);
    request->byte_count = pdu->data[4];

    if (request->quantity < 1 || request->quantity > get_registers_max_count(request->type)) {
        return MODBUS_EXC_ILLEGAL_DATA_VALUE;
    }
    
    if (request->type == MODBUS_TYPE_COIL) {
        if (request->byte_count != request->quantity / 8 + 1) {
            return MODBUS_EXC_ILLEGAL_DATA_VALUE;
        }
    } else {
        if (request->byte_count != request->quantity * 2) {
            return MODBUS_EXC_ILLEGAL_DATA_VALUE;
        }
    }

    if (request->starting_address + request->quantity > get_registers_count(server, request->type)) {
        return MODBUS_EXC_ILLEGAL_DATA_ADDRESS;
    }

    request->values = &pdu->data[5];
    
    return MODBUS_OK;
}

// MARK: Build
static int8_t build_read_coils_response(ModbusServer* server, ModbusRequestRead* request, ModbusPdu* pdu) {
    uint8_t bytes_len = (request->quantity + 7) / 8;
    memset(pdu->data, 0, bytes_len + 1);
    pdu->data[0] = bytes_len;
    for (uint16_t i = 0; i < request->quantity; i++) {
        uint16_t index = request->starting_address + i;
        uint16_t byte_index = i / 8;
        uint8_t bit_index = i % 8;
        if (server->coils[index]) {
            pdu->data[1 + byte_index] |= (1 << bit_index);
        }
    }
    pdu->data_len = bytes_len + 1;
    return MODBUS_OK;
}

static int8_t build_read_discrete_inputs_response(ModbusServer* server, ModbusRequestRead* request, ModbusPdu* pdu) {
    uint8_t bytes_len = (request->quantity + 7) / 8;
    memset(pdu->data, 0, bytes_len + 1);
    pdu->data[0] = bytes_len;
    for (uint16_t i = 0; i < request->quantity; i++) {
        uint16_t index = request->starting_address + i;
        uint16_t byte_index = i / 8;
        uint8_t bit_index = i % 8;
        if (server->discrete_inputs[index]) {
            pdu->data[1 + byte_index] |= (1 << bit_index);
        }
    }
    pdu->data_len = bytes_len + 1;
    return MODBUS_OK;
}

static int8_t build_read_input_registers_response(ModbusServer* server, ModbusRequestRead* request, ModbusPdu* pdu) {
    uint8_t bytes_len = request->quantity * 2;
    memset(pdu->data, 0, bytes_len + 1);
    pdu->data[0] = bytes_len;
    for (uint16_t i = 0; i < request->quantity; i++) {
        uint16_t index = request->starting_address + i;
        pdu->data[1 + (i * 2)]     = uint16_big_endian_hi(server->input_registers[index]);
        pdu->data[1 + (i * 2) + 1] = uint16_big_endian_lo(server->input_registers[index]);
    }
    pdu->data_len = bytes_len + 1;
    return MODBUS_OK;
}

static int8_t build_read_holding_registers_response(ModbusServer* server, ModbusRequestRead* request, ModbusPdu* pdu) {
    uint8_t bytes_len = request->quantity * 2;
    memset(pdu->data, 0, bytes_len + 1);
    pdu->data[0] = bytes_len;
    for (uint16_t i = 0; i < request->quantity; i++) {
        uint16_t index = request->starting_address + i;
        pdu->data[1 + (i * 2)]     = uint16_big_endian_hi(server->holding_registers[index]);
        pdu->data[1 + (i * 2) + 1] = uint16_big_endian_lo(server->holding_registers[index]);
    }
    pdu->data_len = bytes_len + 1;
    return MODBUS_OK;
}

static int8_t build_echo_response(ModbusPdu* pdu, ModbusPdu* response_pdu) {
    response_pdu->data_len = pdu->data_len;

    if (pdu->data_len > 0 && pdu->data != NULL) {
        memcpy(response_pdu->data, pdu->data, pdu->data_len);
    }
    return MODBUS_OK;
}

static int8_t build_write_multiple_response(ModbusRequestWriteMultiple* request, ModbusPdu* pdu) {
    pdu->data[0] = uint16_big_endian_hi(request->starting_address);
    pdu->data[1] = uint16_big_endian_lo(request->starting_address);
    pdu->data[2] = uint16_big_endian_hi(request->quantity);
    pdu->data[3] = uint16_big_endian_lo(request->quantity);
    pdu->data_len = 4;
    return MODBUS_OK;
}

// MARK: Process
static int8_t process_read_coils(ModbusServer* server, ModbusPdu* pdu, ModbusPdu* response_pdu) {
    ModbusRequestRead request = {
        .type = MODBUS_TYPE_COIL
    };
    int8_t result = parse_read_request(server, pdu, &request);
    if (result != MODBUS_OK) {
        return result;
    }

    return build_read_coils_response(server, &request, response_pdu);
}

static int8_t process_read_discrete_inputs(ModbusServer* server, ModbusPdu* pdu, ModbusPdu* response_pdu) {
    ModbusRequestRead request = {
        .type = MODBUS_TYPE_DISCRETE_INPUT
    };
    int8_t result = parse_read_request(server, pdu, &request);
    if (result != MODBUS_OK) {
        return result;
    }

    return build_read_discrete_inputs_response(server, &request, response_pdu);
}

static int8_t process_read_input_registers(ModbusServer* server, ModbusPdu* pdu, ModbusPdu* response_pdu) {
    ModbusRequestRead request = {
        .type = MODBUS_TYPE_DISCRETE_INPUT
    };
    int8_t result = parse_read_request(server, pdu, &request);
    if (result != MODBUS_OK) {
        return result;
    }

    return build_read_input_registers_response(server, &request, response_pdu);
}

static int8_t process_read_holding_registers(ModbusServer* server, ModbusPdu* pdu, ModbusPdu* response_pdu) {
    ModbusRequestRead request = {
        .type = MODBUS_TYPE_DISCRETE_INPUT
    };
    int8_t result = parse_read_request(server, pdu, &request);
    if (result != MODBUS_OK) {
        return result;
    }

    return build_read_holding_registers_response(server, &request, response_pdu);
}

static int8_t process_write_coil(ModbusServer* server, ModbusPdu* pdu, ModbusPdu* response_pdu) {
    ModbusRequestWrite request = {
        .type = MODBUS_TYPE_COIL
    };
    int8_t result = parse_write_single_request(server, pdu, &request);
    if (result != MODBUS_OK) {
        return result;
    }

    server->coils[request.address] = (Coil)request.value;

    return build_echo_response(pdu, response_pdu);
}

static int8_t process_write_register(ModbusServer* server, ModbusPdu* pdu, ModbusPdu* response_pdu) {
    ModbusRequestWrite request = {
        .type = MODBUS_TYPE_HOLDING_REGISTER
    };
    int8_t result = parse_write_single_request(server, pdu, &request);
    if (result != MODBUS_OK) {
        return result;
    }

    server->holding_registers[request.address] = request.value;

    return build_echo_response(pdu, response_pdu);
}

static int8_t process_write_multiple_coils(ModbusServer* server, ModbusPdu* pdu, ModbusPdu* response_pdu) {
    ModbusRequestWriteMultiple request = {
        .type = MODBUS_TYPE_COIL
    };
    int8_t result = parse_write_multiple_request(server, pdu, &request);
    if (result != MODBUS_OK) {
        return result;
    }

    uint16_t index = request.starting_address;
    for (uint16_t i = 0; i < request.quantity; i++) {
        uint16_t byte_index = i / 8;
        uint8_t bit_index = i % 8;
        
        bool coil_value = ((request.values[byte_index] >> bit_index) & 0x01) ? true : false;
        server->coils[index + i] = coil_value;
    }

    return build_write_multiple_response(&request, response_pdu);
}

static int8_t process_write_multiple_registers(ModbusServer* server, ModbusPdu* pdu, ModbusPdu* response_pdu) {
    ModbusRequestWriteMultiple request = {
        .type = MODBUS_TYPE_HOLDING_REGISTER
    };
    int8_t result = parse_write_multiple_request(server, pdu, &request);
    if (result != MODBUS_OK) {
        return result;
    }

    uint16_t index = request.starting_address;
    for (uint16_t i = 0; i < request.quantity; i++) {
        server->holding_registers[index + i] = uint8_to_uint16_big_endian(&request.values[i * 2]);
    }

    return build_write_multiple_response(&request, response_pdu);
}