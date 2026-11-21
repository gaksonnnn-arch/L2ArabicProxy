// ============================================================================
// تم الإنشاء والتطوير بواسطة: Mohamed Tarek
// All rights and development reserved to: Mohamed Tarek
// ============================================================================
// ملف تعريب واجهات ونصوص NWindow.dll في الذاكرة: arabic_nwindow_hook.h
// ============================================================================

#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <string>
#include <mutex>
#include <fstream>
#include "config.h"
#include "arabic_reshaper.h"

// ----------------------------------------------------------------------------
// العناوين النسبية (RVAs) لإصداري P520 و Samurai Crow EU 542
// ----------------------------------------------------------------------------
static const DWORD RVA_FnPtr_GlobalSetText_P520     = 0x00C7C114UL; // FString::operator=(const FString&)
static const DWORD RVA_FnPtr_FStringAssignW_P520    = 0x00C7C1ACUL; // FString::operator=(const wchar_t*)
static const DWORD RVA_FnPtr_FStringCtorW_P520      = 0x00C7C118UL; // FString::FString(const wchar_t*)
static const DWORD RVA_FnPtr_DrawTextCanvas_P520    = 0x00C7D2F0UL; // UCanvas::DrawTextTTFToCanvas
static const DWORD RVA_FnPtr_GetTextSizeTTF_P520    = 0x00C7CC20UL; // UCanvas::GetTextSizeTTF
static const DWORD RVA_FnPtr_GetTextSizeList_P520   = 0x00C7DC70UL; // UCanvas::GetTextSizeTTFList
static const DWORD RVA_FnPtr_FNameGetEntry_P520              = 0x00C7C27CUL; // FName::GetEntry
static const DWORD RVA_FnPtr_GetMinimapRegionData_P520       = 0x00C7D7E4UL; // FL2GameData::GetMinimapRegionData
static const DWORD RVA_FnPtr_GetZoneNameInfoByID_P520        = 0x00C7DC28UL; // FL2GameData::GetZoneNameInfoByID
static const DWORD RVA_FnPtr_GetZoneName_P520                = 0x00C7D714UL; // FL2GameData::GetZoneName
static const DWORD RVA_FnPtr_GetCurrentZoneName_P520         = 0x00C7D244UL; // FL2GameData::GetCurrentZoneName
static const DWORD RVA_FnPtr_GetZoneNameFromID_P520          = 0x00C7D96CUL; // FL2GameData::GetZoneNameFromID
static const DWORD RVA_FnPtr_DrawTextFormatCanvas_P520       = 0x00C7D58CUL; // UCanvas::DrawTextTTFFormatStringToCanvas
static const DWORD RVA_FnPtr_GetTextSizeFormatCanvas_P520    = 0x00C7D588UL; // UCanvas::GetTextSizeTTFFormatString
static const DWORD RVA_FnPtr_ParserGetSize_P520              = 0x00C7D2F4UL; // FL2FormatStringParser::GetSize
static const DWORD RVA_FnPtr_ParserGetAt_P520                = 0x00C7D2F8UL; // FL2FormatStringParser::GetAt
static const DWORD RVA_Ptr_L2GameData_P520          = 0x00C7CBB0UL;
static const DWORD OFFSET_SysStringTable_P520       = 0x002EE8C0UL;
static const DWORD OFFSET_CastleTable_P520          = 0x00329294UL;
static const DWORD OFFSET_ZoneTable_P520            = 0x0032D330UL;
static const DWORD RVA_Fn_BuildZoneFlashObject_P520 = 0x00B78940UL;
static const DWORD RVA_Fn_BuildHuntingZoneFlashObject_P520 = 0x00B74A00UL;
static const DWORD RVA_FnPtr_GetHuntingZoneData_P520= 0x00C7D294UL;

static const DWORD RVA_FnPtr_GlobalSetText_EU542     = 0x00C8F114UL; // FString::operator=(const FString&)
static const DWORD RVA_FnPtr_FStringAssignW_EU542    = 0x00C8F1ACUL; // FString::operator=(const wchar_t*)
static const DWORD RVA_FnPtr_FStringCtorW_EU542      = 0x00C8F118UL; // FString::FString(const wchar_t*)
static const DWORD RVA_FnPtr_DrawTextCanvas_EU542    = 0x00C9035CUL; // UCanvas::DrawTextTTFToCanvas
static const DWORD RVA_FnPtr_GetTextSizeTTF_EU542    = 0x00C8FC54UL; // UCanvas::GetTextSizeTTF
static const DWORD RVA_FnPtr_GetTextSizeList_EU542   = 0x00C90368UL; // UCanvas::GetTextSizeTTFList
static const DWORD RVA_FnPtr_FNameGetEntry_EU542             = 0x00C8F27CUL; // FName::GetEntry
static const DWORD RVA_FnPtr_GetMinimapRegionData_EU542      = 0x00C9085CUL; // FL2GameData::GetMinimapRegionData
static const DWORD RVA_FnPtr_GetZoneNameInfoByID_EU542       = 0x00C90CA8UL; // FL2GameData::GetZoneNameInfoByID
static const DWORD RVA_FnPtr_GetZoneName_EU542               = 0x00C9078CUL; // FL2GameData::GetZoneName
static const DWORD RVA_FnPtr_GetCurrentZoneName_EU542        = 0x00C902A8UL; // FL2GameData::GetCurrentZoneName
static const DWORD RVA_FnPtr_GetZoneNameFromID_EU542         = 0x00C909E8UL; // FL2GameData::GetZoneNameFromID
static const DWORD RVA_FnPtr_DrawTextFormatCanvas_EU542      = 0x00C90600UL; // UCanvas::DrawTextTTFFormatStringToCanvas
static const DWORD RVA_FnPtr_GetTextSizeFormatCanvas_EU542   = 0x00C905FCUL; // UCanvas::GetTextSizeTTFFormatString
static const DWORD RVA_FnPtr_ParserGetSize_EU542             = 0x00C90360UL; // FL2FormatStringParser::GetSize
static const DWORD RVA_FnPtr_ParserGetAt_EU542               = 0x00C90364UL; // FL2FormatStringParser::GetAt
static const DWORD RVA_Ptr_L2GameData_EU542         = 0x00C8FBE4UL;
static const DWORD OFFSET_SysStringTable_EU542      = 0x002F18FCUL;
static const DWORD OFFSET_CastleTable_EU542         = 0x0032C2D0UL;
static const DWORD OFFSET_ZoneTable_EU542           = 0x0033036CUL;
static const DWORD RVA_Fn_BuildZoneFlashObject_EU542= 0x00B8B9D0UL;
static const DWORD RVA_Fn_BuildHuntingZoneFlashObject_EU542= 0x00B87A70UL;
static const DWORD RVA_FnPtr_GetHuntingZoneData_EU542= 0x00C902FCUL;
static const DWORD RVA_FnPtr_CreateFontW_EU542        = 0x00C90D18UL; // gdi32!CreateFontW IAT slot
static const DWORD RVA_FnPtr_CreateFontW_P520         = 0x00C7DD18UL; // gdi32!CreateFontW IAT slot (P520 placeholder)
static const DWORD RVA_FnPtr_CreateFontFace_EU542     = 0x00C90764UL; // Engine.dll!UFontManager::CreateFontFace IAT slot

static const int   MAX_SYS_STRINGS                  = 20000;

// ----------------------------------------------------------------------------
// ★ إصلاح: علم الإيقاف للـ threads
// ----------------------------------------------------------------------------
static volatile LONG g_shutdownFlag = 0;

inline void SignalShutdown() {
    InterlockedExchange(&g_shutdownFlag, 1);
}

inline bool IsShuttingDown() {
    return InterlockedCompareExchange(&g_shutdownFlag, 0, 0) != 0;
}

// ----------------------------------------------------------------------------
// بنية FString
// ----------------------------------------------------------------------------
struct FString_t {
    wchar_t* Data;
    int      Num;
    int      Max;
};

