#pragma once

#include <stdint.h>

typedef struct {
    uintptr_t rva;
#ifdef SHIRO_PATCH_VALIDATE
    const uint8_t *orig;
#endif
    const uint8_t *patch;
    size_t len;
    const char *sect;
} PatchEntry;