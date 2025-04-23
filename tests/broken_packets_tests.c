#include <unity.h>

#include "niva_modbus.h"

DiscreteInput discrete_inputs[100] = {0};
Coil coils[100] = {0};
InputRegister input_registers[100] = {0};
HoldingRegister holding_registers[100] = {0};

ModbusServer server = {
    .address = 1,
    .discrete_inputs = discrete_inputs,
    .discrete_inputs_count = 100,
    .coils = coils,
    .coils_count = 100,
    .input_registers = input_registers,
    .input_registers_count = 100,
    .holding_registers = holding_registers,
    .holding_registers_count = 100,
};

uint8_t response_buffer[256] = {0};
size_t response_len = 0;

void test_broken_packet_to_another_server(void) {
    uint8_t request[256] = {
        0x0, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07
    };

    int8_t result = modbus_server_poll(&server, request, 8, response_buffer, &response_len);
    TEST_ASSERT_EQUAL_INT8(MODBUS_OK, result);
    TEST_ASSERT_EQUAL(false, server.response_required);
}

void test_broken_packet(void) {
    uint8_t request[256] = {
        0x01, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07
    };

    int8_t result = modbus_server_poll(&server, request, 8, response_buffer, &response_len);
    TEST_ASSERT_EQUAL_INT8(MODBUS_ERR_CRC_CHECK_FAILED, result);
    TEST_ASSERT_EQUAL(false, server.response_required);
}

void test_empty_frame(void) {
    uint8_t request[256] = {
        0x01, 0x01
    };

    int8_t result = modbus_server_poll(&server, request, 0, response_buffer, &response_len);
    TEST_ASSERT_EQUAL_INT8(MODBUS_ERR_FRAME_TO_SHORT, result);
    TEST_ASSERT_EQUAL(false, server.response_required);
}

void setUp(void) {}
void tearDown(void) {}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_broken_packet_to_another_server);
    RUN_TEST(test_broken_packet);
    RUN_TEST(test_empty_frame);
    return UNITY_END();
}