// ----------------------------------------------------------------------------
// Logging
// ----------------------------------------------------------------------------
static void HookLog(const std::string& msg) {
    if (IsShuttingDown()) return;
    wchar_t logPath[MAX_PATH] = { 0 };
    HMODULE hMod = GetModuleHandleW(L"d3d9.dll");
    if (hMod) {
        GetModuleFileNameW(hMod, logPath, MAX_PATH);
        wchar_t* p = wcsrchr(logPath, L'\\');
        if (p) *(p + 1) = L'\0';
        wcscat_s(logPath, L"l2arabic.log");
    } else {
        wcscpy_s(logPath, L"l2arabic.log");
    }
    FILE* fp = nullptr;
    if (_wfopen_s(&fp, logPath, L"a, ccs=UTF-8") == 0 && fp) {
        fwprintf(fp, L"[NWHook] %S\n", msg.c_str());
        fclose(fp);
    }
}

// ----------------------------------------------------------------------------
// فحص وجود حروف عربية خام
// ----------------------------------------------------------------------------
static bool HasRawArabic(const wchar_t* p, int len) {
    if (!p || len <= 0) return false;
    for (int i = 0; i < len; ++i) {
        unsigned c = (unsigned)p[i];
        if (c >= 0x0621u && c <= 0x064Au) return true;
    }
    return false;
}

// ----------------------------------------------------------------------------
// فحص وجود أشكال الحروف المشكلة مسبقاً (Presentation Forms)
// ----------------------------------------------------------------------------
static bool HasPresentationForms(const wchar_t* p, int len) {
    if (!p || len <= 0) return false;
    for (int i = 0; i < len; ++i) {
        unsigned c = (unsigned)p[i];
        if ((c >= 0xFB50u && c <= 0xFDFFu) || (c >= 0xFE70u && c <= 0xFEFCu))
            return true;
    }
    return false;
}

// ----------------------------------------------------------------------------
// ★ إصلاح: دالة آمنة لفحص صلاحية الذاكرة (بديل IsBadReadPtr)
// ----------------------------------------------------------------------------
inline bool IsValidReadablePtr(const void* ptr, size_t size) {
    if (!ptr || size == 0) return false;
    const BYTE* start = (const BYTE*)ptr;
    const BYTE* end = start + size - 1;

    MEMORY_BASIC_INFORMATION mbiStart;
    if (VirtualQuery(start, &mbiStart, sizeof(mbiStart)) == 0) return false;
    if (mbiStart.State != MEM_COMMIT) return false;
    if (mbiStart.Protect & (PAGE_NOACCESS | PAGE_GUARD)) return false;

    const BYTE* regionEnd =
        (const BYTE*)mbiStart.BaseAddress + mbiStart.RegionSize;
    if (end < regionEnd) return true;

    MEMORY_BASIC_INFORMATION mbiEnd;
    if (VirtualQuery(end, &mbiEnd, sizeof(mbiEnd)) == 0) return false;
    if (mbiEnd.State != MEM_COMMIT) return false;
    if (mbiEnd.Protect & (PAGE_NOACCESS | PAGE_GUARD)) return false;

    return true;
}

inline int SafeWcsLen(const wchar_t* str, int maxLen = 4096) {
    if (!str || !IsValidReadablePtr(str, sizeof(wchar_t))) return 0;
    int len = 0;
    while (len < maxLen) {
        if ((len % 64 == 0) && !IsValidReadablePtr(&str[len], sizeof(wchar_t))) break;
        if (str[len] == L'\0') return len;
        ++len;
    }
    return 0;
}

inline bool IsEssenceP520NWindow()
{
    HMODULE hNWindow = GetModuleHandleA("NWindow.dll");
    if (!hNWindow) return false;

    IMAGE_DOS_HEADER* dos = (IMAGE_DOS_HEADER*)hNWindow;
    if (!IsValidReadablePtr(dos, sizeof(*dos)) ||
        dos->e_magic != IMAGE_DOS_SIGNATURE ||
        dos->e_lfanew <= 0 ||
        dos->e_lfanew > 0x100000)
        return false;

    IMAGE_NT_HEADERS32* nt =
        (IMAGE_NT_HEADERS32*)((BYTE*)hNWindow + dos->e_lfanew);
    if (!IsValidReadablePtr(nt, sizeof(*nt)) ||
        nt->Signature != IMAGE_NT_SIGNATURE ||
        nt->FileHeader.Machine != IMAGE_FILE_MACHINE_I386 ||
        nt->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC)
        return false;

    return nt->FileHeader.TimeDateStamp == 0x68B150AD &&
        nt->OptionalHeader.SizeOfImage == 0x0180F000;
}

inline DWORD GetRVA_FnPtr_GlobalSetText() {
    return IsEssenceP520NWindow() ? RVA_FnPtr_GlobalSetText_P520 : RVA_FnPtr_GlobalSetText_EU542;
}

inline DWORD GetRVA_FnPtr_FStringAssignW() {
    return IsEssenceP520NWindow() ? RVA_FnPtr_FStringAssignW_P520 : RVA_FnPtr_FStringAssignW_EU542;
}

inline DWORD GetRVA_FnPtr_FStringCtorW() {
    return IsEssenceP520NWindow() ? RVA_FnPtr_FStringCtorW_P520 : RVA_FnPtr_FStringCtorW_EU542;
}

inline DWORD GetRVA_FnPtr_DrawTextCanvas() {
    return IsEssenceP520NWindow() ? RVA_FnPtr_DrawTextCanvas_P520 : RVA_FnPtr_DrawTextCanvas_EU542;
}

inline DWORD GetRVA_FnPtr_GetTextSizeTTF() {
    return IsEssenceP520NWindow() ? RVA_FnPtr_GetTextSizeTTF_P520 : RVA_FnPtr_GetTextSizeTTF_EU542;
}

inline DWORD GetRVA_FnPtr_GetTextSizeList() {
    return IsEssenceP520NWindow() ? RVA_FnPtr_GetTextSizeList_P520 : RVA_FnPtr_GetTextSizeList_EU542;
}

inline DWORD GetRVA_Ptr_L2GameData() {
    return IsEssenceP520NWindow() ? RVA_Ptr_L2GameData_P520 : RVA_Ptr_L2GameData_EU542;
}

inline DWORD GetOFFSET_SysStringTable() {
    return IsEssenceP520NWindow() ? OFFSET_SysStringTable_P520 : OFFSET_SysStringTable_EU542;
}

inline DWORD GetOFFSET_CastleTable() {
    return IsEssenceP520NWindow() ? OFFSET_CastleTable_P520 : OFFSET_CastleTable_EU542;
}

inline DWORD GetOFFSET_ZoneTable() {
    return IsEssenceP520NWindow() ? OFFSET_ZoneTable_P520 : OFFSET_ZoneTable_EU542;
}

inline DWORD GetRVA_Fn_BuildZoneFlashObject() {
    return IsEssenceP520NWindow() ? RVA_Fn_BuildZoneFlashObject_P520 : RVA_Fn_BuildZoneFlashObject_EU542;
}

inline DWORD GetRVA_Fn_BuildHuntingZoneFlashObject() {
    return IsEssenceP520NWindow() ? RVA_Fn_BuildHuntingZoneFlashObject_P520 : RVA_Fn_BuildHuntingZoneFlashObject_EU542;
}

inline DWORD GetRVA_FnPtr_GetHuntingZoneData() {
    return IsEssenceP520NWindow() ? RVA_FnPtr_GetHuntingZoneData_P520 : RVA_FnPtr_GetHuntingZoneData_EU542;
}

inline DWORD GetRVA_FnPtr_CreateFontW() {
    return IsEssenceP520NWindow() ? RVA_FnPtr_CreateFontW_P520 : RVA_FnPtr_CreateFontW_EU542;
}

inline DWORD GetRVA_FnPtr_CreateFontFace() {
    return IsEssenceP520NWindow() ? 0 : RVA_FnPtr_CreateFontFace_EU542;
}

inline DWORD GetRVA_FnPtr_FNameGetEntry() {
    return IsEssenceP520NWindow() ? RVA_FnPtr_FNameGetEntry_P520 : RVA_FnPtr_FNameGetEntry_EU542;
}

inline DWORD GetRVA_FnPtr_GetMinimapRegionData() {
    return IsEssenceP520NWindow() ? RVA_FnPtr_GetMinimapRegionData_P520 : RVA_FnPtr_GetMinimapRegionData_EU542;
}

inline DWORD GetRVA_FnPtr_GetZoneNameInfoByID() {
    return IsEssenceP520NWindow() ? RVA_FnPtr_GetZoneNameInfoByID_P520 : RVA_FnPtr_GetZoneNameInfoByID_EU542;
}

