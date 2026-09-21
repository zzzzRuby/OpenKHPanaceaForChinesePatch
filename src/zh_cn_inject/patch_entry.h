#pragma once

#include <stdint.h>

typedef struct {
    uintptr_t rva;
    const uint8_t *orig;
    const uint8_t *patch;
    size_t len;
    const char *sect;
} PatchEntry;