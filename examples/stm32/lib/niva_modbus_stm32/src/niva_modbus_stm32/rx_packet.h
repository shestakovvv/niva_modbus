#pragma once

#include <stdint.h>
#include <stddef.h>

typedef struct RxPacket {
    bool is_new;
    uint8_t *data;
    size_t len;
} RxPacket;
