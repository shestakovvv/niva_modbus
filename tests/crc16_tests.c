#include "crc16.h"
#include "unity.h"

void test_empty(void) {
    uint8_t data[] = { 0 };
    TEST_ASSERT_EQUAL_UINT16(0x40bf, crc16(data, sizeof(data)));
}

void test_normal(void) {
    {
        uint8_t data[] = { 0x01, 0x02 };
        TEST_ASSERT_EQUAL_HEX16(0xE181, crc16(data, sizeof(data)));
    };
    {
        uint8_t data[] = { 0x12, 0x34, 0x05 };
        TEST_ASSERT_EQUAL_HEX16(0x0607, crc16(data, sizeof(data)));
    };
}

void setUp(void) {}
void tearDown(void) {}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_empty);
    RUN_TEST(test_normal);
    return UNITY_END();
}