inline DWORD GetRVA_FnPtr_GetZoneName() {
    return IsEssenceP520NWindow() ? RVA_FnPtr_GetZoneName_P520 : RVA_FnPtr_GetZoneName_EU542;
}

inline DWORD GetRVA_FnPtr_GetCurrentZoneName() {
    return IsEssenceP520NWindow() ? RVA_FnPtr_GetCurrentZoneName_P520 : RVA_FnPtr_GetCurrentZoneName_EU542;
}

inline DWORD GetRVA_FnPtr_GetZoneNameFromID() {
    return IsEssenceP520NWindow() ? RVA_FnPtr_GetZoneNameFromID_P520 : RVA_FnPtr_GetZoneNameFromID_EU542;
}

inline DWORD GetRVA_FnPtr_DrawTextFormatCanvas() {
    return IsEssenceP520NWindow() ? RVA_FnPtr_DrawTextFormatCanvas_P520 : RVA_FnPtr_DrawTextFormatCanvas_EU542;
}

inline DWORD GetRVA_FnPtr_GetTextSizeFormatCanvas() {
    return IsEssenceP520NWindow() ? RVA_FnPtr_GetTextSizeFormatCanvas_P520 : RVA_FnPtr_GetTextSizeFormatCanvas_EU542;
}

inline DWORD GetRVA_FnPtr_ParserGetSize() {
    return IsEssenceP520NWindow() ? RVA_FnPtr_ParserGetSize_P520 : RVA_FnPtr_ParserGetSize_EU542;
}

inline DWORD GetRVA_FnPtr_ParserGetAt() {
    return IsEssenceP520NWindow() ? RVA_FnPtr_ParserGetAt_P520 : RVA_FnPtr_ParserGetAt_EU542;
}

// ----------------------------------------------------------------------------
// مؤشر مشغل إسناد FString (سيتم ربطه من IAT)
// ----------------------------------------------------------------------------
typedef void* (__thiscall* fnFStringAssignW)(void* thisPtr, const wchar_t* src);
static fnFStringAssignW g_origFStringAssignW = nullptr;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ★ إصلاح: ReshapeFStringInPlace يستخدم cache الجديد + فحص آمن
// ----------------------------------------------------------------------------
static bool ReshapeFStringInPlace(FString_t* pStr) {
    if (!pStr) return false;
    if (!IsValidReadablePtr(pStr, sizeof(FString_t))) return false;
    if (!pStr->Data || pStr->Max <= 0 || pStr->Num <= 0) return false;

    int len = pStr->Num;
    if (len > pStr->Max) len = pStr->Max;
    // التحقق الصارم من صحة مؤشر الذاكرة قبل القراءة
    if (!IsValidReadablePtr(pStr->Data, (size_t)len * sizeof(wchar_t))) return false;
    if (len > 0 && pStr->Data[len - 1] == L'\0') {
        --len;
    }
    if (len <= 0) return false;
    
    // إذا كان النص مشكلاً بالفعل، لا تعيد تشكيله أبداً
    if (HasPresentationForms(pStr->Data, len)) return false;
    if (!HasRawArabic(pStr->Data, len)) return false;

    std::wstring raw(pStr->Data, len);
    std::wstring reshaped = ArabicBiDi::ReshapeCache::Instance().Get(raw);

    if (reshaped.empty() || reshaped == raw) return false;

    // الأمان أولاً: التعديل الموضعي في المساحة المحجوزة مسبقاً فقط لتجنب تعارض الـ Allocator
    if (reshaped.size() >= (size_t)pStr->Max) return false;

    DWORD old;
    if (!VirtualProtect(pStr->Data, (size_t)pStr->Max * sizeof(wchar_t), PAGE_EXECUTE_READWRITE, &old)) return false;
    memcpy(pStr->Data, reshaped.data(), reshaped.size() * sizeof(wchar_t));
    pStr->Data[reshaped.size()] = L'\0';
    pStr->Num = (int)reshaped.size() + 1;
    VirtualProtect(pStr->Data, (size_t)pStr->Max * sizeof(wchar_t), old, &old);
    return true;
}

