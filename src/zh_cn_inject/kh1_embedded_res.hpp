#include "kh1_cn_steam_patch.hpp"
#include "kh1_cn_epic_crack_patch.hpp"
#include <windows.h>

struct DataRef {
    uintptr_t leaRva;
    uint8_t   regModRM;
    const uint8_t origLea[7];
};

static constexpr size_t STUB_SIZE = 16;
static constexpr uintptr_t KH1_FUCKING_EMBEDDED_RES_START_STEAM = kh1_cn_steam_entry_29.rva + 16;
static constexpr uintptr_t KH1_FUCKING_EMBEDDED_RES_START_EPIC_CRACK = kh1_cn_epic_crack_entry_29.rva + 16;

static const DataRef kh1_fucking_embedded_item_shop_message_refs_steam[] = {
    {0x1CA6C9, 0x15, {0x48, 0x8D, 0x15, 0x00, 0x66, 0x33, 0x00}},
    {0x1CA72D, 0x3D, {0x48, 0x8D, 0x3D, 0x9C, 0x65, 0x33, 0x00}},
    {0x1CA75C, 0x0D, {0x48, 0x8D, 0x0D, 0x6D, 0x65, 0x33, 0x00}},
    {0x1CA826, 0x0D, {0x48, 0x8D, 0x0D, 0xA3, 0x64, 0x33, 0x00}},
};

static const DataRef kh1_fucking_embedded_item_shop_message_refs_steam2[] = {
    {0x1CA755, 0x15, {0x48, 0x8D, 0x15, 0x78, 0x65, 0x33, 0x00}},
};

static const DataRef kh1_fucking_embedded_gumi_shop_message_refs_steam[] = {
    {0x1CA6ED, 0x15, {0x48, 0x8D, 0x15, 0xDC, 0x9D, 0x33, 0x00}},
    {0x1CA783, 0x0D, {0x48, 0x8D, 0x0D, 0x46, 0x9D, 0x33, 0x00}},
    {0x1CA83D, 0x0D, {0x48, 0x8D, 0x0D, 0x8C, 0x9C, 0x33, 0x00}},
};

static const DataRef kh1_fucking_embedded_gumi_shop_message_refs_steam2[] = {
    {0x1CA79E, 0x15, {0x48, 0x8D, 0x15, 0x2f, 0x9D, 0x33, 0x00}},
};

static const DataRef kh1_fucking_embedded_phil_cup_refs_steam[] = {
    {0x1C5AD3, 0x15, {0x48, 0x8D, 0x15, 0xF6, 0x99, 0x33, 0x00}},
    {0x1C5B75, 0x35, {0x48, 0x8D, 0x35, 0x54, 0x99, 0x33, 0x00}},
    {0x1C5B9B, 0x0D, {0x48, 0x8D, 0x0D, 0x2E, 0x99, 0x33, 0x00}},
    {0x1C5E87, 0x0D, {0x48, 0x8D, 0x0D, 0x42, 0x96, 0x33, 0x00}},
};

static const DataRef kh1_fucking_embedded_phil_cup_refs_steam2[] = {
    {0x1C5B94, 0x15, {0x48, 0x8D, 0x15, 0x39, 0x99, 0x33, 0x00}},
};

static const DataRef kh1_fucking_embedded_pegasus_cup_refs_steam[] = {
    {0x1C5AF7, 0x15, {0x48, 0x8D, 0x15, 0xD2, 0x9D, 0x33, 0x00}},
    {0x1C5BC4, 0x35, {0x4C, 0x8D, 0x35, 0x05, 0x9D, 0x33, 0x00}},
    {0x1C5BEA, 0x0D, {0x48, 0x8D, 0x0D, 0xDF, 0x9C, 0x33, 0x00}},
    {0x1C5E9F, 0x0D, {0x48, 0x8D, 0x0D, 0x2A, 0x9A, 0x33, 0x00}},
};

