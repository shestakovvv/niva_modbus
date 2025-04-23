#include <unity.h>

#include "niva_modbus.h"

DiscreteInput discrete_inputs[100] = {0};
Coil coils[100] = {0};

ModbusServer server = {
    .address = 1,
    .discrete_inputs = discrete_inputs,
    .discrete_inputs_count = 100,
    .coils = coils,
    .coils_count = 100,
    .input_registers = NULL,
    .input_registers_count = 0,
    .holding_registers = NULL,
    .holding_registers_count = 0,
};

uint8_t response_buffer[256] = {0};
size_t response_len = 0;

void test_read_coils(void) {
    {
        uint8_t request[256] = {
            0x01, 0x01, 0x00, 0x13, 0x00, 0x13, 0x8C, 0x02
        };
        int8_t result = modbus_server_poll(&server, request, 8, response_buffer, &response_len);
        TEST_ASSERT_EQUAL(MODBUS_OK, result);
        TEST_ASSERT_EQUAL(true, server.response_required);
        uint8_t expected[] = { 0x01, 0x01, 0x03, 0x00, 0x00, 0x00, 0x3C, 0x4E };
        TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, response_buffer, response_len);
    };
    {
        uint8_t request[256] = {
            0x01, 0x01, 0x00, 0x13, 0x00, 0x13, 0x8C, 0x02
        };
        server.coils[0x13] = true;
        server.coils[0x13 + 0x13 - 2] = true;
        int8_t result = modbus_server_poll(&server, request, 8, response_buffer, &response_len);
        TEST_ASSERT_EQUAL(MODBUS_OK, result);
        TEST_ASSERT_EQUAL(true, server.response_required);
        uint8_t expected[] = { 0x01, 0x01, 0x03, 0x01, 0x00, 0x02, 0xEC, 0x4F };
        TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, response_buffer, response_len);
    };
    // illegal address start + quantity
    {
        uint8_t request[256] = {
            0x01, 0x01, 0x00, 0x63, 0x00, 0x02, 0x4D, 0xD5
        };
        int8_t result = modbus_server_poll(&server, request, 8, response_buffer, &response_len);
        TEST_ASSERT_EQUAL(MODBUS_EXC_ILLEGAL_DATA_ADDRESS, result);
        TEST_ASSERT_EQUAL(true, server.response_required);
        uint8_t expected[] = { 0x01, 0x81, 0x02, 0xC1, 0x91 };
        TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, response_buffer, response_len);
    };
    // illegal address quantity > len
    {
        uint8_t request[256] = {
            0x01, 0x01, 0x00, 0x00, 0x00, 0x65, 0xFC, 0x21
        };
        int8_t result = modbus_server_poll(&server, request, 8, response_buffer, &response_len);
        TEST_ASSERT_EQUAL(MODBUS_EXC_ILLEGAL_DATA_ADDRESS, result);
        TEST_ASSERT_EQUAL(true, server.response_required);
        uint8_t expected[] = { 0x01, 0x81, 0x02, 0xC1, 0x91 };
        TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, response_buffer, response_len);
    };
    // illegal address quantity 0
    {
        uint8_t request[256] = {
            0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x0A
        };
        int8_t result = modbus_server_poll(&server, request, 8, response_buffer, &response_len);
        TEST_ASSERT_EQUAL(MODBUS_EXC_ILLEGAL_DATA_ADDRESS, result);
        TEST_ASSERT_EQUAL(true, server.response_required);
        uint8_t expected[] = { 0x01, 0x81, 0x02, 0xC1, 0x91 };
        TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, response_buffer, response_len);
    };
    // illegal data value
    {
        uint8_t request[256] = {
            0x01, 0x01, 0x00, 0x00, 0x00, 0x18, 0x3C
        };
        int8_t result = modbus_server_poll(&server, request, 7, response_buffer, &response_len);
        TEST_ASSERT_EQUAL(MODBUS_EXC_ILLEGAL_DATA_VALUE, result);
        TEST_ASSERT_EQUAL(true, server.response_required);
        uint8_t expected[] = { 0x01, 0x81, 0x03, 0x00, 0x51 };
        TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, response_buffer, response_len);
    };
}

void setUp(void) {}
void tearDown(void) {}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_read_coils);
    return UNITY_END();
}
