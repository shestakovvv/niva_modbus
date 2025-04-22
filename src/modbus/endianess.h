#pragma once

#include <stdint.h>

static inline uint16_t uint8_to_uint16_big_endian(uint8_t* data) {
    return ((uint16_t)data[0] << 8) | ((uint16_t)data[1] & 0xff);
}

static inline uint16_t uint8_to_uint16_little_endian(uint8_t* data) {
    return ((uint16_t)data[1] << 8) | ((uint16_t)data[0] & 0xff);
}

static inline uint8_t uint16_big_endian_hi(uint16_t data) {
    return (uint8_t)(data >> 8);
}
static inline uint8_t uint16_big_endian_lo(uint16_t data) {
    return (uint8_t)data;
}

static inline uint8_t uint16_little_endian_hi(uint16_t data) {
    return (uint8_t)(data);
}
static inline uint8_t uint16_little_endian_lo(uint16_t data) {
    return (uint8_t)(data >> 8);
}