static const DataRef kh1_fucking_embedded_pegasus_cup_refs_steam2[] = {
    {0x1C5BE3, 0x15, {0x48, 0x8D, 0x15, 0xEA, 0x9C, 0x33, 0x00}},
};

static const DataRef kh1_fucking_embedded_hercules_cup_refs_steam[] = {
    {0x1C5B1B, 0x15, {0x48, 0x8D, 0x15, 0xAE, 0xA1, 0x33, 0x00}},
    {0x1C5C0F, 0x3D, {0x4C, 0x8D, 0x3D, 0xBA, 0xA0, 0x33, 0x00}},
    {0x1C5C35, 0x0D, {0x48, 0x8D, 0x0D, 0x94, 0xA0, 0x33, 0x00}},
    {0x1C5EB7, 0x0D, {0x48, 0x8D, 0x0D, 0x12, 0x9E, 0x33, 0x00}},
};

static const DataRef kh1_fucking_embedded_hercules_cup_refs_steam2[] = {
    {0x1C5C2E, 0x15, {0x48, 0x8D, 0x15, 0x9F, 0xA0, 0x33, 0x00}},
};

static const DataRef kh1_fucking_embedded_hades_cup_refs_steam[] = {
    {0x1C5B3F, 0x15, {0x48, 0x8D, 0x15, 0x8A, 0xA5, 0x33, 0x00}},
    {0x1C5C5F, 0x2D, {0x4C, 0x8D, 0x2D, 0x6A, 0xA4, 0x33, 0x00}},
    {0x1C5C90, 0x0D, {0x48, 0x8D, 0x0D, 0x39, 0xA4, 0x33, 0x00}},
    {0x1C5ECF, 0x0D, {0x48, 0x8D, 0x0D, 0xFA, 0xA1, 0x33, 0x00}},
};

static const DataRef kh1_fucking_embedded_hades_cup_refs_steam2[] = {
    {0x1C5C89, 0x15, {0x48, 0x8D, 0x15, 0x44, 0xA4, 0x33, 0x00}},
};

