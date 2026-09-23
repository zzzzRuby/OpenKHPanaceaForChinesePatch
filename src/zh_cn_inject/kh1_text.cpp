#include <windows.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "kh1_text.hpp"
#include "../OpenKH.h"

namespace Shiro {

static const KH1S_StringPatch* GetKH1StringPatches(OpenKH::GameStoreId store) {
    switch(store) {
    case OpenKH::GameStoreId::Epic: return KH1S_PATCH_TABLE_epic_crack;
    case OpenKH::GameStoreId::Steam: return KH1S_PATCH_TABLE_steam;
    default: return NULL; 
    }
}

bool kh1_text_apply(HMODULE module, OpenKH::GameStoreId store) {
    const KH1S_StringPatch* patches = GetKH1StringPatches(store);
    if (patches == NULL) {
        return false;
    }

    const uintptr_t actualBase = (uintptr_t)module;

    uint32_t successes = 0;
    uint32_t failures = 0;
    uint32_t skipped = 0;

    for (int i = 0; i < KH1S_NUM_ENTRIES; ++i) {
        const KH1S_StringPatch* p = &patches[i];

        if (p->targetPtrRva == 0 || p->data == NULL) {
            ++skipped;
            continue;
        }

        void* newString = (void*)p->data;

        size_t length = p->length;

        void* targetSlot = (void*)(actualBase + p->targetPtrRva);

        DWORD oldProtect = 0;
        if (!VirtualProtect(targetSlot, 16, PAGE_READWRITE, &oldProtect)) {
            ++failures;
            continue;
        }

        *(void**)targetSlot = newString;

        uint64_t* lengthSlot = (uint64_t*)((uint8_t*)targetSlot + 8);
        uint64_t oldLengthQword = *lengthSlot;
        uint64_t newLengthQword = (oldLengthQword & 0xffffffff00000000ULL) | (length & 0xffffffffULL);
        *lengthSlot = newLengthQword;

        DWORD ignored = 0;
        VirtualProtect(targetSlot, 16, oldProtect, &ignored);

        ++successes;
    }

    printf("KH1FM string patch applied. Base: 0x%llx Success: %u Failed: %u Skipped: %u\n", (unsigned long long)actualBase, successes, failures, skipped);

    return failures == 0;
}

}