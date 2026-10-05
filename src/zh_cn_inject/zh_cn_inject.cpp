#include "khlauncher_cn_steam_patch.hpp"
#include "kh1_cn_steam_patch.hpp"
#include "khtheater_cn_steam_patch.hpp"
#include "khlauncher_cn_epic_crack_patch.hpp"
#include "kh1_cn_epic_crack_patch.hpp"
#include "khtheater_cn_epic_crack_patch.hpp"
#include "kh1_embedded_res.hpp"
#include "kh1_text.hpp"
#include <windows.h>
#include <stdio.h>
#include <vector>
#include <array>
#include <optional>
#include <Shlwapi.h>
#include "../OpenKH.h"

namespace Shiro {

static bool calc_module_sha512(HMODULE module, std::array<std::byte, 64>& outHash) noexcept {
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
    size_t failed = 0;
    for (size_t i = 0; i < entry_count; i++) {
        const PatchEntry *p = &entries[i];
        unsigned char *addr = base + p->rva;
#ifdef SHIRO_PATCH_VALIDATE
        if (memcmp(addr, p->orig, p->len) != 0) {
            if (memcmp(addr, p->patch, p->len) == 0) {
                continue;
            }
            printf("[%s] Patch %zu verification failed at RVA 0x%llX (%s)",
                patch_name, i, (unsigned long long)p->rva, p->sect);
            failed++;
            continue;
        }
#endif
        DWORD oldProtect;
        if (!VirtualProtect(addr, p->len, PAGE_EXECUTE_READWRITE, &oldProtect)) {
            printf("[%s] VirtualProtect failed for patch %zu at RVA 0x%llX err=%lu",
                patch_name, i, (unsigned long long)p->rva, GetLastError());
            failed++;
            continue;
        }
        memcpy(addr, p->patch, p->len);
        DWORD dummy;
        VirtualProtect(addr, p->len, oldProtect, &dummy);
    }
    return failed == 0;
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

    return failures == 0;
}

static bool kh1_fucking_embedded_res(HMODULE module, void* var, const DataRef* refs, size_t refCount, uintptr_t stub_rva, const char* name) {
    uint8_t* base = (uint8_t*)module;

    for (size_t i = 0; i < refCount; i++) {
        const DataRef& r = refs[i];
        PatchEntry entries[2];

        std::array<uint8_t, STUB_SIZE> stubArray;
        std::array<uint8_t, 7> leaPatchArray;

        auto stub = stubArray.data();
        auto leaPatch = leaPatchArray.data();
        uintptr_t stubAddr = (uintptr_t)base + stub_rva + i * STUB_SIZE;
        uint8_t* leaAddr = base + r.leaRva;

        uint8_t rex = r.origLea[0];                       // 0x48 / 0x4C / ...
        uint8_t reg = ((r.regModRM >> 3) & 7) | ((rex & 0x04) ? 8 : 0);

        stub[0] = (reg >= 8) ? 0x49 : 0x48;               // REX.W (+REX.R if reg>=8)
        stub[1] = 0xB8 + (reg & 7);                       // movabs opcode
        *(void**)(stub + 2) = var;
        
        stub[10] = 0xE9; 
        *(int32_t*)(stub + 11) = (int32_t)(ptrdiff_t)((leaAddr + 7) - (stubAddr + 15));

        leaPatch[0] = 0xE9;
        *(int32_t*)(leaPatch + 1) = (int32_t)(stubAddr - (uintptr_t)(leaAddr + 5));
        leaPatch[5] = 0x90;
        leaPatch[6] = 0x90;

#ifdef SHIRO_PATCH_VALIDATE
        const uint8_t zeros[STUB_SIZE] = {0};

        entries[0] = {stub_rva + i * STUB_SIZE, zeros, stub, STUB_SIZE, ".text(padding)"};
        entries[1] = {r.leaRva, r.origLea, leaPatch, 7, ".text"};
#else
        entries[0] = {stub_rva + i * STUB_SIZE, stub, STUB_SIZE, ".text(padding)"};
        entries[1] = {r.leaRva, leaPatch, 7, ".text"};
#endif

        if (!apply_patch(module, entries, _countof(entries), name)) {
            return false;
        }
    }

    return true;
}

static BOOL read_mod_file(
    const std::wstring_view mod_path, 
    const wchar_t* file,
    void* pBuffer, 
    DWORD bufferSize
) {
    if (!pBuffer || bufferSize == 0) {
        return FALSE;
    }

    std::wstring full_path = std::wstring(mod_path).append(L"\\").append(file);

    HANDLE hFile = ::CreateFileW(
        full_path.c_str(),
        GENERIC_READ,
        FILE_SHARE_READ,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr
    );

    if (hFile == INVALID_HANDLE_VALUE) {
        return FALSE;
    }

    DWORD fileSize = ::GetFileSize(hFile, nullptr);
    if (fileSize == INVALID_FILE_SIZE || fileSize > bufferSize) {
        ::CloseHandle(hFile);
        return FALSE;
    }

    DWORD totalBytesRead = 0;
    BYTE* pCurrentBuffer = static_cast<BYTE*>(pBuffer);
    BOOL bSuccess = TRUE;

    while (totalBytesRead < fileSize) {
        DWORD bytesToRead = fileSize - totalBytesRead;
        DWORD bytesReadThisTime = 0;

        bSuccess = ::ReadFile(
            hFile, 
            pCurrentBuffer + totalBytesRead, 
            bytesToRead, 
            &bytesReadThisTime, 
            nullptr
        );

        if (!bSuccess || bytesReadThisTime == 0) {
            bSuccess = FALSE;
            break;
        }

        totalBytesRead += bytesReadThisTime;
    }

    ::CloseHandle(hFile);

    return (bSuccess && (totalBytesRead == fileSize));
}

static uint8_t sys_font_tbl[0x10000] = { 0 };

static bool apply_kh1_sys_font_tbl(HMODULE module, uintptr_t rva, const char* sect, const char* name) noexcept {
#ifdef SHIRO_PATCH_VALIDATE
    constexpr uint8_t orig[11] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
#endif

    uint8_t patch[11];
    
    patch[0] = 0x48; // mov rbx, ...
    patch[1] = 0xbb; 
    *(uintptr_t*)(patch + 2) = (uintptr_t)sys_font_tbl;
    patch[10] = 0xc3; // ret

    const PatchEntry entry = { 
        rva, 
#ifdef SHIRO_PATCH_VALIDATE
        orig,
#endif
        patch,
        sizeof(patch),
        sect,
    };

    return apply_patch(module, &entry, 1, name);
} 

static uint8_t kh1_fucked_embedded_item_shop_message[0x2000] = { 0 }; // size = 8192
static uint8_t kh1_fucked_embedded_wsysmsg_data[0x80] = { 0 };       // size = 128
static uint8_t kh1_fucked_embedded_wsysmsg_offset[0x80] = { 0 };     // size = 128
static uint8_t kh1_fucked_embedded_phil_cup[0x1000] = { 0 };          // size = 4096
static uint8_t kh1_fucked_embedded_pegasus_cup[0x1000] = { 0 };       // size = 4096
static uint8_t kh1_fucked_embedded_hercules_cup[0x1000] = { 0 };      // size = 4096
static uint8_t kh1_fucked_embedded_hades_cup[0x1000] = { 0 };         // size = 4096

static bool kh1_read_embedded_files(std::wstring_view mod_path) {
    if (!read_mod_file(mod_path, L"item_shop_message.bin", kh1_fucked_embedded_item_shop_message, sizeof(kh1_fucked_embedded_item_shop_message))) {
        return false;
    }
    if (!read_mod_file(mod_path, L"exchange\\FM_wsysmsg_data.bin", kh1_fucked_embedded_wsysmsg_data, sizeof(kh1_fucked_embedded_wsysmsg_data))) {
        return false;
    }
    if (!read_mod_file(mod_path, L"exchange\\FM_wsysmsg_offset.bin", kh1_fucked_embedded_wsysmsg_offset, sizeof(kh1_fucked_embedded_wsysmsg_offset))) {
        return false;
    }
    if (!read_mod_file(mod_path, L"exchange\\FM_phil_cup.bin", kh1_fucked_embedded_phil_cup, sizeof(kh1_fucked_embedded_phil_cup))) {
        return false;
    }
    if (!read_mod_file(mod_path, L"exchange\\FM_pegasus_cup.bin", kh1_fucked_embedded_pegasus_cup, sizeof(kh1_fucked_embedded_pegasus_cup))) {
        return false;
    }
    if (!read_mod_file(mod_path, L"exchange\\FM_hercules_cup.bin", kh1_fucked_embedded_hercules_cup, sizeof(kh1_fucked_embedded_hercules_cup))) {
        return false;
    }
    if (!read_mod_file(mod_path, L"exchange\\FM_hades_cup.bin", kh1_fucked_embedded_hades_cup, sizeof(kh1_fucked_embedded_hades_cup))) {
        return false;
    }
    return true;
}

static bool kh1_fucking_wsysmsg(HMODULE module, uintptr_t data_rva, const uint8_t* data, uintptr_t offset_rva, const uint8_t* offset, const char* name) noexcept {
    constexpr size_t PATCH_LEN = 0x80;
    
    unsigned char *base = (unsigned char *)module;
    unsigned char *data_addr = base + data_rva;
    unsigned char *offset_addr = base + offset_rva;
    DWORD oldProtect, dummy;
    
    if (!VirtualProtect(data_addr, PATCH_LEN, PAGE_EXECUTE_READWRITE, &oldProtect)) {
        printf("[%s] VirtualProtect failed for data_rva 0x%llX err=%lu\n",
            name, (unsigned long long)data_rva, GetLastError());
        return false;
    }
    memcpy(data_addr, data, PATCH_LEN);
    VirtualProtect(data_addr, PATCH_LEN, oldProtect, &dummy);

    if (!VirtualProtect(offset_addr, PATCH_LEN, PAGE_EXECUTE_READWRITE, &oldProtect)) {
        printf("[%s] VirtualProtect failed for offset_rva 0x%llX err=%lu\n",
            name, (unsigned long long)offset_rva, GetLastError());
        return false;
    }
    memcpy(offset_addr, offset, PATCH_LEN);
    VirtualProtect(offset_addr, PATCH_LEN, oldProtect, &dummy);

    return true;
}

struct KH1ResourceConfig {
    void* var_ptr;
    const DataRef* refs;
    size_t ref_count;
    uintptr_t stub_rva;
};

static constexpr KH1ResourceConfig kh1_steam_embedded_files[] = {
    { &kh1_fucked_embedded_item_shop_message[0], kh1_fucking_embedded_item_shop_message_refs_steam,  _countof(kh1_fucking_embedded_item_shop_message_refs_steam),  KH1_FUCKING_EMBEDDED_ITEM_SHOP_MESSAGE_STUB_RVA_STEAM },
    { &kh1_fucked_embedded_item_shop_message[4], kh1_fucking_embedded_item_shop_message_refs_steam2, _countof(kh1_fucking_embedded_item_shop_message_refs_steam2), KH1_FUCKING_EMBEDDED_ITEM_SHOP_MESSAGE_STUB_RVA_STEAM2 },
    { &kh1_fucked_embedded_phil_cup[0],          kh1_fucking_embedded_phil_cup_refs_steam,           _countof(kh1_fucking_embedded_phil_cup_refs_steam),           KH1_FUCKING_EMBEDDED_PHIL_CUP_STUB_RVA_STEAM },
    { &kh1_fucked_embedded_phil_cup[4],          kh1_fucking_embedded_phil_cup_refs_steam2,          _countof(kh1_fucking_embedded_phil_cup_refs_steam2),          KH1_FUCKING_EMBEDDED_PHIL_CUP_STUB_RVA_STEAM2 },
    { &kh1_fucked_embedded_pegasus_cup[0],       kh1_fucking_embedded_pegasus_cup_refs_steam,        _countof(kh1_fucking_embedded_pegasus_cup_refs_steam),        KH1_FUCKING_EMBEDDED_PEGASUS_CUP_STUB_RVA_STEAM },
    { &kh1_fucked_embedded_pegasus_cup[4],       kh1_fucking_embedded_pegasus_cup_refs_steam2,       _countof(kh1_fucking_embedded_pegasus_cup_refs_steam2),       KH1_FUCKING_EMBEDDED_PEGASUS_CUP_STUB_RVA_STEAM2 },
    { &kh1_fucked_embedded_hercules_cup[0],      kh1_fucking_embedded_hercules_cup_refs_steam,       _countof(kh1_fucking_embedded_hercules_cup_refs_steam),       KH1_FUCKING_EMBEDDED_HERCULES_CUP_STUB_RVA_STEAM },
    { &kh1_fucked_embedded_hercules_cup[4],      kh1_fucking_embedded_hercules_cup_refs_steam2,      _countof(kh1_fucking_embedded_hercules_cup_refs_steam2),      KH1_FUCKING_EMBEDDED_HERCULES_CUP_STUB_RVA_STEAM2 },
    { &kh1_fucked_embedded_hades_cup[0],         kh1_fucking_embedded_hades_cup_refs_steam,          _countof(kh1_fucking_embedded_hades_cup_refs_steam),          KH1_FUCKING_EMBEDDED_HADES_CUP_STUB_RVA_STEAM },
    { &kh1_fucked_embedded_hades_cup[4],         kh1_fucking_embedded_hades_cup_refs_steam2,         _countof(kh1_fucking_embedded_hades_cup_refs_steam2),         KH1_FUCKING_EMBEDDED_HADES_CUP_STUB_RVA_STEAM2 },
};

static constexpr KH1ResourceConfig kh1_epic_crack_embedded_files[] = {
    { &kh1_fucked_embedded_item_shop_message[0], kh1_fucking_embedded_item_shop_message_refs_epic_crack,  _countof(kh1_fucking_embedded_item_shop_message_refs_epic_crack),  KH1_FUCKING_EMBEDDED_ITEM_SHOP_MESSAGE_STUB_RVA_EPIC_CRACK },
    { &kh1_fucked_embedded_item_shop_message[4], kh1_fucking_embedded_item_shop_message_refs_epic_crack2, _countof(kh1_fucking_embedded_item_shop_message_refs_epic_crack2), KH1_FUCKING_EMBEDDED_ITEM_SHOP_MESSAGE_STUB_RVA_EPIC_CRACK2 },
    { &kh1_fucked_embedded_phil_cup[0],          kh1_fucking_embedded_phil_cup_refs_epic_crack,           _countof(kh1_fucking_embedded_phil_cup_refs_epic_crack),           KH1_FUCKING_EMBEDDED_PHIL_CUP_STUB_RVA_EPIC_CRACK },
    { &kh1_fucked_embedded_phil_cup[4],          kh1_fucking_embedded_phil_cup_refs_epic_crack2,          _countof(kh1_fucking_embedded_phil_cup_refs_epic_crack2),          KH1_FUCKING_EMBEDDED_PHIL_CUP_STUB_RVA_EPIC_CRACK2 },
    { &kh1_fucked_embedded_pegasus_cup[0],       kh1_fucking_embedded_pegasus_cup_refs_epic_crack,        _countof(kh1_fucking_embedded_pegasus_cup_refs_epic_crack),        KH1_FUCKING_EMBEDDED_PEGASUS_CUP_STUB_RVA_EPIC_CRACK },
    { &kh1_fucked_embedded_pegasus_cup[4],       kh1_fucking_embedded_pegasus_cup_refs_epic_crack2,       _countof(kh1_fucking_embedded_pegasus_cup_refs_epic_crack2),       KH1_FUCKING_EMBEDDED_PEGASUS_CUP_STUB_RVA_EPIC_CRACK2 },
    { &kh1_fucked_embedded_hercules_cup[0],      kh1_fucking_embedded_hercules_cup_refs_epic_crack,       _countof(kh1_fucking_embedded_hercules_cup_refs_epic_crack),       KH1_FUCKING_EMBEDDED_HERCULES_CUP_STUB_RVA_EPIC_CRACK },
    { &kh1_fucked_embedded_hercules_cup[4],      kh1_fucking_embedded_hercules_cup_refs_epic_crack2,      _countof(kh1_fucking_embedded_hercules_cup_refs_epic_crack2),      KH1_FUCKING_EMBEDDED_HERCULES_CUP_STUB_RVA_EPIC_CRACK2 },
    { &kh1_fucked_embedded_hades_cup[0],         kh1_fucking_embedded_hades_cup_refs_epic_crack,          _countof(kh1_fucking_embedded_hades_cup_refs_epic_crack),          KH1_FUCKING_EMBEDDED_HADES_CUP_STUB_RVA_EPIC_CRACK },
    { &kh1_fucked_embedded_hades_cup[4],         kh1_fucking_embedded_hades_cup_refs_epic_crack2,         _countof(kh1_fucking_embedded_hades_cup_refs_epic_crack2),         KH1_FUCKING_EMBEDDED_HADES_CUP_STUB_RVA_EPIC_CRACK2 },
};

static bool kh1_apply_embedded_files(HMODULE module, const KH1ResourceConfig* configs, size_t count, const char* name) noexcept {
    for (size_t i = 0;i < count;i++) {
        auto& config = configs[i];
        if (!kh1_fucking_embedded_res(module, config.var_ptr, config.refs, config.ref_count, config.stub_rva, name)) {
            return false;
        }
    }
    return true;
}

static bool khlauncher_cn_steam_Apply(HMODULE module) noexcept {
    return apply_patch(module, khlauncher_cn_steam_patches, _countof(khlauncher_cn_steam_patches), "khlauncher_cn_steam");
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
    //remove 28
    kh1_cn_steam_entry_29,
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
};

static bool kh1_cn_steam_Apply(HMODULE module, std::wstring_view mod_path) noexcept {
    if (!apply_patch(module, kh1_cn_steam_patches_in_dll, _countof(kh1_cn_steam_patches_in_dll), "kh1_cn_steam")) {
        return false;
    }
    
    if (!apply_kh1_sys_font_tbl(module, kh1_cn_steam_entry_28.rva, kh1_cn_steam_entry_28.sect, "kh1_cn_steam")) {
        return false;
    }

    if (!kh1_read_embedded_files(mod_path)) {
        return false;
    }
    
    if (!kh1_apply_embedded_files(module, kh1_steam_embedded_files, _countof(kh1_steam_embedded_files), "kh1_cn_steam")) {
        return false;
    }

    if (!kh1_fucking_wsysmsg(module, KH1_FUCKING_EMBEDDED_WSYSMSG_DATA_RVA_STEAM, kh1_fucked_embedded_wsysmsg_data, KH1_FUCKING_EMBEDDED_WSYSMSG_OFFSET_RVA_STEAM, kh1_fucked_embedded_wsysmsg_offset, "kh1_cn_steam")) {
        return false;
    }

    if (!apply_patch(module, kh1_embedded_text_steam, _countof(kh1_embedded_text_steam), "kh1_cn_steam")) {
        return false;
    }

    if (!kh1_text_apply(module, KH1S_PATCH_TABLE_steam)) {
        return false;
    }

    return true;
}

static bool khtheater_cn_steam_Apply(HMODULE module) noexcept {
    return apply_patch(module, khtheater_cn_steam_patches, _countof(khtheater_cn_steam_patches), "khtheater_cn_steam");
}

static bool khlauncher_cn_epic_crack_Apply(HMODULE module) noexcept {
    return apply_patch(module, khlauncher_cn_epic_crack_patches, _countof(khlauncher_cn_epic_crack_patches), "khlauncher_cn_epic_crack");
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
    //remove 28
    kh1_cn_epic_crack_entry_29,
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
};

static bool kh1_cn_epic_crack_Apply(HMODULE module, std::wstring_view mod_path) noexcept {
    if (!apply_patch(module, kh1_cn_epic_crack_patches_in_dll, _countof(kh1_cn_epic_crack_patches_in_dll), "kh1_cn_epic_crack")) {
        return false;
    }

    if (!apply_kh1_sys_font_tbl(module, kh1_cn_epic_crack_entry_28.rva, kh1_cn_epic_crack_entry_28.sect, "kh1_cn_epic_crack")) {
        return false;
    }

    if (!kh1_read_embedded_files(mod_path)) {
        return false;
    }
    
    if (!kh1_apply_embedded_files(module, kh1_epic_crack_embedded_files, _countof(kh1_epic_crack_embedded_files), "kh1_cn_epic_crack")) {
        return false;
    }

    if (!kh1_fucking_wsysmsg(module, KH1_FUCKING_EMBEDDED_WSYSMSG_DATA_RVA_EPIC_CRACK, kh1_fucked_embedded_wsysmsg_data, KH1_FUCKING_EMBEDDED_WSYSMSG_OFFSET_RVA_EPIC_CRACK, kh1_fucked_embedded_wsysmsg_offset, "kh1_cn_epic_crack")) {
        return false;
    }

    if (!apply_patch(module, kh1_embedded_text_epic_crack, _countof(kh1_embedded_text_epic_crack), "kh1_cn_epic_crack")) {
        return false;
    }

    if (!kh1_text_apply(module, KH1S_PATCH_TABLE_epic_crack)) {
        return false;
    }

    return true;
}

static bool khtheater_cn_epic_crack_Apply(HMODULE module) noexcept {
    return apply_patch(module, khtheater_cn_epic_crack_patches, _countof(khtheater_cn_epic_crack_patches), "khtheater_cn_epic_crack");
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

#define SHIRO_DETECT_VERSION(game, version, module)                                                 \
const auto ______ ## version ## _opt = game ## _detect_version(module);                             \
if (!______ ## version ## _opt.has_value()) {                                                       \
    return false;                                                                                   \
}                                                                                                   \
const auto version = ______ ## version ## _opt.value();

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

    switch(version) {
    case ExeVersion::Steam: return kh1_cn_steam_Apply(module, mod_path); break;
    case ExeVersion::EpicCrack: return kh1_cn_epic_crack_Apply(module, mod_path); break;
    default: std::unreachable();
    }
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