static constexpr uintptr_t KH1_FUCKING_EMBEDDED_ITEM_SHOP_MESSAGE_STUB_RVA_STEAM   = KH1_FUCKING_EMBEDDED_RES_START_STEAM;
static constexpr uintptr_t KH1_FUCKING_EMBEDDED_ITEM_SHOP_MESSAGE_STUB_RVA_STEAM2  = KH1_FUCKING_EMBEDDED_ITEM_SHOP_MESSAGE_STUB_RVA_STEAM + STUB_SIZE * _countof(kh1_fucking_embedded_item_shop_message_refs_steam);
static constexpr uintptr_t KH1_FUCKING_EMBEDDED_GUMI_SHOP_MESSAGE_STUB_RVA_STEAM   = KH1_FUCKING_EMBEDDED_ITEM_SHOP_MESSAGE_STUB_RVA_STEAM2 + STUB_SIZE * _countof(kh1_fucking_embedded_item_shop_message_refs_steam2);
static constexpr uintptr_t KH1_FUCKING_EMBEDDED_GUMI_SHOP_MESSAGE_STUB_RVA_STEAM2  = KH1_FUCKING_EMBEDDED_GUMI_SHOP_MESSAGE_STUB_RVA_STEAM + STUB_SIZE * _countof(kh1_fucking_embedded_gumi_shop_message_refs_steam);
static constexpr uintptr_t KH1_FUCKING_EMBEDDED_PHIL_CUP_STUB_RVA_STEAM            = KH1_FUCKING_EMBEDDED_GUMI_SHOP_MESSAGE_STUB_RVA_STEAM2 + STUB_SIZE * _countof(kh1_fucking_embedded_gumi_shop_message_refs_steam2);
static constexpr uintptr_t KH1_FUCKING_EMBEDDED_PHIL_CUP_STUB_RVA_STEAM2           = KH1_FUCKING_EMBEDDED_PHIL_CUP_STUB_RVA_STEAM + STUB_SIZE * _countof(kh1_fucking_embedded_phil_cup_refs_steam);
static constexpr uintptr_t KH1_FUCKING_EMBEDDED_PEGASUS_CUP_STUB_RVA_STEAM         = KH1_FUCKING_EMBEDDED_PHIL_CUP_STUB_RVA_STEAM2 + STUB_SIZE * _countof(kh1_fucking_embedded_phil_cup_refs_steam2);
static constexpr uintptr_t KH1_FUCKING_EMBEDDED_PEGASUS_CUP_STUB_RVA_STEAM2        = KH1_FUCKING_EMBEDDED_PEGASUS_CUP_STUB_RVA_STEAM + STUB_SIZE * _countof(kh1_fucking_embedded_pegasus_cup_refs_steam);
static constexpr uintptr_t KH1_FUCKING_EMBEDDED_HERCULES_CUP_STUB_RVA_STEAM        = KH1_FUCKING_EMBEDDED_PEGASUS_CUP_STUB_RVA_STEAM2 + STUB_SIZE * _countof(kh1_fucking_embedded_pegasus_cup_refs_steam2);
static constexpr uintptr_t KH1_FUCKING_EMBEDDED_HERCULES_CUP_STUB_RVA_STEAM2       = KH1_FUCKING_EMBEDDED_HERCULES_CUP_STUB_RVA_STEAM + STUB_SIZE * _countof(kh1_fucking_embedded_hercules_cup_refs_steam);
static constexpr uintptr_t KH1_FUCKING_EMBEDDED_HADES_CUP_STUB_RVA_STEAM           = KH1_FUCKING_EMBEDDED_HERCULES_CUP_STUB_RVA_STEAM2 + STUB_SIZE * _countof(kh1_fucking_embedded_hercules_cup_refs_steam2);
static constexpr uintptr_t KH1_FUCKING_EMBEDDED_HADES_CUP_STUB_RVA_STEAM2          = KH1_FUCKING_EMBEDDED_HADES_CUP_STUB_RVA_STEAM + STUB_SIZE * _countof(kh1_fucking_embedded_hades_cup_refs_steam);

static const DataRef kh1_fucking_embedded_item_shop_message_refs_epic_crack[] = {
    {0x1C36F9, 0x15, {0x48, 0x8D, 0x15, 0x60, 0xA2, 0x33, 0x00}},
    {0x1C375D, 0x3D, {0x48, 0x8D, 0x3D, 0xFC, 0xA1, 0x33, 0x00}},
    {0x1C378C, 0x0D, {0x48, 0x8D, 0x0D, 0xCD, 0xA1, 0x33, 0x00}},
    {0x1C3856, 0x0D, {0x48, 0x8D, 0x0D, 0x03, 0xA1, 0x33, 0x00}}
};

static const DataRef kh1_fucking_embedded_item_shop_message_refs_epic_crack2[] = {
    {0x1C3785, 0x15, {0x48, 0x8D, 0x15, 0xD8, 0xA1, 0x33, 0x00}}
};

static const DataRef kh1_fucking_embedded_gumi_shop_message_refs_epic_crack[] = {
    {0x1C371D, 0x15, {0x48, 0x8D, 0x15, 0x3C, 0xDA, 0x33, 0x00}},
    {0x1C37B3, 0x0D, {0x48, 0x8D, 0x0D, 0xA6, 0xD9, 0x33, 0x00}},
    {0x1C386D, 0x0D, {0x48, 0x8D, 0x0D, 0xEC, 0xD8, 0x33, 0x00}}
};

static const DataRef kh1_fucking_embedded_gumi_shop_message_refs_epic_crack2[] = {
    {0x1C37CE, 0x15, {0x48, 0x8D, 0x15, 0x8F, 0xD9, 0x33, 0x00}}
};