// ----------------------------------------------------------------------------
// مسح وتشكيل جميع الجداول النشطة في ذاكرة FL2GameData (SysString, Castle)
// ----------------------------------------------------------------------------
static int ScanAndReshapeAllGameData(HMODULE hNW) {
    if (!hNW || IsShuttingDown()) return 0;

    DWORD rvaGameData = GetRVA_Ptr_L2GameData();
    DWORD offsetSys = GetOFFSET_SysStringTable();
    DWORD offsetCastle = GetOFFSET_CastleTable();

    void** ppGameData = (void**)((BYTE*)hNW + rvaGameData);
    if (!IsValidReadablePtr(ppGameData, sizeof(void*))) return 0;

    BYTE* pGameData = (BYTE*)(*ppGameData);
    if (!IsValidReadablePtr(pGameData, 0x1000)) return 0;

    MEMORY_BASIC_INFORMATION mbi;
    if (VirtualQuery(pGameData, &mbi, sizeof(mbi)) == 0 || mbi.State != MEM_COMMIT)
        return 0;

    int reshapedCount = 0;

    // 1) جدول النصوص العامة SysStringTable (20,000 إدخال)
    BYTE* pSysBase = pGameData + offsetSys;
    if (IsValidReadablePtr(pSysBase, sizeof(FString_t) * 10)) {
        FString_t* pTable = (FString_t*)pSysBase;
        for (int id = 0; id < MAX_SYS_STRINGS; ++id) {
            if (IsShuttingDown()) break;
            FString_t* entry = &pTable[id];
            if (ReshapeFStringInPlace(entry)) {
                ++reshapedCount;
            }
        }
    }

    // 2) جدول أسماء القلاع CastleTable (100 إدخال، خطوة 72 بايت)
    // يحتوي كل إدخال على: szName (offset 0) و szLoc (offset sizeof(FString_t))
    BYTE* pCastleBase = pGameData + offsetCastle;
    if (IsValidReadablePtr(pCastleBase, 72 * 5)) {
        for (int id = 0; id < 100; ++id) {
            if (IsShuttingDown()) break;
            BYTE* entryBase = pCastleBase + id * 72;
            if (!IsValidReadablePtr(entryBase, 72)) continue;
            FString_t* pName = (FString_t*)(entryBase + 0);
            if (ReshapeFStringInPlace(pName)) {
                ++reshapedCount;
            }
            FString_t* pLoc = (FString_t*)(entryBase + sizeof(FString_t));
            if (ReshapeFStringInPlace(pLoc)) {
                ++reshapedCount;
            }
        }
    }

    // 3) جدول أسماء المناطق ZoneTable في FL2GameData (TMap<int, FZoneNameInfo*>)
    DWORD offsetZone = GetOFFSET_ZoneTable();
    void** ppZoneTable = (void**)(pGameData + offsetZone);
    if (IsValidReadablePtr(ppZoneTable, sizeof(void*))) {
        BYTE* pZoneTable = (BYTE*)(*ppZoneTable);
        if (IsValidReadablePtr(pZoneTable, 8)) {
            BYTE** ppArrayData = (BYTE**)pZoneTable;
            int count = *(int*)(pZoneTable + 4);
            if (count > 0 && count < 10000 && IsValidReadablePtr(ppArrayData, sizeof(void*))) {
                BYTE* pArray = *ppArrayData;
                if (IsValidReadablePtr(pArray, (size_t)count * 12)) {
                    for (int i = 0; i < count; ++i) {
                        if (IsShuttingDown()) break;
                        BYTE* entryBase = pArray + i * 12;
                        void** ppZone = (void**)(entryBase + 8);
                        if (IsValidReadablePtr(ppZone, sizeof(void*))) {
                            BYTE* pZone = (BYTE*)(*ppZone);
                            if (IsValidReadablePtr(pZone, 0x20)) {
                                FString_t* pName = (FString_t*)(pZone + 0x10);
                                if (ReshapeFStringInPlace(pName)) {
                                    ++reshapedCount;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return reshapedCount;
}

// ----------------------------------------------------------------------------
// مسار خلفي دوري لمعالجة أي نصوص يتم تحميلها لاحقاً
// ----------------------------------------------------------------------------
static DWORD WINAPI RuntimeStringReshaperThread(LPVOID lpParam) {
    HMODULE hNW = (HMODULE)lpParam;
    HookLog("RuntimeStringReshaperThread started");

    int totalReshaped = 0;

    // المرحلة الأولى: مسح متكرر أثناء بدء التشغيل وتحميل الملفات (كل ثانيتين لمدة 60 ثانية)
    for (int step = 0; step < 30; ++step) {
        if (IsShuttingDown()) {
            HookLog("RuntimeStringReshaperThread: shutdown signal received");
            return 0;
        }
        Sleep(2000);
        int n = ScanAndReshapeAllGameData(hNW);
        if (n > 0) {
            totalReshaped += n;
            HookLog("Reshaped " + std::to_string(n) +
                    " active resource strings (Total: " +
                    std::to_string(totalReshaped) + ")");
        }
    }

    // المرحلة الثانية: مسح دوري هادئ كل 10 ثوانٍ لأي موارد ديناميكية جديدة
    while (!IsShuttingDown()) {
        for (int i = 0; i < 50; ++i) {
            if (IsShuttingDown()) break;
            Sleep(200);
        }
        if (IsShuttingDown()) break;

        int n = ScanAndReshapeAllGameData(hNW);
        if (n > 0) {
            totalReshaped += n;
            HookLog("Reshaped " + std::to_string(n) +
                    " active resource strings (Total: " +
                    std::to_string(totalReshaped) + ")");
        }
    }

    HookLog("RuntimeStringReshaperThread exiting cleanly");
    return 0;
}

// ----------------------------------------------------------------------------
// هوك دالة إسناد النصوص
// ----------------------------------------------------------------------------
typedef void (__fastcall* fnGlobalSetText)(void* pDst, void* edxDummy, FString_t* pSrc);
static fnGlobalSetText g_origGlobalSetText = nullptr;
static thread_local bool g_inReshape = false;

static void __fastcall Hook_GlobalSetText(void* pDst, void* edxDummy, FString_t* pSrc) {
    if (g_inReshape || !pSrc || !pSrc->Data || pSrc->Num <= 1) {
        if (g_origGlobalSetText) g_origGlobalSetText(pDst, nullptr, pSrc);
        return;
    }
    // يعالج النص من أي مورد بعد تحميله؛ لا يقتصر على ملف SysString بعينه.
    ReshapeFStringInPlace(pSrc);
    g_inReshape = true;
    if (g_origGlobalSetText) g_origGlobalSetText(pDst, nullptr, pSrc);
    g_inReshape = false;
}

// ----------------------------------------------------------------------------
// تثبيت هوك على مؤشر دالة في جدول الاستيراد IAT
// ----------------------------------------------------------------------------
static void* HookIatSlot(HMODULE hNW, DWORD rva, void* hookFn) {
    if (!hNW || rva == 0) return nullptr;
    void** pSlot = (void**)((BYTE*)hNW + rva);
    if (!IsValidReadablePtr(pSlot, sizeof(void*))) return nullptr;

    void* original = *pSlot;
    if (!original) return nullptr;

    DWORD old;
    if (!VirtualProtect(pSlot, sizeof(void*), PAGE_READWRITE, &old)) {
        HookLog("Unable to make pointer writable");
        return nullptr;
    }
    *pSlot = hookFn;
    if (!VirtualProtect(pSlot, sizeof(void*), old, &old))
        HookLog("Unable to restore pointer protection");
    FlushInstructionCache(GetCurrentProcess(), pSlot, sizeof(void*));
    return original;
}

// ----------------------------------------------------------------------------
// هوكات FString من مؤشرات النصوص الواسعة
// ----------------------------------------------------------------------------
static void* __fastcall Hook_FStringAssignW(void* thisPtr, void* edxDummy, const wchar_t* src) {
    if (g_inReshape || !src || !g_origFStringAssignW) {
        return g_origFStringAssignW ? g_origFStringAssignW(thisPtr, src) : thisPtr;
    }
    int len = SafeWcsLen(src);
    if (len > 0 && !HasPresentationForms(src, len) && HasRawArabic(src, len)) {
        g_inReshape = true;
        std::wstring reshaped = ArabicBiDi::ReshapeCache::Instance().Get(src);
        void* ret = g_origFStringAssignW(thisPtr, reshaped.c_str());
        g_inReshape = false;
        return ret;
    }
    return g_origFStringAssignW(thisPtr, src);
}

typedef void* (__thiscall* fnFStringCtorW)(void* thisPtr, const wchar_t* src);
static fnFStringCtorW g_origFStringCtorW = nullptr;

static void* __fastcall Hook_FStringCtorW(void* thisPtr, void* edxDummy, const wchar_t* src) {
    if (g_inReshape || !src || !g_origFStringCtorW) {
        return g_origFStringCtorW ? g_origFStringCtorW(thisPtr, src) : thisPtr;
    }
    int len = SafeWcsLen(src);
    if (len > 0 && !HasPresentationForms(src, len) && HasRawArabic(src, len)) {
        g_inReshape = true;
        std::wstring reshaped = ArabicBiDi::ReshapeCache::Instance().Get(src);
        void* ret = g_origFStringCtorW(thisPtr, reshaped.c_str());
        g_inReshape = false;
        return ret;
    }
    return g_origFStringCtorW(thisPtr, src);
}

// ----------------------------------------------------------------------------
// هوكات رسم وحساب نصوص الخريطة والـ Canvas
// ----------------------------------------------------------------------------
typedef int (__thiscall* fnDrawTextTTFToCanvas)(void* thisPtr, int X, int Y, const wchar_t* pText, const void* pInfo, BYTE bUnknown, int n1, int n2, const void* pSections, int* pOut);
static fnDrawTextTTFToCanvas g_origDrawTextTTFToCanvas = nullptr;

static int __fastcall Hook_DrawTextTTFToCanvas(
    void* thisPtr, void* edxDummy,
    int X, int Y, const wchar_t* pText, const void* pInfo,
    BYTE bUnknown, int n1, int n2, const void* pSections, int* pOut)
{
    if (!pText || !g_origDrawTextTTFToCanvas) {
        return g_origDrawTextTTFToCanvas ? g_origDrawTextTTFToCanvas(thisPtr, X, Y, pText, pInfo, bUnknown, n1, n2, pSections, pOut) : 0;
    }
    int len = SafeWcsLen(pText);
    if (len > 0 && !HasPresentationForms(pText, len) && HasRawArabic(pText, len)) {
        std::wstring reshaped = ArabicBiDi::ReshapeCache::Instance().Get(pText);
        return g_origDrawTextTTFToCanvas(thisPtr, X, Y, reshaped.c_str(), pInfo, bUnknown, n1, n2, pSections, pOut);
    }
    return g_origDrawTextTTFToCanvas(thisPtr, X, Y, pText, pInfo, bUnknown, n1, n2, pSections, pOut);
}

typedef int (__thiscall* fnGetTextSizeTTF)(void* thisPtr, SIZE* outSize, const wchar_t* pText, const void* pInfo, int n1, int n2, int* pOut, int n3);
static fnGetTextSizeTTF g_origGetTextSizeTTF = nullptr;

static int __fastcall Hook_GetTextSizeTTF(
    void* thisPtr, void* edxDummy,
    SIZE* outSize, const wchar_t* pText, const void* pInfo,
    int n1, int n2, int* pOut, int n3)
{
    if (!pText || !g_origGetTextSizeTTF) {
        return g_origGetTextSizeTTF ? g_origGetTextSizeTTF(thisPtr, outSize, pText, pInfo, n1, n2, pOut, n3) : 0;
    }
    int len = SafeWcsLen(pText);
    if (len > 0 && !HasPresentationForms(pText, len) && HasRawArabic(pText, len)) {
        std::wstring reshaped = ArabicBiDi::ReshapeCache::Instance().Get(pText);
        return g_origGetTextSizeTTF(thisPtr, outSize, reshaped.c_str(), pInfo, n1, n2, pOut, n3);
    }
    return g_origGetTextSizeTTF(thisPtr, outSize, pText, pInfo, n1, n2, pOut, n3);
}

typedef int (__thiscall* fnGetTextSizeTTFList)(void* thisPtr, SIZE* outSize, void* outList, const wchar_t* pText, const void* pInfo, int n1, int n2, int n3);
static fnGetTextSizeTTFList g_origGetTextSizeTTFList = nullptr;

static int __fastcall Hook_GetTextSizeTTFList(
    void* thisPtr, void* edxDummy,
    SIZE* outSize, void* outList, const wchar_t* pText, const void* pInfo,
    int n1, int n2, int n3)
{
    if (!pText || !g_origGetTextSizeTTFList) {
        return g_origGetTextSizeTTFList ? g_origGetTextSizeTTFList(thisPtr, outSize, outList, pText, pInfo, n1, n2, n3) : 0;
    }
    int len = SafeWcsLen(pText);
    if (len > 0 && !HasPresentationForms(pText, len) && HasRawArabic(pText, len)) {
        std::wstring reshaped = ArabicBiDi::ReshapeCache::Instance().Get(pText);
        return g_origGetTextSizeTTFList(thisPtr, outSize, outList, reshaped.c_str(), pInfo, n1, n2, n3);
    }
    return g_origGetTextSizeTTFList(thisPtr, outSize, outList, pText, pInfo, n1, n2, n3);
}

// ----------------------------------------------------------------------------
// دوال وهوكات الخرائط والمناطق (Minimap, Radar, Zone Names, FName, FormatString)
// ----------------------------------------------------------------------------
static bool ReshapeRawWStringInPlace(wchar_t* str) {
    if (!str || IsShuttingDown()) return false;
    int len = SafeWcsLen(str, 512);
    if (len <= 0) return false;
    if (HasPresentationForms(str, len)) return false;
    if (!HasRawArabic(str, len)) return false;

    std::wstring raw(str, len);
    std::wstring reshaped = ArabicBiDi::ReshapeCache::Instance().Get(raw);
    if (reshaped.empty() || reshaped == raw || reshaped.size() > (size_t)len) return false;

    DWORD old;
    if (!VirtualProtect(str, (len + 1) * sizeof(wchar_t), PAGE_EXECUTE_READWRITE, &old)) return false;
    memcpy(str, reshaped.data(), reshaped.size() * sizeof(wchar_t));
    str[reshaped.size()] = L'\0';
    VirtualProtect(str, (len + 1) * sizeof(wchar_t), old, &old);
    return true;
}

// 1) مؤشر دالة FName::GetEntry الأصلية (بدون هوك عام لتجنب التأثير على محرك اللعبة)
typedef void* (__cdecl* fnFNameGetEntry)(int index);
static fnFNameGetEntry g_fnFNameGetEntry = nullptr;

// هوك FL2GameData::GetMinimapRegionData - تشكيل أسماء المناطق المعروضة على الخريطة
typedef void* (__thiscall* fnGetMinimapRegionData)(void* thisPtr, int regionID);
static fnGetMinimapRegionData g_origGetMinimapRegionData = nullptr;

static void* __fastcall Hook_GetMinimapRegionData(void* thisPtr, void* edxDummy, int regionID) {
    if (!g_origGetMinimapRegionData) return nullptr;
    void* data = g_origGetMinimapRegionData(thisPtr, regionID);
    if (!data || IsShuttingDown()) return data;

    if (g_fnFNameGetEntry && IsValidReadablePtr(data, 16)) {
        int nameIndex = *(int*)((BYTE*)data + 4);
        if (nameIndex > 0) {
            void* entry = g_fnFNameGetEntry(nameIndex);
            if (entry && IsValidReadablePtr(entry, 0x10)) {
                wchar_t* pName = (wchar_t*)((BYTE*)entry + 0x0C);
                ReshapeRawWStringInPlace(pName);
            }
        }
        int locIndex = *(int*)((BYTE*)data + 8);
        if (locIndex > 0) {
            void* entry = g_fnFNameGetEntry(locIndex);
            if (entry && IsValidReadablePtr(entry, 0x10)) {
                wchar_t* pLoc = (wchar_t*)((BYTE*)entry + 0x0C);
                ReshapeRawWStringInPlace(pLoc);
            }
        }
    }
    return data;
}

// 2) هوك FL2GameData::GetZoneNameInfoByID - تشكيل تفاصيل ومعلومات المنطقة
typedef void* (__thiscall* fnGetZoneNameInfoByID)(void* thisPtr, int zoneID);
static fnGetZoneNameInfoByID g_origGetZoneNameInfoByID = nullptr;

static void* __fastcall Hook_GetZoneNameInfoByID(void* thisPtr, void* edxDummy, int zoneID) {
    if (!g_origGetZoneNameInfoByID) return nullptr;
    void* info = g_origGetZoneNameInfoByID(thisPtr, zoneID);
    if (!info || IsShuttingDown()) return info;

    if (IsValidReadablePtr(info, 0x20)) {
        // بنية FZoneNameInfo: offset +0x10 هو كائن FString_t Name
        FString_t* pStr = (FString_t*)((BYTE*)info + 0x10);
        ReshapeFStringInPlace(pStr);
    }
    return info;
}

// 3) هوك FL2GameData::GetZoneName - تشكيل اسم المنطقة بالإحداثيات
typedef wchar_t* (__thiscall* fnGetZoneName)(void* thisPtr, float X, float Y, float Z);
static fnGetZoneName g_origGetZoneName = nullptr;

static wchar_t* __fastcall Hook_GetZoneName(void* thisPtr, void* edxDummy, float X, float Y, float Z) {
    if (!g_origGetZoneName) return nullptr;
    wchar_t* ret = g_origGetZoneName(thisPtr, X, Y, Z);
    if (ret && !IsShuttingDown()) ReshapeRawWStringInPlace(ret);
    return ret;
}

// 4) هوك FL2GameData::GetCurrentZoneName - تشكيل اسم المنطقة الحالية (الرادار وعنوان المنطقة)
typedef wchar_t* (__thiscall* fnGetCurrentZoneName)(void* thisPtr);
static fnGetCurrentZoneName g_origGetCurrentZoneName = nullptr;

static wchar_t* __fastcall Hook_GetCurrentZoneName(void* thisPtr, void* edxDummy) {
    if (!g_origGetCurrentZoneName) return nullptr;
    wchar_t* ret = g_origGetCurrentZoneName(thisPtr);
    if (ret && !IsShuttingDown()) ReshapeRawWStringInPlace(ret);
    return ret;
}

// 5) هوك FL2GameData::GetZoneNameFromID - تشكيل اسم المنطقة برقم الـ ID
typedef wchar_t* (__thiscall* fnGetZoneNameFromID)(void* thisPtr, int zoneID);
static fnGetZoneNameFromID g_origGetZoneNameFromID = nullptr;

static wchar_t* __fastcall Hook_GetZoneNameFromID(void* thisPtr, void* edxDummy, int zoneID) {
    if (!g_origGetZoneNameFromID) return nullptr;
    wchar_t* ret = g_origGetZoneNameFromID(thisPtr, zoneID);
    if (ret && !IsShuttingDown()) ReshapeRawWStringInPlace(ret);
    return ret;
}

// ----------------------------------------------------------------------------
// هوك FL2GameData::GetHuntingZoneData: تشكيل أسماء وأوصاف مناطق الصيد
// ----------------------------------------------------------------------------
typedef void* (__thiscall* fnGetHuntingZoneData)(void* thisPtr, int zoneID);
static fnGetHuntingZoneData g_origGetHuntingZoneData = nullptr;

static void* __fastcall Hook_GetHuntingZoneData(void* thisPtr, void* edxDummy, int zoneID) {
    if (!g_origGetHuntingZoneData) return nullptr;
    void* data = g_origGetHuntingZoneData(thisPtr, zoneID);
    if (!data || IsShuttingDown()) return data;

    if (IsValidReadablePtr(data, 0x40)) {
        FString_t* pDesc = (FString_t*)((BYTE*)data + 0x1C);
        ReshapeFStringInPlace(pDesc);
        FString_t* pName = (FString_t*)((BYTE*)data + 0x28);
        ReshapeFStringInPlace(pName);
    }
    return data;
}

// ----------------------------------------------------------------------------
// بنية DetourHook_t لتثبيت هوكات inline بأمان
// ----------------------------------------------------------------------------
struct DetourHook_t {
    BYTE* target = nullptr;
    BYTE* trampoline = nullptr;

    bool Install(void* pTarget, void* pHook) {
        if (!pTarget || !pHook) return false;
        target = (BYTE*)pTarget;

        trampoline = (BYTE*)VirtualAlloc(nullptr, 64, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
        if (!trampoline) return false;

        memcpy(trampoline, target, 5);
        trampoline[5] = 0xE9;
        LONG jmpBack = (LONG)(target + 5) - (LONG)(trampoline + 5 + 5);
        memcpy(trampoline + 6, &jmpBack, sizeof(LONG));
        FlushInstructionCache(GetCurrentProcess(), trampoline, 64);

        DWORD oldProt;
        if (!VirtualProtect(target, 5, PAGE_EXECUTE_READWRITE, &oldProt))
            return false;
        target[0] = 0xE9;
        LONG jmpToHook = (LONG)pHook - (LONG)(target + 5);
        memcpy(target + 1, &jmpToHook, sizeof(LONG));
        VirtualProtect(target, 5, oldProt, &oldProt);
        FlushInstructionCache(GetCurrentProcess(), target, 5);
        return true;
    }

    bool IsInstalled() const {
        return target != nullptr;
    }

    void* GetOriginal() {
        return (void*)trampoline;
    }
};

// ----------------------------------------------------------------------------
// هوك BuildZoneFlashObject: اعتراض بناء كائن الفلاش للمنطقة لتشكيل النص العربي
// ----------------------------------------------------------------------------
typedef void (__fastcall* fnBuildZoneFlashObject)(void* pDst, void* pZoneInfo);
static DetourHook_t g_hookBuildZoneFlashObject;

static void __fastcall Hook_BuildZoneFlashObject(void* pDst, void* pZoneInfo) {
    if (pZoneInfo && IsValidReadablePtr(pZoneInfo, 0x20) && !IsShuttingDown()) {
        FString_t* pName = (FString_t*)((BYTE*)pZoneInfo + 0x10);
        ReshapeFStringInPlace(pName);
    }
    fnBuildZoneFlashObject orig = (fnBuildZoneFlashObject)g_hookBuildZoneFlashObject.GetOriginal();
    if (orig) {
        orig(pDst, pZoneInfo);
    }
}

// ----------------------------------------------------------------------------
// هوك BuildHuntingZoneFlashObject: اعتراض بناء كائن الفلاش لمناطق الصيد لتشكيل النص العربي
// ----------------------------------------------------------------------------
typedef void (__fastcall* fnBuildHuntingZoneFlashObject)(void* pDst, void* pHuntingZone);
static DetourHook_t g_hookBuildHuntingZoneFlashObject;

static void __fastcall Hook_BuildHuntingZoneFlashObject(void* pDst, void* pHuntingZone) {
    if (pHuntingZone && IsValidReadablePtr(pHuntingZone, 0x40) && !IsShuttingDown()) {
        FString_t* pDesc = (FString_t*)((BYTE*)pHuntingZone + 0x1C);
        ReshapeFStringInPlace(pDesc);
        FString_t* pName = (FString_t*)((BYTE*)pHuntingZone + 0x28);
        ReshapeFStringInPlace(pName);
    }
    fnBuildHuntingZoneFlashObject orig = (fnBuildHuntingZoneFlashObject)g_hookBuildHuntingZoneFlashObject.GetOriginal();
    if (orig) {
        orig(pDst, pHuntingZone);
    }
}

// 6) معالجة رموز وتنسيقات نصوص الكانفاس FL2FormatStringParser
typedef int (__thiscall* fnGetSizeParser)(const void* parser);
typedef void* (__thiscall* fnGetAtParser)(const void* parser, int index);
static fnGetSizeParser g_fnGetSizeParser = nullptr;
static fnGetAtParser g_fnGetAtParser = nullptr;

static void ReshapeFormatStringTokens(const void* parser) {
    if (!parser || !g_fnGetSizeParser || !g_fnGetAtParser || IsShuttingDown()) return;
    int count = g_fnGetSizeParser(parser);
    if (count <= 0 || count > 500) return;
    for (int i = 0; i < count; ++i) {
        BYTE* token = (BYTE*)g_fnGetAtParser(parser, i);
        if (token && IsValidReadablePtr(token, 16) && *(int*)token == 1) { // Type 1: Text Token
            FString_t* pStr = (FString_t*)(token + 4);
            ReshapeFStringInPlace(pStr);
        }
    }
}

typedef int (__thiscall* fnDrawTextTTFFormatStringToCanvas)(
    void* thisPtr, int X, int Y, const void* pParser, const void* pInfo,
    int n1, int n2, const void* pSections, BYTE bUnknown, int* pOut);
static fnDrawTextTTFFormatStringToCanvas g_origDrawTextTTFFormatStringToCanvas = nullptr;

static int __fastcall Hook_DrawTextTTFFormatStringToCanvas(
    void* thisPtr, void* edxDummy,
    int X, int Y, const void* pParser, const void* pInfo,
    int n1, int n2, const void* pSections, BYTE bUnknown, int* pOut)
{
    if (pParser) {
        ReshapeFormatStringTokens(pParser);
    }
    return g_origDrawTextTTFFormatStringToCanvas ?
        g_origDrawTextTTFFormatStringToCanvas(thisPtr, X, Y, pParser, pInfo, n1, n2, pSections, bUnknown, pOut) : 0;
}

typedef int (__thiscall* fnGetTextSizeTTFFormatString)(
    void* thisPtr, SIZE* outSize, const void* pParser, const void* pInfo,
    int n1, int n2, int* pOut, int n3);
static fnGetTextSizeTTFFormatString g_origGetTextSizeTTFFormatString = nullptr;

static int __fastcall Hook_GetTextSizeTTFFormatString(
    void* thisPtr, void* edxDummy,
    SIZE* outSize, const void* pParser, const void* pInfo,
    int n1, int n2, int* pOut, int n3)
{
    if (pParser) {
        ReshapeFormatStringTokens(pParser);
    }
    return g_origGetTextSizeTTFFormatString ?
        g_origGetTextSizeTTFFormatString(thisPtr, outSize, pParser, pInfo, n1, n2, pOut, n3) : 0;
}

// ----------------------------------------------------------------------------
// تحميل خط Cairo المخصص ودعم استبدال خطوط GDI
// ----------------------------------------------------------------------------
static bool g_CairoFontLoaded = false;

static void LoadCairoFont(HMODULE hNW) {
    if (g_CairoFontLoaded) return;
    wchar_t fontPath[MAX_PATH] = { 0 };
    if (hNW) {
        GetModuleFileNameW(hNW, fontPath, MAX_PATH);
        wchar_t* lastSlash = wcsrchr(fontPath, L'\\');
        if (lastSlash) {
            *(lastSlash + 1) = L'\0';
            wcscat_s(fontPath, L"Cairo.ttf");
        }
    }
    if (fontPath[0] == L'\0' || GetFileAttributesW(fontPath) == INVALID_FILE_ATTRIBUTES) {
        wchar_t curDir[MAX_PATH] = { 0 };
        GetCurrentDirectoryW(MAX_PATH, curDir);
        swprintf_s(fontPath, L"%s\\Cairo.ttf", curDir);
    }

    if (GetFileAttributesW(fontPath) != INVALID_FILE_ATTRIBUTES) {
        int res = AddFontResourceExW(fontPath, FR_PRIVATE, 0);
        if (res > 0) {
            g_CairoFontLoaded = true;
            HookLog("Cairo font loaded successfully via AddFontResourceExW (private)");

            // تحميل نسخة الخط العريض Cairo-Bold إن وجدت
            wchar_t boldPath[MAX_PATH] = { 0 };
            wcscpy_s(boldPath, fontPath);
            wchar_t* p = wcsrchr(boldPath, L'\\');
            if (p) {
                *(p + 1) = L'\0';
                wcscat_s(boldPath, L"Cairo-Bold.ttf");
                if (GetFileAttributesW(boldPath) != INVALID_FILE_ATTRIBUTES) {
                    int resB = AddFontResourceExW(boldPath, FR_PRIVATE, 0);
                    if (resB > 0) {
                        HookLog("Cairo-Bold font loaded successfully via AddFontResourceExW");
                    }
                }
            }
        } else {
            HookLog("AddFontResourceExW returned 0 for Cairo.ttf");
        }
    } else {
        HookLog("Cairo.ttf file not found in system folder");
    }
}

// ----------------------------------------------------------------------------
// هوك دالة CreateFontW لتطبيق خط Cairo أو الخط المخصص
// ----------------------------------------------------------------------------
typedef HFONT (WINAPI* fnCreateFontW)(
    int cHeight, int cWidth, int cEscapement, int cOrientation, int cWeight,
    DWORD bItalic, DWORD bUnderline, DWORD bStrikeOut, DWORD iCharSet,
    DWORD iOutPrecision, DWORD iClipPrecision, DWORD iQuality,
    DWORD iPitchAndFamily, LPCWSTR pszFaceName);

static fnCreateFontW g_origCreateFontW = nullptr;

static HFONT WINAPI Hook_CreateFontW(
    int cHeight, int cWidth, int cEscapement, int cOrientation, int cWeight,
    DWORD bItalic, DWORD bUnderline, DWORD bStrikeOut, DWORD iCharSet,
    DWORD iOutPrecision, DWORD iClipPrecision, DWORD iQuality,
    DWORD iPitchAndFamily, LPCWSTR pszFaceName)
{
    if (!g_origCreateFontW) return nullptr;

    LPCWSTR targetFace = pszFaceName;

    if (g_Config.UseCustomFont && g_CairoFontLoaded) {
        bool skipCustom = false;
        if (pszFaceName) {
            // استثناء خط أرقام الضربات والضرر فقط ($DamageTextFont = Sylfaen)
            if (_wcsicmp(pszFaceName, L"Sylfaen") == 0 ||
                wcsstr(pszFaceName, L"Sylfaen") != nullptr)
            {
                skipCustom = true;
            }
        }
        if (!skipCustom) {
            targetFace = g_Config.CustomFontName.c_str();
            iCharSet = DEFAULT_CHARSET;
        }
    }

    static int s_createFontLogCount = 0;
    if (s_createFontLogCount < 60) {
        s_createFontLogCount++;
        char buf[256];
        sprintf_s(buf, "[CreateFontW #%d] face='%ls' h=%d w=%d -> target='%ls'",
            s_createFontLogCount,
            pszFaceName ? pszFaceName : L"(null)",
            cHeight, cWeight,
            targetFace ? targetFace : L"(null)");
        HookLog(buf);
    }

    return g_origCreateFontW(
        cHeight, cWidth, cEscapement, cOrientation, cWeight,
        bItalic, bUnderline, bStrikeOut, iCharSet,
        iOutPrecision, iClipPrecision, iQuality,
        iPitchAndFamily, targetFace);
}

// ----------------------------------------------------------------------------
// هوك دالة UFontManager::CreateFontFace
// ?CreateFontFace@UFontManager@@QAE?AVFONTID@@ABVFString@@0HHPBVFontProperty@@@Z
// المسؤولة عن تسجيل وإنشاء كافة خطوط المحرك والواجهة (Tooltips, Chat, HTML, Menus, Items)
// ----------------------------------------------------------------------------
typedef void* (__thiscall* fnCreateFontFace)(
    void* thisPtr,
    void* resultBuf,
    const FString_t* pFontName,
    const FString_t* pFontFace,
    int size,
    int weight,
    const void* pProp);

static fnCreateFontFace g_origCreateFontFace = nullptr;

static void* __fastcall Hook_CreateFontFace(
    void* thisPtr, void* edxDummy,
    void* resultBuf,
    const FString_t* pFontName,
    const FString_t* pFontFace,
    int size,
    int weight,
    const void* pProp)
{
    if (!g_origCreateFontFace) return nullptr;

    const wchar_t* fontName = (pFontName && pFontName->Data) ? pFontName->Data : L"";
    const wchar_t* fontFace = (pFontFace && pFontFace->Data) ? pFontFace->Data : L"";

    const FString_t* targetFace = pFontFace;
    FString_t customFace;
    wchar_t cairoPath[MAX_PATH] = { 0 };

    if (g_Config.UseCustomFont && g_CairoFontLoaded) {
        bool skip = false;
        if (fontFace && (wcsstr(fontFace, L"Sylfaen") != nullptr || wcsstr(fontFace, L"sylfaen") != nullptr)) {
            skip = true;
        }
        if (!skip) {
            // فحص إذا كان مسار ملف كامل أو ينتهي بـ ttf أو مسار خطوط
            if (fontFace && (wcsstr(fontFace, L":\\") != nullptr ||
                             wcsstr(fontFace, L".ttf") != nullptr ||
                             wcsstr(fontFace, L".TTF") != nullptr ||
                             wcsstr(fontFace, L"Fonts") != nullptr))
            {
                if (weight >= 700) {
                    wcscpy_s(cairoPath, L"C:\\Windows\\Fonts\\Cairo-Bold.ttf");
                } else {
                    wcscpy_s(cairoPath, L"C:\\Windows\\Fonts\\Cairo.ttf");
                }
            } else {
                // اسم عائلة الخط مباشرة
                wcscpy_s(cairoPath, g_Config.CustomFontName.c_str());
            }

            customFace.Data = cairoPath;
            customFace.Num = (int)wcslen(cairoPath) + 1;
            customFace.Max = customFace.Num;
            targetFace = &customFace;
        }
    }

    static int s_cffLogCount = 0;
    if (s_cffLogCount < 60) {
        s_cffLogCount++;
        char buf[512];
        const wchar_t* targetFaceStr = (targetFace && targetFace->Data) ? targetFace->Data : L"(null)";
        sprintf_s(buf, "[CreateFontFace #%d] name='%ls' face='%ls' sz=%d wt=%d -> target='%ls'",
            s_cffLogCount, fontName, fontFace, size, weight, targetFaceStr);
        HookLog(buf);
    }

    return g_origCreateFontFace(thisPtr, resultBuf, pFontName, targetFace, size, weight, pProp);
}

// ----------------------------------------------------------------------------
// التثبيت الرئيسي
// ----------------------------------------------------------------------------
static bool g_NWindowHooked = false;

static void InstallNWindowArabicHooks() {
    if (g_NWindowHooked) return;

    HMODULE hNW = GetModuleHandleA("NWindow.dll");
    if (!hNW) return;

    if (!g_Config.ArabicWindows) {
        HookLog("ArabicWindows disabled - skipping NWindow hooks");
        g_NWindowHooked = true;
        return;
    }

    DWORD rvaFnPtr = GetRVA_FnPtr_GlobalSetText();
    g_origGlobalSetText = (fnGlobalSetText)HookIatSlot(hNW, rvaFnPtr, (void*)Hook_GlobalSetText);
    if (g_origGlobalSetText) {
        char buf[64];
        sprintf_s(buf, "Hooked text-setter fn-ptr @0x%08X", rvaFnPtr);
        HookLog(buf);
    } else {
        char buf[64];
        sprintf_s(buf, "FAILED to hook text-setter fn-ptr @0x%08X", rvaFnPtr);
        HookLog(buf);
    }

    DWORD rvaFStringAssignW = GetRVA_FnPtr_FStringAssignW();
    g_origFStringAssignW = (fnFStringAssignW)HookIatSlot(hNW, rvaFStringAssignW, (void*)Hook_FStringAssignW);
    if (g_origFStringAssignW) {
        HookLog("Hooked FString operator=(const wchar_t*) IAT slot");
    }

    DWORD rvaFStringCtorW = GetRVA_FnPtr_FStringCtorW();
    g_origFStringCtorW = (fnFStringCtorW)HookIatSlot(hNW, rvaFStringCtorW, (void*)Hook_FStringCtorW);
    if (g_origFStringCtorW) {
        HookLog("Hooked FString ctor(const wchar_t*) IAT slot");
    }

    DWORD rvaDrawTextCanvas = GetRVA_FnPtr_DrawTextCanvas();
    g_origDrawTextTTFToCanvas = (fnDrawTextTTFToCanvas)HookIatSlot(hNW, rvaDrawTextCanvas, (void*)Hook_DrawTextTTFToCanvas);
    if (g_origDrawTextTTFToCanvas) {
        HookLog("Hooked UCanvas::DrawTextTTFToCanvas IAT slot");
    }

    DWORD rvaGetTextSizeTTF = GetRVA_FnPtr_GetTextSizeTTF();
    g_origGetTextSizeTTF = (fnGetTextSizeTTF)HookIatSlot(hNW, rvaGetTextSizeTTF, (void*)Hook_GetTextSizeTTF);
    if (g_origGetTextSizeTTF) {
        HookLog("Hooked UCanvas::GetTextSizeTTF IAT slot");
    }

    DWORD rvaGetTextSizeList = GetRVA_FnPtr_GetTextSizeList();
    g_origGetTextSizeTTFList = (fnGetTextSizeTTFList)HookIatSlot(hNW, rvaGetTextSizeList, (void*)Hook_GetTextSizeTTFList);
    if (g_origGetTextSizeTTFList) {
        HookLog("Hooked UCanvas::GetTextSizeTTFList IAT slot");
    }

    DWORD rvaFNameGetEntry = GetRVA_FnPtr_FNameGetEntry();
    void** pSlotFName = (void**)((BYTE*)hNW + rvaFNameGetEntry);
    if (IsValidReadablePtr(pSlotFName, sizeof(void*))) {
        g_fnFNameGetEntry = (fnFNameGetEntry)*pSlotFName;
        HookLog("Resolved FName::GetEntry pointer from IAT");
    }

    DWORD rvaMinimapRegion = GetRVA_FnPtr_GetMinimapRegionData();
    g_origGetMinimapRegionData = (fnGetMinimapRegionData)HookIatSlot(hNW, rvaMinimapRegion, (void*)Hook_GetMinimapRegionData);
    if (g_origGetMinimapRegionData) {
        HookLog("Hooked FL2GameData::GetMinimapRegionData IAT slot");
    }

    DWORD rvaZoneInfo = GetRVA_FnPtr_GetZoneNameInfoByID();
    g_origGetZoneNameInfoByID = (fnGetZoneNameInfoByID)HookIatSlot(hNW, rvaZoneInfo, (void*)Hook_GetZoneNameInfoByID);
    if (g_origGetZoneNameInfoByID) {
        HookLog("Hooked FL2GameData::GetZoneNameInfoByID IAT slot");
    }

    DWORD rvaGetZoneName = GetRVA_FnPtr_GetZoneName();
    g_origGetZoneName = (fnGetZoneName)HookIatSlot(hNW, rvaGetZoneName, (void*)Hook_GetZoneName);
    if (g_origGetZoneName) {
        HookLog("Hooked FL2GameData::GetZoneName IAT slot");
    }

    DWORD rvaGetCurrentZoneName = GetRVA_FnPtr_GetCurrentZoneName();
    g_origGetCurrentZoneName = (fnGetCurrentZoneName)HookIatSlot(hNW, rvaGetCurrentZoneName, (void*)Hook_GetCurrentZoneName);
    if (g_origGetCurrentZoneName) {
        HookLog("Hooked FL2GameData::GetCurrentZoneName IAT slot");
    }

    DWORD rvaGetZoneNameFromID = GetRVA_FnPtr_GetZoneNameFromID();
    g_origGetZoneNameFromID = (fnGetZoneNameFromID)HookIatSlot(hNW, rvaGetZoneNameFromID, (void*)Hook_GetZoneNameFromID);
    if (g_origGetZoneNameFromID) {
        HookLog("Hooked FL2GameData::GetZoneNameFromID IAT slot");
    }

    DWORD rvaParserSize = GetRVA_FnPtr_ParserGetSize();
    DWORD rvaParserAt = GetRVA_FnPtr_ParserGetAt();
    void** pSlotParserSize = (void**)((BYTE*)hNW + rvaParserSize);
    void** pSlotParserAt = (void**)((BYTE*)hNW + rvaParserAt);
    if (IsValidReadablePtr(pSlotParserSize, sizeof(void*))) g_fnGetSizeParser = (fnGetSizeParser)*pSlotParserSize;
    if (IsValidReadablePtr(pSlotParserAt, sizeof(void*))) g_fnGetAtParser = (fnGetAtParser)*pSlotParserAt;

    DWORD rvaDrawTextFormat = GetRVA_FnPtr_DrawTextFormatCanvas();
    g_origDrawTextTTFFormatStringToCanvas = (fnDrawTextTTFFormatStringToCanvas)HookIatSlot(hNW, rvaDrawTextFormat, (void*)Hook_DrawTextTTFFormatStringToCanvas);
    if (g_origDrawTextTTFFormatStringToCanvas) {
        HookLog("Hooked UCanvas::DrawTextTTFFormatStringToCanvas IAT slot");
    }

    DWORD rvaGetTextSizeFormat = GetRVA_FnPtr_GetTextSizeFormatCanvas();
    g_origGetTextSizeTTFFormatString = (fnGetTextSizeTTFFormatString)HookIatSlot(hNW, rvaGetTextSizeFormat, (void*)Hook_GetTextSizeTTFFormatString);
    if (g_origGetTextSizeTTFFormatString) {
        HookLog("Hooked UCanvas::GetTextSizeTTFFormatString IAT slot");
    }

    // هوك FL2GameData::GetHuntingZoneData — تشكيل أسماء مناطق الصيد في الخريطة
    DWORD rvaGetHuntingZoneData = GetRVA_FnPtr_GetHuntingZoneData();
    g_origGetHuntingZoneData = (fnGetHuntingZoneData)HookIatSlot(hNW, rvaGetHuntingZoneData, (void*)Hook_GetHuntingZoneData);
    if (g_origGetHuntingZoneData) {
        HookLog("Hooked FL2GameData::GetHuntingZoneData IAT slot");
    }

    // هوك BuildZoneFlashObject — تشكيل أسماء المناطق في خريطة الفلاش
    DWORD rvaBuildZone = GetRVA_Fn_BuildZoneFlashObject();
    if (rvaBuildZone != 0) {
        void* pBuildZone = (void*)((BYTE*)hNW + rvaBuildZone);
        if (IsValidReadablePtr(pBuildZone, 16)) {
            bool installed = g_hookBuildZoneFlashObject.Install(pBuildZone, (void*)Hook_BuildZoneFlashObject);
            if (installed) {
                HookLog("Installed inline detour: BuildZoneFlashObject");
            } else {
                HookLog("FAILED to install BuildZoneFlashObject detour");
            }
        }
    }

    // هوك BuildHuntingZoneFlashObject — تشكيل كائنات الفلاش لمناطق الصيد في الخريطة
    DWORD rvaBuildHuntingZone = GetRVA_Fn_BuildHuntingZoneFlashObject();
    if (rvaBuildHuntingZone != 0) {
        void* pBuildHuntingZone = (void*)((BYTE*)hNW + rvaBuildHuntingZone);
        if (IsValidReadablePtr(pBuildHuntingZone, 16)) {
            bool installed = g_hookBuildHuntingZoneFlashObject.Install(pBuildHuntingZone, (void*)Hook_BuildHuntingZoneFlashObject);
            if (installed) {
                HookLog("Installed inline detour: BuildHuntingZoneFlashObject");
            } else {
                HookLog("FAILED to install BuildHuntingZoneFlashObject detour");
            }
        }
    }

    // تحميل وتفعيل خط Cairo المخصص للنصوص والقوائم
    if (g_Config.UseCustomFont) {
        LoadCairoFont(hNW);

        DWORD rvaCreateFontW = GetRVA_FnPtr_CreateFontW();
        g_origCreateFontW = (fnCreateFontW)HookIatSlot(hNW, rvaCreateFontW, (void*)Hook_CreateFontW);
        if (g_origCreateFontW) {
            HookLog("Hooked CreateFontW IAT slot to use Cairo font");
        } else {
            HookLog("FAILED to hook CreateFontW IAT slot");
        }

        DWORD rvaCreateFontFace = GetRVA_FnPtr_CreateFontFace();
        if (rvaCreateFontFace != 0) {
            g_origCreateFontFace = (fnCreateFontFace)HookIatSlot(hNW, rvaCreateFontFace, (void*)Hook_CreateFontFace);
            if (g_origCreateFontFace) {
                HookLog("Hooked UFontManager::CreateFontFace IAT slot to use Cairo font for all UI/Tooltips/Chat/HTML");
            } else {
                HookLog("FAILED to hook UFontManager::CreateFontFace IAT slot");
            }
        }
    }

    HANDLE h = CreateThread(nullptr, 0, RuntimeStringReshaperThread,
                            (LPVOID)hNW, 0, nullptr);
    if (h) CloseHandle(h);

    g_NWindowHooked = true;
    HookLog("NWindow Arabic hooks installed successfully");
}