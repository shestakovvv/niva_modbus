#include "endianess.h"

#include "unity.h"

void test_uint8_to_uint16_big_endian(void) {
    {
        uint8_t data[2] = { 0x01, 0x02 };
        TEST_ASSERT_EQUAL_HEX16(0x0102, uint8_to_uint16_big_endian(data));
    };
    {
        uint8_t data[2] = { 0x31, 0x35 };
        TEST_ASSERT_EQUAL_HEX16(0x3135, uint8_to_uint16_big_endian(data));
    };
}

void test_uint8_to_uint16_little_endian(void) {
    {
        uint8_t data[2] = { 0x01, 0x02 };
        TEST_ASSERT_EQUAL_HEX16(0x0201, uint8_to_uint16_little_endian(data));
    };
    {
        uint8_t data[2] = { 0x31, 0x35 };
        TEST_ASSERT_EQUAL_HEX16(0x3531, uint8_to_uint16_little_endian(data));
    };
}

void test_uint16_big_endian(void) {
    {
        uint16_t data = 0x0102;
        TEST_ASSERT_EQUAL_HEX16(0x01, uint16_big_endian_hi(data));
        TEST_ASSERT_EQUAL_HEX16(0x02, uint16_big_endian_lo(data));
    };
    {
        uint16_t data = 0x3135;
        TEST_ASSERT_EQUAL_HEX16(0x31, uint16_big_endian_hi(data));
        TEST_ASSERT_EQUAL_HEX16(0x35, uint16_big_endian_lo(data));
    };
}

void test_uint16_little_endian(void) {
    {
        uint16_t data = 0x0102;
        TEST_ASSERT_EQUAL_HEX16(0x02, uint16_little_endian_hi(data));
        TEST_ASSERT_EQUAL_HEX16(0x01, uint16_little_endian_lo(data));
    };
    {
        uint16_t data = 0x3135;
        TEST_ASSERT_EQUAL_HEX16(0x35, uint16_little_endian_hi(data));
        TEST_ASSERT_EQUAL_HEX16(0x31, uint16_little_endian_lo(data));
    };
}

void setUp(void) {}
void tearDown(void) {}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_uint8_to_uint16_big_endian);
    RUN_TEST(test_uint8_to_uint16_little_endian);
    RUN_TEST(test_uint16_big_endian);
    RUN_TEST(test_uint16_little_endian);
    return UNITY_END();
}
