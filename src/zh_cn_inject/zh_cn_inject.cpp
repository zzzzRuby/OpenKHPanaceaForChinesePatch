#include "khlauncher_cn_steam_patch.hpp"
#include "kh1_cn_steam_patch.hpp"
#include "khtheater_cn_steam_patch.hpp"
#include "khlauncher_cn_epic_crack_patch.hpp"
#include "kh1_cn_epic_crack_patch.hpp"
#include "khtheater_cn_epic_crack_patch.hpp"
#include <windows.h>
#include <stdio.h>
#include "../OpenKH.h"

namespace Shiro {

bool kh1_text_apply(HMODULE module, OpenKH::GameStoreId store);

static bool apply_patch(HMODULE module, const PatchEntry* entries, size_t entry_count, const char* patch_name) {
    unsigned char *base = (unsigned char *)module;
    size_t applied = 0;
    for (size_t i = 0; i < entry_count; i++) {
        const PatchEntry *p = &entries[i];
        unsigned char *addr = base + p->rva;
        if (memcmp(addr, p->orig, p->len) != 0) {
            if (memcmp(addr, p->patch, p->len) == 0) {
                applied++;
                continue;
            }
            printf("[%s] Patch %zu verification failed at RVA 0x%llX (%s)",
                patch_name, i, (unsigned long long)p->rva, p->sect);
            continue;
        }
        DWORD oldProtect;
        if (!VirtualProtect(addr, p->len, PAGE_EXECUTE_READWRITE, &oldProtect)) {
            printf("[%s] VirtualProtect failed for patch %zu at RVA 0x%llX err=%lu",
                patch_name, i, (unsigned long long)p->rva, GetLastError());
            continue;
        }
        memcpy(addr, p->patch, p->len);
        DWORD dummy;
        VirtualProtect(addr, p->len, oldProtect, &dummy);
        applied++;
    }
    return applied == entry_count;
}

static uint8_t sys_font_tbl[0x10000] = { 0 };

static bool khlauncher_cn_steam_Apply(HMODULE module)
{
    constexpr size_t count = sizeof(khlauncher_cn_steam_patches) / sizeof(khlauncher_cn_steam_patches[0]);
    return apply_patch(module, khlauncher_cn_steam_patches, count, "khlauncher_cn_steam");
}

static constexpr PatchEntry kh1_cn_steam_patches_in_dll[] = {
    kh1_cn_steam_entry_0,
    kh1_cn_steam_entry_1,
    kh1_cn_steam_entry_2,
    kh1_cn_steam_entry_3,
    kh1_cn_steam_entry_4,
    kh1_cn_steam_entry_5,
    kh1_cn_steam_entry_6,
    kh1_cn_steam_entry_7,
    kh1_cn_steam_entry_8,
    kh1_cn_steam_entry_9,
    kh1_cn_steam_entry_10,
    kh1_cn_steam_entry_11,
    kh1_cn_steam_entry_12,
    kh1_cn_steam_entry_13,
    kh1_cn_steam_entry_14,
    kh1_cn_steam_entry_15,
    kh1_cn_steam_entry_16,
    kh1_cn_steam_entry_17,
    kh1_cn_steam_entry_18,
    kh1_cn_steam_entry_19,
    kh1_cn_steam_entry_20,
    kh1_cn_steam_entry_21,
    kh1_cn_steam_entry_22,
    kh1_cn_steam_entry_23,
    kh1_cn_steam_entry_24,
    kh1_cn_steam_entry_25,
    kh1_cn_steam_entry_26,
    kh1_cn_steam_entry_27,
    kh1_cn_steam_entry_28,
    //remove 29
    kh1_cn_steam_entry_30,
    kh1_cn_steam_entry_31,
    kh1_cn_steam_entry_32,
    kh1_cn_steam_entry_33,
    kh1_cn_steam_entry_34,
    kh1_cn_steam_entry_35,
    kh1_cn_steam_entry_36,
    kh1_cn_steam_entry_37,
    kh1_cn_steam_entry_38,
    kh1_cn_steam_entry_39,
    kh1_cn_steam_entry_40,
    kh1_cn_steam_entry_41,
    kh1_cn_steam_entry_42,
    kh1_cn_steam_entry_43,
    kh1_cn_steam_entry_44,
    kh1_cn_steam_entry_45,
};

static bool kh1_cn_steam_Apply(HMODULE module)
{
    constexpr size_t count = sizeof(kh1_cn_steam_patches_in_dll) / sizeof(kh1_cn_steam_patches_in_dll[0]);
    if (!apply_patch(module, kh1_cn_steam_patches_in_dll, count, "kh1_cn_steam")) {
        return false;
    }

    constexpr auto kh1_cn_steam_patch_29_rva = kh1_cn_steam_entry_29.rva;
    constexpr uint8_t kh1_cn_steam_orig_29_in_dll[11] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    uint8_t kh1_cn_steam_patch_29_in_dll[11];
    
    kh1_cn_steam_patch_29_in_dll[0] = 0x48; // mov rbx, ...
    kh1_cn_steam_patch_29_in_dll[1] = 0xbb; 
    *(uintptr_t*)(kh1_cn_steam_patch_29_in_dll + 2) = (uintptr_t)sys_font_tbl;
    kh1_cn_steam_patch_29_in_dll[10] = 0xc3; // ret

    PatchEntry g_kh1_cn_steam_patch_29_in_dll = { 
        kh1_cn_steam_patch_29_rva, 
        kh1_cn_steam_orig_29_in_dll,
        kh1_cn_steam_patch_29_in_dll,
        sizeof(kh1_cn_steam_patch_29_in_dll),
        ".text(padding)" 
    };

    return apply_patch(module, &g_kh1_cn_steam_patch_29_in_dll, 1, "kh1_cn_steam");
}

static bool khtheater_cn_steam_Apply(HMODULE module)
{
    constexpr size_t count = sizeof(khtheater_cn_steam_patches) / sizeof(khtheater_cn_steam_patches[0]);
    return apply_patch(module, khtheater_cn_steam_patches, count, "khtheater_cn_steam");
}

static bool khlauncher_cn_epic_crack_Apply(HMODULE module)
{
    constexpr size_t count = sizeof(khlauncher_cn_epic_crack_patches) / sizeof(khlauncher_cn_epic_crack_patches[0]);
    return apply_patch(module, khlauncher_cn_epic_crack_patches, count, "khlauncher_cn_epic_crack");
}

static constexpr PatchEntry kh1_cn_epic_crack_patches_in_dll[] = {
    kh1_cn_epic_crack_entry_0,
    kh1_cn_epic_crack_entry_1,
    kh1_cn_epic_crack_entry_2,
    kh1_cn_epic_crack_entry_3,
    kh1_cn_epic_crack_entry_4,
    kh1_cn_epic_crack_entry_5,
    kh1_cn_epic_crack_entry_6,
    kh1_cn_epic_crack_entry_7,
    kh1_cn_epic_crack_entry_8,
    kh1_cn_epic_crack_entry_9,
    kh1_cn_epic_crack_entry_10,
    kh1_cn_epic_crack_entry_11,
    kh1_cn_epic_crack_entry_12,
    kh1_cn_epic_crack_entry_13,
    kh1_cn_epic_crack_entry_14,
    kh1_cn_epic_crack_entry_15,
    kh1_cn_epic_crack_entry_16,
    kh1_cn_epic_crack_entry_17,
    kh1_cn_epic_crack_entry_18,
    kh1_cn_epic_crack_entry_19,
    kh1_cn_epic_crack_entry_20,
    kh1_cn_epic_crack_entry_21,
    kh1_cn_epic_crack_entry_22,
    kh1_cn_epic_crack_entry_23,
    kh1_cn_epic_crack_entry_24,
    kh1_cn_epic_crack_entry_25,
    kh1_cn_epic_crack_entry_26,
    kh1_cn_epic_crack_entry_27,
    kh1_cn_epic_crack_entry_28,
    //remove 29
    kh1_cn_epic_crack_entry_30,
    kh1_cn_epic_crack_entry_31,
    kh1_cn_epic_crack_entry_32,
    kh1_cn_epic_crack_entry_33,
    kh1_cn_epic_crack_entry_34,
    kh1_cn_epic_crack_entry_35,
    kh1_cn_epic_crack_entry_36,
    kh1_cn_epic_crack_entry_37,
    kh1_cn_epic_crack_entry_38,
    kh1_cn_epic_crack_entry_39,
    kh1_cn_epic_crack_entry_40,
    kh1_cn_epic_crack_entry_41,
    kh1_cn_epic_crack_entry_42,
    kh1_cn_epic_crack_entry_43,
};

static bool kh1_cn_epic_crack_Apply(HMODULE module)
{
    constexpr size_t count = sizeof(kh1_cn_epic_crack_patches_in_dll) / sizeof(kh1_cn_epic_crack_patches_in_dll[0]);
    if (!apply_patch(module, kh1_cn_epic_crack_patches_in_dll, count, "kh1_cn_epic_crack")) {
        return false;
    }

    constexpr auto kh1_cn_epic_crack_patch_29_rva = kh1_cn_epic_crack_entry_29.rva;
    constexpr uint8_t kh1_cn_epic_crack_orig_29_in_dll[11] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    uint8_t kh1_cn_epic_crack_patch_29_in_dll[11];
    
    kh1_cn_epic_crack_patch_29_in_dll[0] = 0x48; // mov rbx, ...
    kh1_cn_epic_crack_patch_29_in_dll[1] = 0xbb; 
    *(uintptr_t*)(kh1_cn_epic_crack_patch_29_in_dll + 2) = (uintptr_t)sys_font_tbl;
    kh1_cn_epic_crack_patch_29_in_dll[10] = 0xc3; // ret

    PatchEntry g_kh1_cn_epic_crack_patch_29_in_dll = { 
        kh1_cn_epic_crack_patch_29_rva, 
        kh1_cn_epic_crack_orig_29_in_dll,
        kh1_cn_epic_crack_patch_29_in_dll,
        sizeof(kh1_cn_epic_crack_patch_29_in_dll),
        ".text(padding)" 
    };

    return apply_patch(module, &g_kh1_cn_epic_crack_patch_29_in_dll, 1, "kh1_cn_epic_crack");
}

static bool khtheater_cn_epic_crack_Apply(HMODULE module)
{
    constexpr size_t count = sizeof(khtheater_cn_epic_crack_patches) / sizeof(khtheater_cn_epic_crack_patches[0]);
    return apply_patch(module, khtheater_cn_epic_crack_patches, count, "khtheater_cn_epic_crack");
}

bool kh1_cn_Apply(HMODULE module, OpenKH::GameStoreId store)
{
    bool result = false;
    switch(store) {
    case OpenKH::GameStoreId::Steam: result = kh1_cn_steam_Apply(module); break;
    case OpenKH::GameStoreId::Epic: result = kh1_cn_epic_crack_Apply(module); break;
    default: break;
    }

    if (!result) {
        return false;
    }

    return kh1_text_apply(module, store);
}

bool khlauncher_cn_Apply(HMODULE module, OpenKH::GameStoreId store)
{
    switch(store) {
    case OpenKH::GameStoreId::Steam: return khlauncher_cn_steam_Apply(module);
    case OpenKH::GameStoreId::Epic: return khlauncher_cn_epic_crack_Apply(module);
    default: return false;
    }
}

bool khtheater_cn_Apply(HMODULE module, OpenKH::GameStoreId store)
{
    switch(store) {
    case OpenKH::GameStoreId::Steam: return khtheater_cn_steam_Apply(module);
    case OpenKH::GameStoreId::Epic: return khtheater_cn_epic_crack_Apply(module);
    default: return false;
    }
}
}