static const DataRef kh1_fucking_embedded_phil_cup_refs_epic_crack[] = {
    {0x1C11B3, 0x15, {0x48, 0x8D, 0x15, 0xA6, 0xAF, 0x33, 0x00}},
    {0x1C1255, 0x35, {0x48, 0x8D, 0x35, 0x04, 0xAF, 0x33, 0x00}},
    {0x1C127B, 0x0D, {0x48, 0x8D, 0x0D, 0xDE, 0xAE, 0x33, 0x00}},
    {0x1C1567, 0x0D, {0x48, 0x8D, 0x0D, 0xF2, 0xAB, 0x33, 0x00}}
};

static const DataRef kh1_fucking_embedded_phil_cup_refs_epic_crack2[] = {
    {0x1C1274, 0x15, {0x48, 0x8D, 0x15, 0xE9, 0xAE, 0x33, 0x00}}
};

static const DataRef kh1_fucking_embedded_pegasus_cup_refs_epic_crack[] = {
    {0x1C11D7, 0x15, {0x48, 0x8D, 0x15, 0x82, 0xB3, 0x33, 0x00}},
    {0x1C12A4, 0x35, {0x4C, 0x8D, 0x35, 0xB5, 0xB2, 0x33, 0x00}},
    {0x1C12CA, 0x0D, {0x48, 0x8D, 0x0D, 0x8F, 0xB2, 0x33, 0x00}},
    {0x1C157F, 0x0D, {0x48, 0x8D, 0x0D, 0xDA, 0xAF, 0x33, 0x00}}
};

static const DataRef kh1_fucking_embedded_pegasus_cup_refs_epic_crack2[] = {
    {0x1C12C3, 0x15, {0x48, 0x8D, 0x15, 0x9A, 0xB2, 0x33, 0x00}}
};

static const DataRef kh1_fucking_embedded_hercules_cup_refs_epic_crack[] = {
    {0x1C11FB, 0x15, {0x48, 0x8D, 0x15, 0x5E, 0xB7, 0x33, 0x00}},
    {0x1C12EF, 0x3D, {0x4C, 0x8D, 0x3D, 0x6A, 0xB6, 0x33, 0x00}},
    {0x1C1315, 0x0D, {0x48, 0x8D, 0x0D, 0x44, 0xB6, 0x33, 0x00}},
    {0x1C1597, 0x0D, {0x48, 0x8D, 0x0D, 0xC2, 0xB3, 0x33, 0x00}}
};

static const DataRef kh1_fucking_embedded_hercules_cup_refs_epic_crack2[] = {
    {0x1C130E, 0x15, {0x48, 0x8D, 0x15, 0x4F, 0xB6, 0x33, 0x00}}
};

static const DataRef kh1_fucking_embedded_hades_cup_refs_epic_crack[] = {
    {0x1C121F, 0x15, {0x48, 0x8D, 0x15, 0x3A, 0xBB, 0x33, 0x00}},
    {0x1C133F, 0x2D, {0x4C, 0x8D, 0x2D, 0x1A, 0xBA, 0x33, 0x00}},
    {0x1C1370, 0x0D, {0x48, 0x8D, 0x0D, 0xE9, 0xB9, 0x33, 0x00}},
    {0x1C15AF, 0x0D, {0x48, 0x8D, 0x0D, 0xAA, 0xB7, 0x33, 0x00}}
};

static const DataRef kh1_fucking_embedded_hades_cup_refs_epic_crack2[] = {
    {0x1C1369, 0x15, {0x48, 0x8D, 0x15, 0xF4, 0xB9, 0x33, 0x00}}
};

