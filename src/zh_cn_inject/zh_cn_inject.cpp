#include "khlauncher_cn_steam_patch.hpp"
#include "kh1_cn_steam_patch.hpp"
#include "khtheater_cn_steam_patch.hpp"
#include "khlauncher_cn_epic_crack_patch.hpp"
#include "kh1_cn_epic_crack_patch.hpp"
#include "khtheater_cn_epic_crack_patch.hpp"
#include <windows.h>
#include <stdio.h>
#include <vector>
#include <array>
#include <optional>
#include <Shlwapi.h>
#include "../OpenKH.h"
#include "kh1_text.hpp"

namespace Shiro {

bool calc_module_sha512(HMODULE module, std::array<std::byte, 64>& outHash) noexcept {
    wchar_t exePath[MAX_PATH];
    if (GetModuleFileNameW(module, exePath, MAX_PATH) == 0) {
        return false;
    }

    HANDLE hFile = CreateFileW(exePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) return false;

    HCRYPTPROV hProv = 0;
    HCRYPTHASH hHash = 0;
    bool success = false;

    if (CryptAcquireContextW(&hProv, NULL, NULL, PROV_RSA_AES, CRYPT_VERIFYCONTEXT)) {
        if (CryptCreateHash(hProv, CALG_SHA_512, 0, 0, &hHash)) {
            std::vector<BYTE> buffer(65536);
            DWORD bytesRead = 0;

            success = true;
            while (ReadFile(hFile, buffer.data(), static_cast<DWORD>(buffer.size()), &bytesRead, NULL) && bytesRead > 0) {
                if (!CryptHashData(hHash, buffer.data(), bytesRead, 0)) {
                    success = false;
                    break;
                }
            }

            if (success) {
                DWORD hashLen = static_cast<DWORD>(outHash.size());
                if (!CryptGetHashParam(hHash, HP_HASHVAL, (BYTE*)outHash.data(), &hashLen, 0)) {
                    success = false;
                }
            }
            CryptDestroyHash(hHash);
        }
        CryptReleaseContext(hProv, 0);
    }

    CloseHandle(hFile);
    return success;
}

static bool apply_patch(HMODULE module, const PatchEntry* entries, size_t entry_count, const char* patch_name) noexcept {
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

static bool kh1_text_apply(HMODULE module, const KH1S_StringPatch* patches) noexcept {
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

static uint8_t sys_font_tbl[0x10000] = { 0 };

static bool apply_kh1_sys_font_tbl(HMODULE module, uintptr_t rva, const char* sect, const char* name) noexcept {
    constexpr uint8_t orig[11] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    uint8_t patch[11];
    
    patch[0] = 0x48; // mov rbx, ...
    patch[1] = 0xbb; 
    *(uintptr_t*)(patch + 2) = (uintptr_t)sys_font_tbl;
    patch[10] = 0xc3; // ret

    const PatchEntry entry = { 
        rva, 
        orig,
        patch,
        sizeof(patch),
        sect,
    };

    return apply_patch(module, &entry, 1, name);
} 

static bool khlauncher_cn_steam_Apply(HMODULE module) noexcept {
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

static bool kh1_cn_steam_Apply(HMODULE module) noexcept {
    constexpr size_t count = sizeof(kh1_cn_steam_patches_in_dll) / sizeof(kh1_cn_steam_patches_in_dll[0]);
    if (!apply_patch(module, kh1_cn_steam_patches_in_dll, count, "kh1_cn_steam")) {
        return false;
    }

    return apply_kh1_sys_font_tbl(module, kh1_cn_steam_entry_29.rva, kh1_cn_steam_entry_29.sect, "kh1_cn_steam");
}

static bool khtheater_cn_steam_Apply(HMODULE module) noexcept {
    constexpr size_t count = sizeof(khtheater_cn_steam_patches) / sizeof(khtheater_cn_steam_patches[0]);
    return apply_patch(module, khtheater_cn_steam_patches, count, "khtheater_cn_steam");
}

static bool khlauncher_cn_epic_crack_Apply(HMODULE module) noexcept {
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

static bool kh1_cn_epic_crack_Apply(HMODULE module) noexcept {
    constexpr size_t count = sizeof(kh1_cn_epic_crack_patches_in_dll) / sizeof(kh1_cn_epic_crack_patches_in_dll[0]);
    if (!apply_patch(module, kh1_cn_epic_crack_patches_in_dll, count, "kh1_cn_epic_crack")) {
        return false;
    }

    return apply_kh1_sys_font_tbl(module, kh1_cn_epic_crack_entry_29.rva, kh1_cn_epic_crack_entry_29.sect, "kh1_cn_epic_crack");
}

static bool khtheater_cn_epic_crack_Apply(HMODULE module) noexcept {
    constexpr size_t count = sizeof(khtheater_cn_epic_crack_patches) / sizeof(khtheater_cn_epic_crack_patches[0]);
    return apply_patch(module, khtheater_cn_epic_crack_patches, count, "khtheater_cn_epic_crack");
}

enum class ExeVersion {
    Steam,
    EpicCrack,
};

#define SHIRO_DETECT_VERSION_IMPL(game)                                                             \
static std::optional<ExeVersion> game ## _detect_version(HMODULE module) noexcept {                 \
    std::array<std::byte, 64> currentHash{};                                                        \
    if (!calc_module_sha512(module, currentHash)) {                                                 \
        return std::nullopt;                                                                        \
    }                                                                                               \
    if (memcmp(game ## _cn_steam_sha512, currentHash.data(), currentHash.size()) == 0) {            \
        return ExeVersion::Steam;                                                                   \
    }                                                                                               \
    if (memcmp(game ## _cn_epic_crack_sha512, currentHash.data(), currentHash.size()) == 0) {       \
        return ExeVersion::EpicCrack;                                                               \
    }                                                                                               \
    return std::nullopt;                                                                            \
}

#define SHIRO_DETECT_VERSION(game, version, module)                                                  \
const auto ______ ## version ## _opt = game ## _detect_version(module);                             \
if (!______ ## version ## _opt.has_value()) {                                                       \
    return false;                                                                                   \
}                                                                                                   \
const auto version = ______ ## version ## _opt.value();                                             \

SHIRO_DETECT_VERSION_IMPL(kh1);
SHIRO_DETECT_VERSION_IMPL(khlauncher);
SHIRO_DETECT_VERSION_IMPL(khtheater);

static bool check_shiro_info(std::wstring_view mod_path) {
    std::wstring shiro_info = std::wstring(mod_path).append(L"\\shiro.info");
    return PathFileExistsW(shiro_info.c_str());
}

bool kh1_cn_Apply(HMODULE module, std::wstring_view mod_path) noexcept {
    if (!check_shiro_info(mod_path)) {
        return true;
    }
    SHIRO_DETECT_VERSION(kh1, version, module);

    const KH1S_StringPatch* kh1s;
    switch(version) {
    case ExeVersion::Steam: kh1s = KH1S_PATCH_TABLE_steam; break;
    case ExeVersion::EpicCrack: kh1s = KH1S_PATCH_TABLE_epic_crack; break;
    default: std::unreachable();
    }

    bool result;
    switch(version) {
    case ExeVersion::Steam: result = kh1_cn_steam_Apply(module); break;
    case ExeVersion::EpicCrack: result = kh1_cn_epic_crack_Apply(module); break;
    default: std::unreachable();
    }

    if (!result) {
        return false;
    }

    return kh1_text_apply(module, kh1s);
}

bool khlauncher_cn_Apply(HMODULE module, std::wstring_view mod_path) noexcept {
    if (!check_shiro_info(mod_path)) {
        return true;
    }
    SHIRO_DETECT_VERSION(khlauncher, version, module);

    switch(version) {
    case ExeVersion::Steam: return khlauncher_cn_steam_Apply(module);
    case ExeVersion::EpicCrack: return khlauncher_cn_epic_crack_Apply(module);
    default: std::unreachable();
    }
}

bool khtheater_cn_Apply(HMODULE module, std::wstring_view mod_path) noexcept {
    if (!check_shiro_info(mod_path)) {
        return true;
    }
    SHIRO_DETECT_VERSION(khtheater, version, module);

    switch(version) {
    case ExeVersion::Steam: return khtheater_cn_steam_Apply(module);
    case ExeVersion::EpicCrack: return khtheater_cn_epic_crack_Apply(module);
    default: std::unreachable();
    }
}
}