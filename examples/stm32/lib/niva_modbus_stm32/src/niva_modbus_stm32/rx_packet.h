#pragma once

#include <stdint.h>
#include <stddef.h>

typedef struct RxPacket {
    bool is_new;
    uint8_t *data;
    size_t len;
} RxPacket;

static inline void rx_packet_next(size_t* num) {
    (*num)++;
    if (*num >= MODBUS_RX_PACKETS_COUNT) {
        *num = 0;
    }
}