static constexpr uintptr_t KH1_FUCKING_EMBEDDED_ITEM_SHOP_MESSAGE_STUB_RVA_EPIC_CRACK  = KH1_FUCKING_EMBEDDED_RES_START_EPIC_CRACK;
static constexpr uintptr_t KH1_FUCKING_EMBEDDED_ITEM_SHOP_MESSAGE_STUB_RVA_EPIC_CRACK2 = KH1_FUCKING_EMBEDDED_ITEM_SHOP_MESSAGE_STUB_RVA_EPIC_CRACK + STUB_SIZE * _countof(kh1_fucking_embedded_item_shop_message_refs_epic_crack);
static constexpr uintptr_t KH1_FUCKING_EMBEDDED_GUMI_SHOP_MESSAGE_STUB_RVA_EPIC_CRACK  = KH1_FUCKING_EMBEDDED_ITEM_SHOP_MESSAGE_STUB_RVA_EPIC_CRACK2 + STUB_SIZE * _countof(kh1_fucking_embedded_item_shop_message_refs_epic_crack2);
static constexpr uintptr_t KH1_FUCKING_EMBEDDED_GUMI_SHOP_MESSAGE_STUB_RVA_EPIC_CRACK2 = KH1_FUCKING_EMBEDDED_GUMI_SHOP_MESSAGE_STUB_RVA_EPIC_CRACK + STUB_SIZE * _countof(kh1_fucking_embedded_gumi_shop_message_refs_epic_crack);
static constexpr uintptr_t KH1_FUCKING_EMBEDDED_PHIL_CUP_STUB_RVA_EPIC_CRACK           = KH1_FUCKING_EMBEDDED_GUMI_SHOP_MESSAGE_STUB_RVA_EPIC_CRACK2 + STUB_SIZE * _countof(kh1_fucking_embedded_gumi_shop_message_refs_epic_crack2);
static constexpr uintptr_t KH1_FUCKING_EMBEDDED_PHIL_CUP_STUB_RVA_EPIC_CRACK2          = KH1_FUCKING_EMBEDDED_PHIL_CUP_STUB_RVA_EPIC_CRACK + STUB_SIZE * _countof(kh1_fucking_embedded_phil_cup_refs_epic_crack);
static constexpr uintptr_t KH1_FUCKING_EMBEDDED_PEGASUS_CUP_STUB_RVA_EPIC_CRACK        = KH1_FUCKING_EMBEDDED_PHIL_CUP_STUB_RVA_EPIC_CRACK2 + STUB_SIZE * _countof(kh1_fucking_embedded_phil_cup_refs_epic_crack2);
static constexpr uintptr_t KH1_FUCKING_EMBEDDED_PEGASUS_CUP_STUB_RVA_EPIC_CRACK2       = KH1_FUCKING_EMBEDDED_PEGASUS_CUP_STUB_RVA_EPIC_CRACK + STUB_SIZE * _countof(kh1_fucking_embedded_pegasus_cup_refs_epic_crack);
static constexpr uintptr_t KH1_FUCKING_EMBEDDED_HERCULES_CUP_STUB_RVA_EPIC_CRACK       = KH1_FUCKING_EMBEDDED_PEGASUS_CUP_STUB_RVA_EPIC_CRACK2 + STUB_SIZE * _countof(kh1_fucking_embedded_pegasus_cup_refs_epic_crack2);
static constexpr uintptr_t KH1_FUCKING_EMBEDDED_HERCULES_CUP_STUB_RVA_EPIC_CRACK2      = KH1_FUCKING_EMBEDDED_HERCULES_CUP_STUB_RVA_EPIC_CRACK + STUB_SIZE * _countof(kh1_fucking_embedded_hercules_cup_refs_epic_crack);
static constexpr uintptr_t KH1_FUCKING_EMBEDDED_HADES_CUP_STUB_RVA_EPIC_CRACK          = KH1_FUCKING_EMBEDDED_HERCULES_CUP_STUB_RVA_EPIC_CRACK2 + STUB_SIZE * _countof(kh1_fucking_embedded_hercules_cup_refs_epic_crack2);
static constexpr uintptr_t KH1_FUCKING_EMBEDDED_HADES_CUP_STUB_RVA_EPIC_CRACK2         = KH1_FUCKING_EMBEDDED_HADES_CUP_STUB_RVA_EPIC_CRACK + STUB_SIZE * _countof(kh1_fucking_embedded_hades_cup_refs_epic_crack);