#include "kh1_cn_steam_patch.hpp"
#include "khlauncher_cn_steam_patch.hpp"
#include "khtheater_cn_steam_patch.hpp"
#include <windows.h>
#include <stdio.h>

namespace Shiro {

static int apply_patch(HMODULE module, const PatchEntry* entries, size_t entry_count, const char* patch_name) {
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
    return (applied == entry_count) ? 1 : 0;
}

static uint8_t g_sys_font_tbl[0x10000] = { 0 };

PatchEntry g_kh1_cn_steam_patches_in_dll[] = {
    g_kh1_cn_steam_patch_0,
    g_kh1_cn_steam_patch_1,
    g_kh1_cn_steam_patch_2,
    g_kh1_cn_steam_patch_3,
    g_kh1_cn_steam_patch_4,
    g_kh1_cn_steam_patch_5,
    g_kh1_cn_steam_patch_6,
    g_kh1_cn_steam_patch_7,
    g_kh1_cn_steam_patch_8,
    g_kh1_cn_steam_patch_9,
    g_kh1_cn_steam_patch_10,
    g_kh1_cn_steam_patch_11,
    g_kh1_cn_steam_patch_12,
    g_kh1_cn_steam_patch_13,
    g_kh1_cn_steam_patch_14,
    g_kh1_cn_steam_patch_15,
    g_kh1_cn_steam_patch_16,
    g_kh1_cn_steam_patch_17,
    g_kh1_cn_steam_patch_18,
    g_kh1_cn_steam_patch_19,
    g_kh1_cn_steam_patch_20,
    g_kh1_cn_steam_patch_21,
    g_kh1_cn_steam_patch_22,
    g_kh1_cn_steam_patch_23,
    g_kh1_cn_steam_patch_24,
    g_kh1_cn_steam_patch_25,
    g_kh1_cn_steam_patch_26,
    g_kh1_cn_steam_patch_27,
    g_kh1_cn_steam_patch_28,
    //remove 29
    g_kh1_cn_steam_patch_30,
    g_kh1_cn_steam_patch_31,
    g_kh1_cn_steam_patch_32,
    g_kh1_cn_steam_patch_33,
    g_kh1_cn_steam_patch_34,
    g_kh1_cn_steam_patch_35,
    g_kh1_cn_steam_patch_36,
    g_kh1_cn_steam_patch_37,
    g_kh1_cn_steam_patch_38,
    g_kh1_cn_steam_patch_39,
    g_kh1_cn_steam_patch_40,
    g_kh1_cn_steam_patch_41,
    g_kh1_cn_steam_patch_42,
    g_kh1_cn_steam_patch_43,
    g_kh1_cn_steam_patch_44,
    g_kh1_cn_steam_patch_45,
};

int kh1_cn_steam_Apply(HMODULE module)
{
    constexpr size_t count = sizeof(g_kh1_cn_steam_patches_in_dll) / sizeof(g_kh1_cn_steam_patches_in_dll[0]);
    if (apply_patch(module, g_kh1_cn_steam_patches_in_dll, count, "kh1_cn_steam") == 0) {
        return 0;
    }

    constexpr auto kh1_cn_steam_patch_29_rva = g_kh1_cn_steam_patch_29.rva;
    constexpr uint8_t kh1_cn_steam_orig_29_in_dll[11] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    uint8_t kh1_cn_steam_patch_29_in_dll[11];
    
    kh1_cn_steam_patch_29_in_dll[0] = 0x48; // mov rbx, ...
    kh1_cn_steam_patch_29_in_dll[1] = 0xbb; 
    *(uintptr_t*)(kh1_cn_steam_patch_29_in_dll + 2) = (uintptr_t)g_sys_font_tbl;
    kh1_cn_steam_patch_29_in_dll[10] = 0xc3; // ret

    PatchEntry g_kh1_cn_steam_patch_29_in_dll = { 
        kh1_cn_steam_patch_29_rva, 
        kh1_cn_steam_orig_29_in_dll,
        kh1_cn_steam_patch_29_in_dll,
        sizeof(kh1_cn_steam_patch_29_in_dll),
        ".text(padding)" 
    };

    return apply_patch(module, &g_kh1_cn_steam_patch_29_in_dll, 1, "kh1_cn_steam") == 0;

    /*
    constexpr auto kh1_cn_steam_patch_21_rva = g_kh1_cn_steam_patch_21.rva;
    uint8_t kh1_cn_steam_patch_21_in_dll[8] = { 0 };
    kh1_cn_steam_patch_21_in_dll[0] = 0x00; //保持原来的字节

    kh1_cn_steam_patch_21_in_dll[1] = 0x48; //lea rbx, [rip + ...]
    kh1_cn_steam_patch_21_in_dll[2] = 0x8d; 
    kh1_cn_steam_patch_21_in_dll[3] = 0x1d; 

    uintptr_t patch_21_addr = (uintptr_t)module + kh1_cn_steam_patch_21_rva;
    uintptr_t next_instruction_addr = patch_21_addr + 8;
    
    int64_t real_offset = (int64_t)((uintptr_t)g_sys_font_tbl - next_instruction_addr);

    if (real_offset < INT32_MIN || real_offset > INT32_MAX)
    {
        printf("[kh1_cn_steam] RIP relative offset overflow! Distance: %lld bytes. Maximum allowed is +-2GB.\n", real_offset);
        return 0; 
    }

    int32_t relative_offset = (int32_t)real_offset;
    *(int32_t*)(kh1_cn_steam_patch_21_in_dll + 4) = relative_offset;
    PatchEntry g_kh1_cn_steam_patch_21_in_dll = { kh1_cn_steam_patch_21_rva, kh1_cn_steam_orig_21, kh1_cn_steam_patch_21, sizeof(kh1_cn_steam_patch_29), ".text" };
    return apply_patch(module, &g_kh1_cn_steam_patch_21_in_dll, 1, "kh1_cn_steam");
    */
}

int khlauncher_cn_steam_Apply(HMODULE module)
{
    constexpr size_t count = sizeof(g_khlauncher_cn_steam_patches) / sizeof(g_khlauncher_cn_steam_patches[0]);
    return apply_patch(module, g_khlauncher_cn_steam_patches, count, "khlauncher_cn_steam");
}

int khtheater_cn_steam_Apply(HMODULE module)
{
    constexpr size_t count = sizeof(g_khtheater_cn_steam_patches) / sizeof(g_khtheater_cn_steam_patches[0]);
    return apply_patch(module, g_khtheater_cn_steam_patches, count, "khtheater_cn_steam");
}

}