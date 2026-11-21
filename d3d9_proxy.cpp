// بروكسي Direct3D 9: يمرر صادرات المكتبة الأصلية ويضيف دعم الكتابة العربية.

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include "config.h"
#include "arabic_reshaper.h"
#include "arabic_nwindow_hook.h"

static HINSTANCE g_hInstance = NULL; // مقبض هذه المكتبة.

// النسخة الوحيدة من إعدادات التطبيق المستخدمة في جميع ملفات المشروع.
AppConfig g_Config = {
    DEFAULT_OPEN_ALL_CHAT,        // خيار توافق قديم لفتح الشات.
    DEFAULT_ARABIC_WINDOWS,       // تفعيل تعريب نوافذ اللعبة.
    DEFAULT_ARABIC_CHAT,          // تفعيل معالجة الكتابة العربية.
    DEFAULT_MAX_LINE_LENGTH,      // الحد الأقصى لطول رسالة الشات.
    DEFAULT_MAX_USERNAME_LENGTH,  // الحد الأقصى لطول اسم المستخدم.
    DEFAULT_MIRROR_BRACKETS,      // عكس الأقواس في النص العربي.
    DEFAULT_CACHE_SIZE,           // سعة ذاكرة نتائج التشكيل المؤقتة.
    DEFAULT_BEEP_THROTTLE_MS,     // الفاصل بين تنبيهات تجاوز الحد.
    DEFAULT_USE_CUSTOM_FONT,      // تفعيل الخط المخصص (Cairo)
    DEFAULT_CUSTOM_FONT_NAME      // اسم الخط المخصص
};

// يضيف رسالة إلى السجل مع الوقت ومعرّف مسار التنفيذ.
static void WriteLog(const std::string& msg) {
    if (IsShuttingDown()) return;

    SYSTEMTIME st;
    GetLocalTime(&st);
    char timeBuf[48];
    sprintf_s(timeBuf, "[%02d:%02d:%02d.%03d][TID:%lu] ",
              st.wHour, st.wMinute, st.wSecond, st.wMilliseconds,
              GetCurrentThreadId());

    wchar_t logPath[MAX_PATH] = { 0 };
    if (g_hInstance) {
        GetModuleFileNameW(g_hInstance, logPath, MAX_PATH);
        wchar_t* p = wcsrchr(logPath, L'\\');
        if (p) *(p + 1) = L'\0';
        wcscat_s(logPath, L"l2arabic.log");
    } else {
        wcscpy_s(logPath, L"l2arabic.log");
    }

    FILE* fp = nullptr;
    if (_wfopen_s(&fp, logPath, L"a, ccs=UTF-8") == 0 && fp) {
        fwprintf(fp, L"%S%S\n", timeBuf, msg.c_str());
        fclose(fp);
    }
}

// أسماء الصادرات المطلوب توفرها في مكتبة Direct3D الأصلية.
static const char* EXPECTED_D3D9_EXPORTS[] = {
    "Direct3DCreate9",
    "Direct3DCreate9Ex",
    "Direct3DShaderValidatorCreate9",
    "PSGPError",
    "PSGPSampleTexture",
    "D3DPERF_BeginEvent",
    "D3DPERF_EndEvent",
    "D3DPERF_GetStatus",
    "D3DPERF_QueryRepeatFrame",
    "D3DPERF_SetMarker",
    "D3DPERF_SetOptions",
    "D3DPERF_SetRegion",
    "DebugSetLevel",
    "DebugSetMute"
};

static const int EXPECTED_D3D9_EXPORTS_COUNT =
    sizeof(EXPECTED_D3D9_EXPORTS) / sizeof(EXPECTED_D3D9_EXPORTS[0]);

static std::string GetRealD3D9Path() {
    char sysDir[MAX_PATH] = { 0 };
    UINT len = GetSystemDirectoryA(sysDir, MAX_PATH);
    if (len == 0 || len >= MAX_PATH) {
        return "d3d9.dll";
    }
    std::string path(sysDir);
    if (!path.empty() && path.back() != '\\') path += "\\";
    path += "d3d9.dll";
    return path;
}

static void VerifyExports() {
    std::string realPath = GetRealD3D9Path();
    WriteLog("[L2Arabic] Verifying exports from: " + realPath);

    HMODULE hReal = LoadLibraryA(realPath.c_str());
    if (!hReal) {
        WriteLog("[L2Arabic] WARNING: Cannot load real d3d9.dll for verification");
        return;
    }

    int missing = 0;
    for (int i = 0; i < EXPECTED_D3D9_EXPORTS_COUNT; ++i) {
        if (!GetProcAddress(hReal, EXPECTED_D3D9_EXPORTS[i])) {
            missing++;
            WriteLog("[L2Arabic] WARNING: missing export - " +
                     std::string(EXPECTED_D3D9_EXPORTS[i]));
        }
    }

    if (missing == 0) {
        WriteLog("[L2Arabic] All " +
                 std::to_string(EXPECTED_D3D9_EXPORTS_COUNT) +
                 " exports verified successfully");
    } else {
        WriteLog("[L2Arabic] WARNING: " + std::to_string(missing) +
                 " exports missing from real d3d9.dll!");
    }

    FreeLibrary(hReal);
}

// أنواع دوال Direct3D التي يستدعيها البروكسي.
typedef void*   (WINAPI *tDirect3DCreate9)(UINT SDKVersion);
typedef HRESULT (WINAPI *tDirect3DCreate9Ex)(UINT SDKVersion, void** ppD3D);

static HMODULE           g_hRealD3D9           = NULL;
static tDirect3DCreate9   g_pfnDirect3DCreate9   = NULL;
static tDirect3DCreate9Ex g_pfnDirect3DCreate9Ex = NULL;
static FARPROC           g_pfnOtherD3D[12]     = { 0 };

static void LoadRealD3D9() {
    if (g_hRealD3D9) return;

    std::string realPath = GetRealD3D9Path();
    g_hRealD3D9 = LoadLibraryA(realPath.c_str());

    if (g_hRealD3D9) {
        WriteLog("[L2Arabic] Real d3d9.dll loaded from: " + realPath);

        g_pfnDirect3DCreate9   = (tDirect3DCreate9)
            GetProcAddress(g_hRealD3D9, "Direct3DCreate9");
        g_pfnDirect3DCreate9Ex = (tDirect3DCreate9Ex)
            GetProcAddress(g_hRealD3D9, "Direct3DCreate9Ex");

        g_pfnOtherD3D[0]  = GetProcAddress(g_hRealD3D9, "Direct3DShaderValidatorCreate9");
        g_pfnOtherD3D[1]  = GetProcAddress(g_hRealD3D9, "PSGPError");
        g_pfnOtherD3D[2]  = GetProcAddress(g_hRealD3D9, "PSGPSampleTexture");
        g_pfnOtherD3D[3]  = GetProcAddress(g_hRealD3D9, "D3DPERF_BeginEvent");
        g_pfnOtherD3D[4]  = GetProcAddress(g_hRealD3D9, "D3DPERF_EndEvent");
        g_pfnOtherD3D[5]  = GetProcAddress(g_hRealD3D9, "D3DPERF_GetStatus");
        g_pfnOtherD3D[6]  = GetProcAddress(g_hRealD3D9, "D3DPERF_QueryRepeatFrame");
        g_pfnOtherD3D[7]  = GetProcAddress(g_hRealD3D9, "D3DPERF_SetMarker");
        g_pfnOtherD3D[8]  = GetProcAddress(g_hRealD3D9, "D3DPERF_SetOptions");
        g_pfnOtherD3D[9]  = GetProcAddress(g_hRealD3D9, "D3DPERF_SetRegion");
        g_pfnOtherD3D[10] = GetProcAddress(g_hRealD3D9, "DebugSetLevel");
        g_pfnOtherD3D[11] = GetProcAddress(g_hRealD3D9, "DebugSetMute");
    } else {
        WriteLog("[L2Arabic] ERROR: Failed to load real d3d9.dll from " + realPath);
    }
}

// نافذة الترحيب التي تظهر عند بدء تشغيل Direct3D.
#include "Lineage2WelcomeDialog.h"

// نقاط التصدير البديلة التي تحيل الاستدعاءات إلى Direct3D الأصلي.
extern "C" {
    void* WINAPI Direct3DCreate9_Proxy(UINT SDKVersion) {
        EnsureWelcomeDialog();
        LoadRealD3D9();
        return g_pfnDirect3DCreate9 ? g_pfnDirect3DCreate9(SDKVersion) : NULL;
    }

    HRESULT WINAPI Direct3DCreate9Ex_Proxy(UINT SDKVersion, void** ppD3D) {
        EnsureWelcomeDialog();
        LoadRealD3D9();
        return g_pfnDirect3DCreate9Ex
            ? g_pfnDirect3DCreate9Ex(SDKVersion, ppD3D) : E_FAIL;
    }

    void WINAPI Direct3DShaderValidatorCreate9_Proxy() { LoadRealD3D9(); if(g_pfnOtherD3D[0])((void(*)())g_pfnOtherD3D[0])(); }
    void WINAPI PSGPError_Proxy()                      { LoadRealD3D9(); if(g_pfnOtherD3D[1])((void(*)())g_pfnOtherD3D[1])(); }
    void WINAPI PSGPSampleTexture_Proxy()              { LoadRealD3D9(); if(g_pfnOtherD3D[2])((void(*)())g_pfnOtherD3D[2])(); }
    void WINAPI D3DPERF_BeginEvent_Proxy()             { LoadRealD3D9(); if(g_pfnOtherD3D[3])((void(*)())g_pfnOtherD3D[3])(); }
    void WINAPI D3DPERF_EndEvent_Proxy()               { LoadRealD3D9(); if(g_pfnOtherD3D[4])((void(*)())g_pfnOtherD3D[4])(); }
    void WINAPI D3DPERF_GetStatus_Proxy()              { LoadRealD3D9(); if(g_pfnOtherD3D[5])((void(*)())g_pfnOtherD3D[5])(); }
    void WINAPI D3DPERF_QueryRepeatFrame_Proxy()       { LoadRealD3D9(); if(g_pfnOtherD3D[6])((void(*)())g_pfnOtherD3D[6])(); }
    void WINAPI D3DPERF_SetMarker_Proxy()              { LoadRealD3D9(); if(g_pfnOtherD3D[7])((void(*)())g_pfnOtherD3D[7])(); }
    void WINAPI D3DPERF_SetOptions_Proxy()             { LoadRealD3D9(); if(g_pfnOtherD3D[8])((void(*)())g_pfnOtherD3D[8])(); }
    void WINAPI D3DPERF_SetRegion_Proxy()              { LoadRealD3D9(); if(g_pfnOtherD3D[9])((void(*)())g_pfnOtherD3D[9])(); }
    void WINAPI DebugSetLevel_Proxy()                  { LoadRealD3D9(); if(g_pfnOtherD3D[10])((void(*)())g_pfnOtherD3D[10])(); }
    void WINAPI DebugSetMute_Proxy()                   { LoadRealD3D9(); if(g_pfnOtherD3D[11])((void(*)())g_pfnOtherD3D[11])(); }
}

// يصدر تنبيهاً صوتياً عند بلوغ حد الإدخال مع تحديد تكراره.
static DWORD g_lastBeepTime = 0; // وقت آخر تنبيه.

static void ThrottledBeep() {
    DWORD now = GetTickCount(); // الوقت الحالي بالميلي ثانية.
    DWORD elapsed = now - g_lastBeepTime; // الوقت المنقضي منذ التنبيه السابق.
    // لا يصدر تنبيه جديد إلا بعد انقضاء المهلة المحددة.
    if (g_lastBeepTime == 0 || elapsed >= (DWORD)g_Config.BeepThrottleMs) {
        g_lastBeepTime = now; // حفظ وقت التنبيه الحالي.
        MessageBeep(MB_ICONWARNING); // نغمة التحذير القياسية.
        MessageBeep(0xFFFFFFFF); // نغمة النظام الاحتياطية.
    }
}

// حالة النافذة والكتابة العربية الحالية.
static WNDPROC         g_oWndProc         = NULL;  // معالج النافذة الأصلي.
static HWND            g_hGameWnd         = NULL;  // نافذة اللعبة.
static HHOOK           g_hHook            = NULL;  // خطاف رسائل واجهة المستخدم.
static std::wstring    s_arabicWord;               // النص بترتيب إدخاله.
static size_t          s_visualCaret      = 0;     // موضع المؤشر في النص المعروض.
static bool            s_cursorMoved      = false; // يحدد ما إذا تحرك المؤشر عن موضع الكتابة.
// يزداد العداد أثناء إعادة الرسم لمنع معالجة رسائل الإدخال الناتجة عنها.
// العداد يدعم تداخل الاستدعاءات، خلافاً لقيمة منطقية بسيطة.
static volatile LONG   s_internalDepth   = 0;


// يزامن معالجة الرسائل حتى لا تتداخل عمليات تعديل النص.
static CRITICAL_SECTION g_cs;              // قفل مزامنة الرسائل.
static bool             g_csInit = false;  // حالة تهيئة القفل.

// يهيئ القفل عند أول استخدام.
static void EnsureCSInit() {
    if (!g_csInit) {
        InitializeCriticalSection(&g_cs); // إنشاء كائن القفل.
        g_csInit = true;
    }
}

// يضمن أخذ القفل عند الدخول إلى النطاق وتحريره عند الخروج منه.
struct CSGuard {
    CSGuard()  { EnsureCSInit(); EnterCriticalSection(&g_cs); } // أخذ القفل.
    ~CSGuard() { LeaveCriticalSection(&g_cs); }                 // تحرير القفل.
    CSGuard(const CSGuard&) = delete;
    CSGuard& operator=(const CSGuard&) = delete;
};

// بيانات الخطافات ومؤشر صندوق التحرير النشط في NWindow.dll.
// يلتقط الخطاف كائن NCEditBox لاستخدامه في تحديث النص والمؤشر.
static volatile DWORD s_pNCEditBox = 0;
static size_t s_untrackedInputLength = 0;
static bool s_untrackedInputIsLogin = true;
static int s_untrackedInputClickY = -1;

// مدخلا الخطاف يحفظان مؤشر الكائن من ECX ثم ينفذان التعليمات الأصلية.
struct NCEditBoxOnCharHook {
    BYTE* target;
    BYTE original[5];
};
static NCEditBoxOnCharHook g_onCharHooks[2] = {};
static size_t g_onCharHookCount = 0;
static void* g_pOnCharContinuation0 = nullptr;
static void* g_pOnCharContinuation1 = nullptr;
struct ArabicWhitelistPatch {
    BYTE* address;
    BYTE original[9];
};
static ArabicWhitelistPatch g_arabicWhitelistPatches[8] = {};
static size_t g_arabicWhitelistPatchCount = 0;

static void __declspec(naked) Hook_NCEditBox_OnChar_Direct0()
{
    __asm {
        mov dword ptr [s_pNCEditBox], ecx
        push ebp
        mov ebp, esp
        push -1
        jmp dword ptr [g_pOnCharContinuation0]
    }
}

static void __declspec(naked) Hook_NCEditBox_OnChar_Direct1()
{
    __asm {
        mov dword ptr [s_pNCEditBox], ecx
        push ebp
        mov ebp, esp
        push -1
        jmp dword ptr [g_pOnCharContinuation1]
    }
}

// قراءة موضع المؤشر من حقول NCEditBox بعد التحقق من صلاحية الذاكرة.
static int GetNCEditBoxCursorPos()
{
    DWORD pBox = (DWORD)InterlockedCompareExchange((volatile LONG*)&s_pNCEditBox, 0, 0);
    if (!pBox) return -1;
    int* pCursor = (int*)(pBox + 0x28c);
    MEMORY_BASIC_INFORMATION mbi;
    if (VirtualQuery(pCursor, &mbi, sizeof(mbi)) != 0 &&
        mbi.State == MEM_COMMIT &&
        !(mbi.Protect & (PAGE_NOACCESS | PAGE_GUARD)))
    {
        return *pCursor;
    }
    return -1;
}

struct NCEditBoxLimitInfo {
    bool isChatBox;
    bool isLoginBox;
    size_t textLength;
    size_t maxLength;
};


static bool GetNCEditBoxLimitInfo(NCEditBoxLimitInfo& info)
{
    DWORD pBox = (DWORD)InterlockedCompareExchange(
        (volatile LONG*)&s_pNCEditBox, 0, 0);
    if (!pBox) return false;

    int* pIsChat = (int*)(pBox + 0x408);
    wchar_t** ppData = (wchar_t**)(pBox + 0x240);
    int* pNum = (int*)(pBox + 0x244);
    int* pMax = (int*)(pBox + 0x248);

    if (!IsValidReadablePtr(pIsChat, sizeof(*pIsChat)) ||
        !IsValidReadablePtr(ppData, sizeof(*ppData)) ||
        !IsValidReadablePtr(pNum, sizeof(*pNum)) ||
        !IsValidReadablePtr(pMax, sizeof(*pMax)))
        return false;

    // في EU 542 تشير قيمة هذا الحقل إلى صناديق الدخول، لا صندوق الشات.
    if (*pIsChat != 0 && *pIsChat != 1) return false;
    info.isChatBox = IsChatEditBoxFlag(*pIsChat, IsEssenceP520NWindow());
    info.isLoginBox = !info.isChatBox;

    int num = *pNum;
    int capacity = *pMax;
    wchar_t* data = *ppData;
    if (num < 0 || capacity < num || capacity > 100000)
        return false;

    if (!data) {
        if (num != 0 || capacity != 0) return false;
        info.textLength = 0;
        info.maxLength = GetInputCharacterLimit(info.isLoginBox, g_Config);
        return true;
    }

    if (num < 1 ||
        !IsValidReadablePtr(data, (size_t)num * sizeof(wchar_t)))
        return false;

    info.textLength = (size_t)(num - 1);
    info.maxLength = GetInputCharacterLimit(info.isLoginBox, g_Config);
    return true;
}

static void SetNCEditBoxCursorPos(int pos)
{
    DWORD pBox = (DWORD)InterlockedCompareExchange((volatile LONG*)&s_pNCEditBox, 0, 0);
    if (!pBox) return;
    int* pCursor = (int*)(pBox + 0x28c);
    int* pCursor2 = (int*)(pBox + 0x290);
    MEMORY_BASIC_INFORMATION mbi;
    if (VirtualQuery(pCursor, &mbi, sizeof(mbi)) != 0 &&
        mbi.State == MEM_COMMIT &&
        !(mbi.Protect & (PAGE_NOACCESS | PAGE_GUARD)))
    {
        DWORD oldCur;
        if (VirtualProtect(pCursor, sizeof(int) * 2, PAGE_EXECUTE_READWRITE, &oldCur)) {
            *pCursor = pos;
            *pCursor2 = pos;
            VirtualProtect(pCursor, sizeof(int) * 2, oldCur, &oldCur);
        }
    }
}

// يحدّث النص والمؤشر مباشرة في FString التابعة لكائن NCEditBox.
static bool WriteDirectToNCEditBox(const std::wstring& text, size_t caretPos)
{
    DWORD pBox = (DWORD)InterlockedCompareExchange(
        (volatile LONG*)&s_pNCEditBox, 0, 0);
    if (!pBox) {
        WriteLog("[WriteDirect] FAIL: s_pNCEditBox=0 (vtable hook not fired yet)");
        return false;
    }

    char dbg[128];
    sprintf_s(dbg, "[WriteDirect] pBox=0x%08X", pBox);
    WriteLog(dbg);

    // تحتوي FString هنا على مؤشر البيانات وعدد العناصر والسعة.
    wchar_t** ppData = (wchar_t**)(pBox + 0x240);
    int*      pNum   = (int*)    (pBox + 0x244);
    int*      pMax   = (int*)    (pBox + 0x248);

    // تحقق من صلاحية الوصول إلى حقول النص.
    MEMORY_BASIC_INFORMATION mbi;
    if (VirtualQuery(ppData, &mbi, sizeof(mbi)) == 0) {
        WriteLog("[WriteDirect] FAIL: VirtualQuery(ppData) == 0");
        return false;
    }
    if (mbi.State != MEM_COMMIT) {
        WriteLog("[WriteDirect] FAIL: ppData not committed");
        return false;
    }
    if (mbi.Protect & (PAGE_NOACCESS | PAGE_GUARD)) {
        WriteLog("[WriteDirect] FAIL: ppData protected");
        return false;
    }

    wchar_t* pData = *ppData;
    int curMax = (IsValidReadablePtr(pMax, sizeof(int))) ? *pMax : 0;
    int curNum = (IsValidReadablePtr(pNum, sizeof(int))) ? *pNum : 0;
    int writeLen = (int)text.size();

    sprintf_s(dbg, "[WriteDirect] pData=0x%p Num=%d Max=%d writeLen=%d", pData, curNum, curMax, writeLen);
    WriteLog(dbg);

    // إذا كانت FString فارغة أو السعة لا تكفي، نستخدم مشغل الإسناد الأصلي لإعادة التخصيص
    if (!pData || curMax <= writeLen) {
        if (g_origFStringAssignW) {
            WriteLog("[WriteDirect] Allocating FString via g_origFStringAssignW");
            g_origFStringAssignW((void*)(pBox + 0x240), text.c_str());
        } else {
            WriteLog("[WriteDirect] FAIL: pData==NULL and g_origFStringAssignW is null");
            return false;
        }
    } else {
        DWORD old;
        if (!VirtualProtect(pData, curMax * sizeof(wchar_t),
                            PAGE_EXECUTE_READWRITE, &old)) {
            WriteLog("[WriteDirect] FAIL: VirtualProtect failed");
            return false;
        }

        if (writeLen > 0)
            memcpy(pData, text.data(), writeLen * sizeof(wchar_t));
        pData[writeLen] = L'\0';
        *pNum = writeLen + 1; // يتضمن Num محرف الإنهاء.

        VirtualProtect(pData, curMax * sizeof(wchar_t), old, &old);
    }

    // حدّث حقلي موضع المؤشر داخل كائن NCEditBox.
    int* pCursorPos = (int*)(pBox + 0x28c);
    int* pCursorPos2 = (int*)(pBox + 0x290);
    MEMORY_BASIC_INFORMATION mbiCursor;
    if (VirtualQuery(pCursorPos, &mbiCursor, sizeof(mbiCursor)) != 0 &&
        mbiCursor.State == MEM_COMMIT &&
        !(mbiCursor.Protect & (PAGE_NOACCESS | PAGE_GUARD)))
    {
        DWORD oldCur;
        if (VirtualProtect(pCursorPos, sizeof(int) * 2, PAGE_EXECUTE_READWRITE, &oldCur)) {
            int pos = (int)caretPos;
            if (pos < 0) pos = 0;
            if (pos > writeLen) pos = writeLen;
            *pCursorPos = pos;
            *pCursorPos2 = pos;
            VirtualProtect(pCursorPos, sizeof(int) * 2, oldCur, &oldCur);
        }
    }

    WriteLog("[WriteDirect] SUCCESS: wrote " + std::to_string(writeLen) + " chars");
    return true;
}


// يعيد عرض النص عبر محاكاة المفاتيح لإجبار NCEditBox على التحديث وإعادة الرسم.
static void DoRedraw(HWND hWnd, const std::wstring& prev,
                     const std::wstring& next, size_t targetCaret)
{
    InterlockedIncrement(&s_internalDepth);

    // استخدم دالة Clear في خانة vtable رقم 45 (الإزاحة 0xB4) إن كان الكائن صالحاً.
    DWORD pBox = (DWORD)InterlockedCompareExchange((volatile LONG*)&s_pNCEditBox, 0, 0);
    if (pBox && IsEssenceP520NWindow()) {
        MEMORY_BASIC_INFORMATION mbi;
        if (VirtualQuery((void*)pBox, &mbi, sizeof(mbi)) != 0 &&
            mbi.State == MEM_COMMIT &&
            !(mbi.Protect & (PAGE_NOACCESS | PAGE_GUARD)))
        {
            DWORD* pVTable = *(DWORD**)pBox;
            MEMORY_BASIC_INFORMATION mbiV;
            if (VirtualQuery(pVTable, &mbiV, sizeof(mbiV)) != 0 &&
                mbiV.State == MEM_COMMIT &&
                !(mbiV.Protect & (PAGE_NOACCESS | PAGE_GUARD)))
            {
                typedef void (__thiscall *fnNCEditBoxClear)(void* thisPtr);
                fnNCEditBoxClear pClear = (fnNCEditBoxClear)(pVTable[45]);
                if (pClear) {
                    pClear((void*)pBox);
                    WriteLog("[DoRedraw] Cleared NCEditBox via vtable[45]");
                }
            }
        }
    }

    size_t wipeCount = (prev.size() > next.size() ? prev.size() : next.size()) + 10;

    // انقل المؤشر إلى نهاية النص.
    CallWindowProcW(g_oWndProc, hWnd, WM_KEYDOWN, VK_END,   0x014F0001);
    CallWindowProcW(g_oWndProc, hWnd, WM_KEYUP,   VK_END,   0xC14F0001);
    CallWindowProcW(g_oWndProc, hWnd, WM_KEYDOWN, VK_RIGHT, 0x014D0001);
    CallWindowProcW(g_oWndProc, hWnd, WM_KEYUP,   VK_RIGHT, 0xC14D0001);

    // امسح النص بالرجوع للخلف.
    for (size_t i = 0; i < wipeCount; ++i) {
        CallWindowProcW(g_oWndProc, hWnd, WM_KEYDOWN, VK_BACK, 0x000E0001);
        CallWindowProcW(g_oWndProc, hWnd, WM_KEYUP,   VK_BACK, 0xC00E0001);
    }

    // انتقل إلى بداية النص.
    CallWindowProcW(g_oWndProc, hWnd, WM_KEYDOWN, VK_HOME,  0x01470001);
    CallWindowProcW(g_oWndProc, hWnd, WM_KEYUP,   VK_HOME,  0xC1470001);

    // امسح أي محرف متبقٍ بالتقدم إلى الأمام.
    for (size_t i = 0; i < wipeCount; ++i) {
        CallWindowProcW(g_oWndProc, hWnd, WM_KEYDOWN, VK_DELETE, 0x01530001);
        CallWindowProcW(g_oWndProc, hWnd, WM_KEYUP,   VK_DELETE, 0xC1530001);
    }

    // أدخل النص بعد تشكيله وعكس اتجاهه.
    for (wchar_t c : next) {
        CallWindowProcW(g_oWndProc, hWnd, WM_CHAR, (WPARAM)c, 1);
    }

    // أعد المؤشر إلى موضعه المستهدف.
    if (targetCaret == 0 || !s_cursorMoved) {
        CallWindowProcW(g_oWndProc, hWnd, WM_KEYDOWN, VK_HOME, 0x01470001);
        CallWindowProcW(g_oWndProc, hWnd, WM_KEYUP,   VK_HOME, 0xC1470001);
    } else if (targetCaret < next.size()) {
        CallWindowProcW(g_oWndProc, hWnd, WM_KEYDOWN, VK_HOME, 0x01470001);
        CallWindowProcW(g_oWndProc, hWnd, WM_KEYUP,   VK_HOME, 0xC1470001);
        for (size_t i = 0; i < targetCaret; ++i) {
            CallWindowProcW(g_oWndProc, hWnd, WM_KEYDOWN, VK_RIGHT, 0x014D0001);
            CallWindowProcW(g_oWndProc, hWnd, WM_KEYUP,   VK_RIGHT, 0xC14D0001);
        }
    }

    InvalidateRect(hWnd, NULL, FALSE);
    InterlockedDecrement(&s_internalDepth);
}





// يحذف المحرف السابق للمؤشر المرئي مع الحفاظ على بقية النص.
static void HandleBackspace(HWND hWnd)
{
    if (s_arabicWord.empty()) return;

    auto mapping = ArabicBiDi::ProcessArabicWithMap(s_arabicWord);
    std::wstring prev = mapping.visual;

    // اعتمد قيمة المؤشر المحلية؛ فالمحرك قد يعيد صفراً حتى عند وجود المؤشر في الوسط.
    int engineCaret = GetNCEditBoxCursorPos();
    size_t curVisualPos = s_visualCaret; // الموضع الذي تتبعه المعالجة المحلية.
    if (engineCaret > 0 && (size_t)engineCaret != s_visualCaret &&
        (size_t)engineCaret <= prev.size()) {
        // استخدم قراءة المحرك إذا كانت صالحة ومختلفة عن الموضع المحلي.
        curVisualPos = (size_t)engineCaret;
        s_visualCaret = curVisualPos;
    }

    char dbg[256];
    sprintf_s(dbg, "[L2Arabic][BACKSPACE] engineCaret=%d s_visualCaret=%zu curVisualPos=%zu moved=%d totalLog=%zu",
              engineCaret, s_visualCaret, curVisualPos, s_cursorMoved ? 1 : 0, s_arabicWord.size());
    WriteLog(dbg);

    // عند الحد الأيسر للنص المعروض لا يوجد محرف سابق لحذفه.
    if (curVisualPos == 0 && s_cursorMoved) {
        WriteLog("[L2Arabic][BACKSPACE] Leftmost boundary with moved cursor - no char behind caret");
        return;
    }

    // عند موضع الكتابة الافتراضي احذف أول محرف مرئي؛ وإلا احذف المحرف السابق للمؤشر.
    size_t delVisualIdx = (curVisualPos > 0) ? (curVisualPos - 1) : 0;

    sprintf_s(dbg, "[L2Arabic][BACKSPACE] delVisualIdx=%zu v2l.size=%zu",
              delVisualIdx, mapping.v2l.size());
    WriteLog(dbg);

    if (delVisualIdx < mapping.v2l.size()) {
        size_t logIdx = mapping.v2l[delVisualIdx];
        if (logIdx < s_arabicWord.size()) {
            wchar_t chDel = s_arabicWord[logIdx];
            char cDbg[160];
            sprintf_s(cDbg, "[L2Arabic][BACKSPACE] Erased logical[%zu] U+%04X at visual[%zu]",
                      logIdx, (unsigned)chDel, delVisualIdx);
            WriteLog(cDbg);
            s_arabicWord.erase(logIdx, 1);
        }
    } else {
        s_arabicWord.pop_back();
    }

    if (s_arabicWord.empty()) {
        s_visualCaret = 0;
        s_cursorMoved = false;
        DoRedraw(hWnd, prev, L"", 0);
        return;
    }

    std::wstring next = ArabicBiDi::ProcessArabic(s_arabicWord);
    size_t targetCaret = delVisualIdx;
    if (targetCaret > next.size()) targetCaret = next.size();
    s_visualCaret = targetCaret;
    s_cursorMoved = (s_visualCaret > 0);

    DoRedraw(hWnd, prev, next, targetCaret);
}

// يحذف المحرف الواقع أمام المؤشر المرئي.
static void HandleDelete(HWND hWnd)
{
    if (s_arabicWord.empty()) return;

    auto mapping = ArabicBiDi::ProcessArabicWithMap(s_arabicWord);
    std::wstring prev = mapping.visual;

    // فضّل الموضع المحلي لأن المحرك قد يعيد صفراً غير دقيق.
    int engineCaret = GetNCEditBoxCursorPos();
    size_t curVisualPos = s_visualCaret;
    if (engineCaret > 0 && (size_t)engineCaret != s_visualCaret &&
        (size_t)engineCaret <= prev.size()) {
        curVisualPos = (size_t)engineCaret;
        s_visualCaret = curVisualPos;
    }

    if (curVisualPos >= mapping.v2l.size()) {
        WriteLog("[L2Arabic][DELETE] Caret at rightmost end, nothing to delete");
        return;
    }

    size_t logIdx = mapping.v2l[curVisualPos];
    if (logIdx < s_arabicWord.size()) {
        wchar_t chDel = s_arabicWord[logIdx];
        char cDbg[160];
        sprintf_s(cDbg, "[L2Arabic][DELETE] Erased logical[%zu] U+%04X at visual[%zu]",
                  logIdx, (unsigned)chDel, curVisualPos);
        WriteLog(cDbg);
        s_arabicWord.erase(logIdx, 1);
    }

    if (s_arabicWord.empty()) {
        s_visualCaret = 0;
        s_cursorMoved = false;
        DoRedraw(hWnd, prev, L"", 0);
        return;
    }

    std::wstring next = ArabicBiDi::ProcessArabic(s_arabicWord);
    size_t targetCaret = curVisualPos;
    if (targetCaret > next.size()) targetCaret = next.size();
    s_visualCaret = targetCaret;
    s_cursorMoved = (s_visualCaret > 0);

    DoRedraw(hWnd, prev, next, targetCaret);
}

// يصفر حالة صندوق التحرير وتتبع الإدخال غير المرتبط بصندوق معروف.
static void ResetActiveEditBoxCapture()
{
    InterlockedExchange((volatile LONG*)&s_pNCEditBox, 0);
    s_untrackedInputLength = 0;
    s_untrackedInputIsLogin = true;
    s_untrackedInputClickY = -1;
}

static void HandleReset()
{
    s_arabicWord.clear(); // حذف النص المنطقي الحالي.
    s_cursorMoved = false; // إعادة تتبع المؤشر إلى وضعه الافتراضي.
    s_visualCaret = 0; // تصفير موضع المؤشر المرئي.
    LoadAppConfig(g_hInstance); // إعادة تحميل الإعدادات من ملف ini.
}

// يعالج رسائل الإدخال قبل تمريرها إلى معالج اللعبة الأصلي.
static LRESULT CALLBACK Hooked_WndProc(HWND hWnd, UINT uMsg,
                                       WPARAM wParam, LPARAM lParam)
{
    // تجاهل الرسائل التي ولّدتها إعادة الرسم لتفادي معالجتها كإدخال جديد.
    if (s_internalDepth > 0)
        return CallWindowProcW(g_oWndProc, hWnd, uMsg, wParam, lParam);


    // امنع تداخل تحديثات الحالة؛ يحرر CSGuard القفل تلقائياً عند الخروج.
    CSGuard lock;

    // سجّل تغيّر كائن NCEditBox النشط.
    static DWORD s_lastSeenNCEditBox = 0;
    DWORD curBox = (DWORD)InterlockedCompareExchange((volatile LONG*)&s_pNCEditBox, 0, 0);
    if (curBox != 0 && curBox != s_lastSeenNCEditBox) {
        char dbg[128];
        sprintf_s(dbg, "[L2Arabic][CAPTURE] Active NCEditBox: 0x%08X (prev: 0x%08X)", curBox, s_lastSeenNCEditBox);
        WriteLog(dbg);
        if (s_lastSeenNCEditBox != 0 && !s_arabicWord.empty()) {
            s_arabicWord.clear();
            s_visualCaret = 0;
            s_cursorMoved = false;
        }
        s_lastSeenNCEditBox = curBox;
    }

    if (uMsg == WM_CHAR &&
        ((wParam >= 0x20 && wParam != 0x7F) || wParam == L'\t')) {
        NCEditBoxLimitInfo boxInfo = {};
        const bool hasKnownBox = GetNCEditBoxLimitInfo(boxInfo);
        const bool isLoginBox = hasKnownBox ? boxInfo.isLoginBox : false;
        const size_t currentLength = hasKnownBox ? boxInfo.textLength : s_arabicWord.size();
        const size_t maxLength = hasKnownBox
            ? boxInfo.maxLength
            : GetInputCharacterLimit(isLoginBox, g_Config);
        if (!CanFitInputCharacters(currentLength, 1, maxLength)) {
            ThrottledBeep();
            WriteLog(std::string("[L2Arabic] ") +
                     (isLoginBox ? "Login" : "Chat") +
                     (hasKnownBox ? "" : " (uncaptured input)") +
                     " input limit reached (" +
                     std::to_string(maxLength) + " chars)");
            return 0;
        }
        s_untrackedInputIsLogin = isLoginBox;
        if (wParam >= 0x20 && wParam != 0x7F) {
            s_untrackedInputLength = currentLength + 1;
        }
    }

    const bool isCtrlVPaste =
        uMsg == WM_KEYDOWN &&
        (wParam == 'V' || wParam == 'v') &&
        (GetKeyState(VK_CONTROL) & 0x8000);
    if (!g_Config.ArabicChat && uMsg != WM_PASTE && !isCtrlVPaste)
        return CallWindowProcW(g_oWndProc, hWnd, uMsg, wParam, lParam);

    // تجاهل WM_IME_CHAR فقط لتفادي الحرف المكرر، واترك بقية رسائل IME للمحرك.
    if (uMsg == WM_IME_CHAR) {
        return 0; // منع معالجة نسخة الحرف الإضافية.
    }

    // سجّل رموز الأحرف الواردة للمساعدة في تشخيص الإدخال.
    if (uMsg == WM_CHAR) {
        wchar_t c = (wchar_t)wParam;
        char buf[80];
        if (c >= 0x0600 && c <= 0x06FF) {
            sprintf_s(buf, "[L2Arabic] WM_CHAR Arabic U+%04X", (unsigned)c);
            WriteLog(buf);
        } else if (c >= 0x20) {
            sprintf_s(buf, "[L2Arabic] WM_CHAR Char U+%04X", (unsigned)c);
            WriteLog(buf);
        }
    }

    // عالج مفاتيح التحكم وموقع المؤشر.
    if (uMsg == WM_KEYDOWN) {
        if ((wParam == 'A' || wParam == 'a') &&
            (GetKeyState(VK_CONTROL) & 0x8000)) {
            NCEditBoxLimitInfo boxInfo = {};
            if (!GetNCEditBoxLimitInfo(boxInfo)) {
                s_untrackedInputLength = 0;
            }
        }

        switch (wParam) {

            case VK_RETURN:
                {
                    NCEditBoxLimitInfo boxInfo = {};
                    const bool isUntrackedInput =
                        !GetNCEditBoxLimitInfo(boxInfo);
                    const bool nextIsLoginInput = isUntrackedInput
                        ? IsLoginInputAfterEnter(
                              s_untrackedInputIsLogin,
                              s_untrackedInputLength)
                        : true;
                    ResetActiveEditBoxCapture();
                    if (isUntrackedInput) {
                        s_untrackedInputIsLogin = nextIsLoginInput;
                    }
                }
                if (!s_arabicWord.empty()) {
                    WriteLog("[L2Arabic] Reset on Return");
                    HandleReset();
                }
                return CallWindowProcW(g_oWndProc, hWnd, uMsg, wParam, lParam);

            case VK_ESCAPE:
            case VK_TAB:
                ResetActiveEditBoxCapture();
                if (!s_arabicWord.empty()) {
                    WriteLog("[L2Arabic] Reset on Return/Escape/Tab");
                    HandleReset();
                }
                return CallWindowProcW(g_oWndProc, hWnd, uMsg, wParam, lParam);

            case VK_LEFT:
                {
                    LRESULT res = CallWindowProcW(g_oWndProc, hWnd, uMsg, wParam, lParam);
                    if (!s_arabicWord.empty()) {
                        // في النص المعروض، يحرك السهم الأيسر المؤشر نحو فهرس مرئي أصغر.
                        int realCaret = GetNCEditBoxCursorPos();
                        size_t visualLen = ArabicBiDi::ProcessArabic(s_arabicWord).size();
                        if (realCaret > 0 && (size_t)realCaret != s_visualCaret && (size_t)realCaret < visualLen) {
                            // اعتمد موضع المحرك إذا كان صالحاً.
                            s_visualCaret = (size_t)realCaret;
                        } else {
                            // عند غياب قراءة صالحة، حدّث الموضع محلياً.
                            if (s_visualCaret > 0) s_visualCaret--;
                        }
                        s_cursorMoved = (s_visualCaret > 0);
                        WriteLog("[L2Arabic] VK_LEFT: visualCaret=" + std::to_string(s_visualCaret));
                    }
                    return res;
                }

            case VK_RIGHT:
                {
                    LRESULT res = CallWindowProcW(g_oWndProc, hWnd, uMsg, wParam, lParam);
                    if (!s_arabicWord.empty()) {
                        // في النص المعروض، يحرك السهم الأيمن المؤشر نحو فهرس مرئي أكبر.
                        int realCaret = GetNCEditBoxCursorPos();
                        std::wstring curVis = ArabicBiDi::ProcessArabic(s_arabicWord);
                        size_t visualLen = curVis.size();
                        if (realCaret > 0 && (size_t)realCaret != s_visualCaret && (size_t)realCaret <= visualLen) {
                            // اعتمد موضع المحرك إذا كان صالحاً.
                            s_visualCaret = (size_t)realCaret;
                        } else {
                            // عند غياب قراءة صالحة، حدّث الموضع محلياً.
                            if (s_visualCaret < visualLen) s_visualCaret++;
                        }
                        s_cursorMoved = (s_visualCaret > 0);
                        WriteLog("[L2Arabic] VK_RIGHT: visualCaret=" + std::to_string(s_visualCaret));
                    }
                    return res;
                }

            case VK_HOME:
                {
                    LRESULT res = CallWindowProcW(g_oWndProc, hWnd, uMsg, wParam, lParam);
                    s_visualCaret = 0;
                    s_cursorMoved = false;
                    WriteLog("[L2Arabic] VK_HOME: visualCaret=0");
                    return res;
                }

            case VK_END:
                {
                    LRESULT res = CallWindowProcW(g_oWndProc, hWnd, uMsg, wParam, lParam);
                    if (!s_arabicWord.empty()) {
                        std::wstring curVis = ArabicBiDi::ProcessArabic(s_arabicWord);
                        s_visualCaret = curVis.size();
                        s_cursorMoved = (s_visualCaret > 0);
                        WriteLog("[L2Arabic] VK_END: visualCaret=" + std::to_string(s_visualCaret));
                    }
                    return res;
                }

            case VK_BACK:
                {
                    NCEditBoxLimitInfo boxInfo = {};
                    if (!GetNCEditBoxLimitInfo(boxInfo) &&
                        s_untrackedInputLength > 0) {
                        --s_untrackedInputLength;
                    }
                }
                if (!s_arabicWord.empty()) {
                    WriteLog("[L2Arabic] VK_BACK: handled in WM_KEYDOWN");
                    HandleBackspace(hWnd);
                    return 0;
                }
                return CallWindowProcW(g_oWndProc, hWnd, uMsg, wParam, lParam);

            case VK_DELETE:
                {
                    NCEditBoxLimitInfo boxInfo = {};
                    if (!GetNCEditBoxLimitInfo(boxInfo) &&
                        s_untrackedInputLength > 0) {
                        --s_untrackedInputLength;
                    }
                }
                if (!s_arabicWord.empty()) {
                    WriteLog("[L2Arabic] VK_DELETE: handled");
                    HandleDelete(hWnd);
                    return 0;
                }
                return CallWindowProcW(g_oWndProc, hWnd, uMsg, wParam, lParam);
        }
    }

    // أعد الحالة عند انتقال التركيز إلى نافذة أخرى.
    if (uMsg == WM_KILLFOCUS) {
        ResetActiveEditBoxCapture();
        HandleReset();
    }

    // حدّث حالة الحقل عند النقر، وحاذِ المؤشر إذا كان النص العربي قيد التحرير.
    if (uMsg == WM_LBUTTONUP || uMsg == WM_RBUTTONUP || uMsg == WM_LBUTTONDOWN) {
        if (s_arabicWord.empty()) {
            const int clickY = static_cast<short>(HIWORD(lParam));
            if (s_untrackedInputClickY < 0 ||
                clickY - s_untrackedInputClickY > 18 ||
                s_untrackedInputClickY - clickY > 18) {
                s_untrackedInputLength = 0;
            }
            s_untrackedInputClickY = clickY;
            InterlockedExchange((volatile LONG*)&s_pNCEditBox, 0);
        }
        LRESULT res = CallWindowProcW(g_oWndProc, hWnd, uMsg, wParam, lParam);
        if (!s_arabicWord.empty()) {
            int realCaret = GetNCEditBoxCursorPos();
            if (realCaret >= 0) {
                s_visualCaret = (size_t)realCaret;
                s_cursorMoved = (s_visualCaret > 0);
                WriteLog("[L2Arabic] Mouse click synced caret to: " + std::to_string(s_visualCaret));
            } else {
                s_cursorMoved = true;
            }
        }
        return res;
    }

    // اقرأ النص من الحافظة ومرره عبر معالجة الإدخال دون تعديل محتواها.
    if (uMsg == WM_PASTE || isCtrlVPaste)
    {
        // امنع تكرار اللصق الناتج عن استمرار ضغط المفتاح.
        if (isCtrlVPaste && (lParam & (1L << 30)) != 0) return 0;

        std::wstring clipboardText;
        bool hasUnicodeText = false;

        if (OpenClipboard(hWnd)) {
            HANDLE hData = GetClipboardData(CF_UNICODETEXT);
            if (hData) {
                SIZE_T dataSize = GlobalSize(hData);
                const wchar_t* pText =
                    dataSize >= sizeof(wchar_t)
                    ? (const wchar_t*)GlobalLock(hData)
                    : nullptr;

                if (pText) {
                    size_t maxChars = dataSize / sizeof(wchar_t);
                    size_t textLength = 0;
                    while (textLength < maxChars && pText[textLength] != L'\0') {
                        ++textLength;
                    }
                    clipboardText.assign(pText, textLength);
                    hasUnicodeText = true;
                    GlobalUnlock(hData);
                } else {
                    WriteLog("[L2Arabic] Ctrl+V: unable to lock Unicode clipboard data");
                }
            }
            if (!hasUnicodeText) {
                HANDLE hAnsiData = GetClipboardData(CF_TEXT);
                if (hAnsiData) {
                    SIZE_T dataSize = GlobalSize(hAnsiData);
                    const char* pText =
                        dataSize > 0
                        ? (const char*)GlobalLock(hAnsiData)
                        : nullptr;
                    if (pText) {
                        size_t textLength = 0;
                        while (textLength < dataSize && pText[textLength] != '\0') {
                            ++textLength;
                        }
                        if (textLength <= 0x7FFFFFFF) {
                            int wideLength = MultiByteToWideChar(
                                CP_ACP, 0, pText, (int)textLength, nullptr, 0);
                            if (wideLength > 0) {
                                clipboardText.resize((size_t)wideLength);
                                if (MultiByteToWideChar(
                                        CP_ACP, 0, pText, (int)textLength,
                                        &clipboardText[0], wideLength) == wideLength) {
                                    hasUnicodeText = true;
                                } else {
                                    clipboardText.clear();
                                }
                            }
                        }
                        GlobalUnlock(hAnsiData);
                    } else {
                        WriteLog("[L2Arabic] Ctrl+V: unable to lock ANSI clipboard data");
                    }
                }
            }
            CloseClipboard();
        } else {
            WriteLog("[L2Arabic] Paste blocked: unable to open clipboard");
            ThrottledBeep();
            return 0;
        }

        if (!hasUnicodeText) {
            WriteLog("[L2Arabic] Paste blocked: Unicode clipboard text unavailable");
            ThrottledBeep();
            return 0;
        }

        NCEditBoxLimitInfo boxInfo = {};
        const bool hasKnownBox = GetNCEditBoxLimitInfo(boxInfo);
        const bool isLoginBox =
            hasKnownBox ? boxInfo.isLoginBox : s_untrackedInputIsLogin;
        const size_t currentLength =
            hasKnownBox ? boxInfo.textLength : s_untrackedInputLength;
        const size_t maxLength = hasKnownBox
            ? boxInfo.maxLength
            : GetInputCharacterLimit(isLoginBox, g_Config);
        size_t pastedLength = 0;
        for (wchar_t ch : clipboardText) {
            if (ch != L'\r' && ch != L'\n') ++pastedLength;
        }
        if (!CanFitInputCharacters(currentLength, pastedLength, maxLength)) {
            ThrottledBeep();
            WriteLog(std::string("[L2Arabic] ") +
                     (isLoginBox ? "Login" : "Chat") +
                     (hasKnownBox ? "" : " (uncaptured input)") +
                     " paste exceeds the " +
                     std::to_string(maxLength) + "-character limit");
            return 0;
        }

        for (wchar_t ch : clipboardText) {
            if (ch == L'\r' || ch == L'\n') continue;
            Hooked_WndProc(hWnd, WM_CHAR, (WPARAM)ch, 0);
        }
        return 0;
    }

    // عالج الأحرف بعد التحقق من حد الحقل.
    if (uMsg == WM_CHAR)
    {
        wchar_t ch = (wchar_t)wParam;

        // لا تُبقِ حالة التشكيل بعد إنهاء الإدخال أو إلغائه.
        if (ch == 0x0D || ch == 0x0A || ch == 0x1B) {
            HandleReset();
            return CallWindowProcW(g_oWndProc, hWnd, uMsg, wParam, lParam);
        }

        // عولج Backspace مسبقاً عبر WM_KEYDOWN عند وجود نص عربي.
        if (ch == VK_BACK) {
            if (!s_arabicWord.empty()) {
                return 0; // امنع حذف المحرف مرة ثانية.
            }
            return CallWindowProcW(g_oWndProc, hWnd, uMsg, wParam, lParam);
        }

        // أدرج المحرف بعد التحقق من السعة وحساب موضعه المنطقي.
        auto InsertChar = [&](wchar_t c) -> void
        {
            NCEditBoxLimitInfo boxInfo = {};
            const bool hasKnownBox = GetNCEditBoxLimitInfo(boxInfo);
            const bool isLoginUsernameBox = hasKnownBox && boxInfo.isLoginBox;
            const size_t maxLimit = GetInputCharacterLimit(isLoginUsernameBox, g_Config);

            // ارفض المحرف إذا تجاوز طول النص أو محتوى الحقل الحد المحدد.
            if (!CanFitInputCharacters(s_arabicWord.size(), 1, maxLimit) ||
                (hasKnownBox &&
                 !CanFitInputCharacters(boxInfo.textLength, 1, boxInfo.maxLength))) {
                ThrottledBeep();
                std::string boxType = isLoginUsernameBox ? "Username" : "Chat";
                WriteLog("[L2Arabic] " + boxType + " limit reached (" + std::to_string(maxLimit) +
                         " chars), input stopped for U+" + std::to_string((unsigned)c));
                return;
            }

            int realCaret = GetNCEditBoxCursorPos();
            size_t curVisualPos = s_visualCaret;
            const size_t visualLength =
                ArabicBiDi::ProcessArabic(s_arabicWord).size();
            if (realCaret > 0 && (size_t)realCaret <= visualLength) {
                curVisualPos = (size_t)realCaret;
                s_visualCaret = curVisualPos;
            } else if (realCaret == 0 && !s_cursorMoved) {
                curVisualPos = 0;
            }

            bool isAtHead = (curVisualPos == 0 && !s_cursorMoved);

            struct InsertionPlan {
                size_t logicalPos;
                size_t nextVisualCaret;
            };

            auto ComputeInsertionPlan = [&](size_t curVis, bool atHead) -> InsertionPlan {
                if (s_arabicWord.empty() || atHead || curVis == 0) {
                    return { s_arabicWord.size(), 0 };
                }

                auto mapping = ArabicBiDi::ProcessArabicWithMap(s_arabicWord);
                size_t logicalPos =
                    ArabicBiDi::VisualCaretToLogicalInsertion(
                        s_arabicWord, mapping, curVis);
                size_t nextCaret = curVis >= mapping.v2l.size()
                    ? mapping.v2l.size() + 1
                    : curVis + 1;
                return { logicalPos, nextCaret };
            };

            InsertionPlan plan = ComputeInsertionPlan(curVisualPos, isAtHead);

            // احسب النص الذي سينتج عن الإدراج قبل تعديل الحالة.
            std::wstring testWord = s_arabicWord;
            if (plan.logicalPos >= testWord.size()) {
                testWord += c; // أضف المحرف إلى نهاية النص المنطقي.
            } else {
                testWord.insert(plan.logicalPos, 1, c);
            }

            // طبّق التشكيل والاتجاه لحساب النص المعروض.
            std::wstring testRendered = ArabicBiDi::ProcessArabic(testWord);

            // لا تسمح بتجاوز الحد، وأصدر تنبيهاً عند رفض الإدخال.
            if (testWord.size() > maxLimit || testRendered.size() > maxLimit) {
                ThrottledBeep(); // نبّه المستخدم إلى بلوغ الحد.
                std::string boxType = isLoginUsernameBox ? "Username" : "Chat";
                WriteLog("[L2Arabic] " + boxType + " limit reached (" + std::to_string(maxLimit) +
                         " chars), blocked input U+" + std::to_string((unsigned)c));
                return; // ارفض الإدراج قبل تغيير النص.
            }

            if (isAtHead) {
                // في وضع الكتابة المعتاد، أبقِ المؤشر عند موضع الإدراج العربي.
                std::wstring prev = ArabicBiDi::ProcessArabic(s_arabicWord);
                s_arabicWord += c;
                std::wstring next = ArabicBiDi::ProcessArabic(s_arabicWord);
                s_visualCaret = 0;
                s_cursorMoved = false;
                DoRedraw(hWnd, prev, next, 0);
            } else {
                // عند تحرير النص، حوّل الموضع المرئي إلى موضع منطقي قبل الإدراج.
                auto curMap = ArabicBiDi::ProcessArabicWithMap(s_arabicWord);
                std::wstring prev = curMap.visual;

                if (plan.logicalPos >= s_arabicWord.size()) {
                    s_arabicWord += c;
                } else {
                    s_arabicWord.insert(plan.logicalPos, 1, c);
                }

                std::wstring next = ArabicBiDi::ProcessArabic(s_arabicWord);
                size_t targetCaret = plan.nextVisualCaret;
                if (targetCaret > next.size()) targetCaret = next.size();
                s_visualCaret = targetCaret;
                s_cursorMoved = (s_visualCaret > 0);

                DoRedraw(hWnd, prev, next, targetCaret);
            }
        };

        // ابدأ معالجة النص عند وصول حرف عربي.
        if (ArabicBiDi::IsArabicChar(ch)) {
            InsertChar(ch);
            return 0;
        }
        // أدرج المسافات والرموز والأرقام والحروف اللاتينية في النص الجاري.
        else if (!s_arabicWord.empty() && (ch >= 0x20 || ch == L'\t')) {
            InsertChar(ch);
            return 0;
        }
        // أنهِ حالة النص عند وصول محرف تحكم غير مدعوم.
        else {
            HandleReset();
        }
    }

    return CallWindowProcW(g_oWndProc, hWnd, uMsg, wParam, lParam);
}

// يثبت معالج الرسائل من مسار واجهة المستخدم إذا لم يثبت بعد.
static LRESULT CALLBACK GetMsgProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode >= 0 && !g_oWndProc && g_hGameWnd && !IsShuttingDown()) {
        SetLastError(0);
        WNDPROC old = (WNDPROC)SetWindowLongPtrW(
            g_hGameWnd, GWLP_WNDPROC, (LONG_PTR)Hooked_WndProc);
        if (old) {
            g_oWndProc = old;
            WriteLog("[L2Arabic] WndProc hooked from UI thread");
        }
    }

    return CallNextHookEx(g_hHook, nCode, wParam, lParam);
}


// ----------------------------------------------------------------------------
// هيكل DetourHook لتثبيت هوكات inline بأمان
// ----------------------------------------------------------------------------
struct DetourHook {
    BYTE* target = nullptr;
    BYTE trampoline[16] = {};

    bool Install(void* pTarget, void* pHook) {
        if (!pTarget || !pHook) return false;
        target = (BYTE*)pTarget;

        DWORD oldProt;
        if (!VirtualProtect(trampoline, sizeof(trampoline), PAGE_EXECUTE_READWRITE, &oldProt))
            return false;

        memcpy(trampoline, target, 5);
        trampoline[5] = 0xE9;
        LONG jmpBack = (LONG)(target + 5) - (LONG)(&trampoline[5] + 5);
        memcpy(&trampoline[6], &jmpBack, sizeof(LONG));
        FlushInstructionCache(GetCurrentProcess(), trampoline, sizeof(trampoline));

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
// هوك execGetLanguage: تفعيل صندوق الإدخال الأصلي لشاشة إنشاء الشخصيات
// ----------------------------------------------------------------------------
typedef void (__thiscall* fnExecGetLanguage)(void* thisPtr, void* pStack, void* pResult);
static DetourHook g_hookGetLanguage;

typedef const wchar_t* (__thiscall* fnUObjectGetName)(const void* thisPtr);
static fnUObjectGetName g_fnUObjectGetName = nullptr;

static void __fastcall Hook_execGetLanguage(void* thisPtr, void* edxDummy, void* pStack, void* pResult) {
    fnExecGetLanguage orig = (fnExecGetLanguage)g_hookGetLanguage.GetOriginal();
    if (orig) orig(thisPtr, pStack, pResult);

    if (!pResult || !thisPtr) return;

    if (!g_fnUObjectGetName) {
        HMODULE hCore = GetModuleHandleA("Core.dll");
        if (hCore) {
            g_fnUObjectGetName = (fnUObjectGetName)GetProcAddress(hCore, "?GetName@UObject@@QBEPB_WXZ");
        }
    }

    if (g_fnUObjectGetName) {
        const wchar_t* name = g_fnUObjectGetName(thisPtr);
        if (name && wcsstr(name, L"CharacterCreate") != nullptr) {
            *(BYTE*)pResult = 4; // China language ID -> activates native CharacterCreateEditbox
            WriteLog("[L2Arabic] execGetLanguage: activated native editbox for Character Creation");
        }
    }
}

// ----------------------------------------------------------------------------
// هوك execCheckValidName: السماح بالأسماء العربية أثناء إنشاء الشخصيات
// ----------------------------------------------------------------------------
typedef void (__thiscall* fnExecCheckValidName)(void* thisPtr, void* pStack, void* pResult);
static DetourHook g_hookCheckValidName;

static void __fastcall Hook_execCheckValidName(void* thisPtr, void* edxDummy, void* pStack, void* pResult) {
    fnExecCheckValidName orig = (fnExecCheckValidName)g_hookCheckValidName.GetOriginal();
    if (orig) orig(thisPtr, pStack, pResult);

    if (!pResult) return;

    if (*(DWORD*)pResult == 0) {
        bool hasArabic = false;
        if (!s_arabicWord.empty()) {
            hasArabic = true;
        }

        DWORD pBox = (DWORD)InterlockedCompareExchange((volatile LONG*)&s_pNCEditBox, 0, 0);
        if (pBox) {
            wchar_t** ppData = (wchar_t**)(pBox + 0x240);
            int* pNum = (int*)(pBox + 0x244);
            if (IsValidReadablePtr(ppData, sizeof(wchar_t*)) && *ppData &&
                IsValidReadablePtr(pNum, sizeof(int)) && *pNum > 1) {
                if (HasRawArabic(*ppData, *pNum - 1) || HasPresentationForms(*ppData, *pNum - 1)) {
                    hasArabic = true;
                }
            }
        }

        if (hasArabic) {
            *(DWORD*)pResult = 1; // Mark name as valid
            WriteLog("[L2Arabic] execCheckValidName: approved Arabic character name");
        }
    }
}

// يرقّع نقاط الإدخال في NWindow.dll لدعم الأحرف العربية.
static bool g_bNWindowPatched = false;

static void PatchNWindowNativeFunctions()
{
    if (g_bNWindowPatched) return;

    HMODULE hNWindow = GetModuleHandleA("NWindow.dll");
    if (!hNWindow) return;

    InstallNWindowArabicHooks();

    // ------------------------------------------------------------------------
    // تثبيت هوكات واجهة إنشاء الشخصيات والتحقق من الأسماء
    // ------------------------------------------------------------------------
    FARPROC pfnGetLang = GetProcAddress(hNWindow,
        "?execGetLanguage@UUIScript@@QAEXAAUFFrame@@QAX@Z");
    if (pfnGetLang) {
        if (g_hookGetLanguage.Install(pfnGetLang, (void*)Hook_execGetLanguage)) {
            WriteLog("[L2Arabic] Installed detour for execGetLanguage");
        } else {
            WriteLog("[L2Arabic] FAILED to install detour for execGetLanguage");
        }
    }

    FARPROC pfnCheckValidName = GetProcAddress(hNWindow,
        "?execCheckValidName@UUIScript@@QAEXAAUFFrame@@QAX@Z");
    if (pfnCheckValidName) {
        if (g_hookCheckValidName.Install(pfnCheckValidName, (void*)Hook_execCheckValidName)) {
            WriteLog("[L2Arabic] Installed detour for execCheckValidName");
        } else {
            WriteLog("[L2Arabic] FAILED to install detour for execCheckValidName");
        }
    }

    if (g_Config.ArabicChat) {
        const unsigned char safeReturnStub[] = {
            0x8B,0x4C,0x24,0x04, 0xFF,0x41,0x0C, 0x8B,0x41,0x0C,
            0x80,0x38,0x42, 0x75,0x03, 0xFF,0x41,0x0C, 0xC2,0x08,0x00
        };

        FARPROC pfnSetChat = GetProcAddress(hNWindow,
            "?execSetAsChatEditBox@UEditBoxHandle@@QAEXAAUFFrame@@QAX@Z");
        if (pfnSetChat) {
            DWORD op;
            if (VirtualProtect(pfnSetChat, sizeof(safeReturnStub),
                               PAGE_EXECUTE_READWRITE, &op)) {
                memcpy(pfnSetChat, safeReturnStub, sizeof(safeReturnStub));
                VirtualProtect(pfnSetChat, sizeof(safeReturnStub), op, &op);
                FlushInstructionCache(GetCurrentProcess(), pfnSetChat,
                                      sizeof(safeReturnStub));
                WriteLog("[L2Arabic] Patched execSetAsChatEditBox");
            }
        }

        FARPROC pfnSetIME = GetProcAddress(hNWindow,
            "?execSetIME@UEditBoxHandle@@QAEXAAUFFrame@@QAX@Z");
        if (pfnSetIME) {
            DWORD op;
            if (VirtualProtect(pfnSetIME, sizeof(safeReturnStub),
                               PAGE_EXECUTE_READWRITE, &op)) {
                memcpy(pfnSetIME, safeReturnStub, sizeof(safeReturnStub));
                VirtualProtect(pfnSetIME, sizeof(safeReturnStub), op, &op);
                FlushInstructionCache(GetCurrentProcess(), pfnSetIME,
                                      sizeof(safeReturnStub));
                WriteLog("[L2Arabic] Patched execSetIME");
            }
        }

        // ابحث عن فحص Unicode داخل المقاطع التنفيذية بدلاً من الاعتماد على RVA ثابت.
        IMAGE_DOS_HEADER* dos = (IMAGE_DOS_HEADER*)hNWindow;
        IMAGE_NT_HEADERS32* nt = nullptr;
        BYTE* matches[8] = {};
        DWORD matchTargets[8] = {};
        size_t matchCount = 0;
        bool tooManyMatches = false;

        if (dos->e_magic == IMAGE_DOS_SIGNATURE) {
            nt = (IMAGE_NT_HEADERS32*)((BYTE*)hNWindow + dos->e_lfanew);
        }
        if (nt && nt->Signature == IMAGE_NT_SIGNATURE &&
            nt->FileHeader.Machine == IMAGE_FILE_MACHINE_I386 &&
            nt->OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR32_MAGIC)
        {
            IMAGE_SECTION_HEADER* section = IMAGE_FIRST_SECTION(nt);
            for (WORD i = 0; i < nt->FileHeader.NumberOfSections; ++i) {
                if ((section[i].Characteristics & IMAGE_SCN_MEM_EXECUTE) == 0)
                    continue;

                DWORD sectionStart = section[i].VirtualAddress;
                if (sectionStart >= nt->OptionalHeader.SizeOfImage)
                    continue;
                DWORD sectionSize = section[i].Misc.VirtualSize;
                if (sectionSize > nt->OptionalHeader.SizeOfImage - sectionStart)
                    sectionSize = nt->OptionalHeader.SizeOfImage - sectionStart;
                if (sectionSize < 18) continue;

                BYTE* code = (BYTE*)hNWindow + sectionStart;
                for (DWORD offset = 5; offset + 18 <= sectionSize; ++offset) {
                    BYTE* candidate = code + offset;
                    BYTE reg = candidate[1];
                    if (candidate[0] != 0x83 ||
                        (reg != 0xFE && reg != 0xFF) ||
                        candidate[2] != 0x7E ||
                        candidate[3] != 0x0F || candidate[4] != 0x86)
                        continue;

                    // تحقّق من تسلسل فحص ASCII وUnicode لتجنب مطابقة تعليمات غير مرتبطة.
                    if (candidate[-5] != 0x83 || candidate[-4] != reg ||
                        candidate[-3] != 0x20 ||
                        candidate[9] != 0x81 || candidate[10] != reg ||
                        candidate[11] != 0x01 || candidate[12] != 0xFF ||
                        candidate[13] != 0x00 || candidate[14] != 0x00)
                        continue;

                    LONG oldDisplacement = *(LONG*)(candidate + 5);
                    BYTE* branchTarget = candidate + 9 + oldDisplacement;
                    BYTE* sectionEnd = code + sectionSize;
                    if (branchTarget <= candidate + 9 || branchTarget >= sectionEnd)
                        continue;

                    if (matchCount == _countof(matches)) {
                        tooManyMatches = true;
                        continue;
                    }
                    matches[matchCount] = candidate;
                    matchTargets[matchCount] = (DWORD)(branchTarget - candidate);
                    ++matchCount;
                }
            }
        }

        if (matchCount > 0 && !tooManyMatches) {
            for (size_t i = 0; i < matchCount; ++i) {
                BYTE patch[9] = { 0xE9, 0, 0, 0, 0, 0x90, 0x90, 0x90, 0x90 };
                LONG jumpDisplacement = (LONG)matchTargets[i] - 5;
                memcpy(patch + 1, &jumpDisplacement, sizeof(jumpDisplacement));

                DWORD oldProtect;
                if (VirtualProtect(matches[i], sizeof(patch),
                                   PAGE_EXECUTE_READWRITE, &oldProtect)) {
                    ArabicWhitelistPatch& saved =
                        g_arabicWhitelistPatches[g_arabicWhitelistPatchCount++];
                    saved.address = matches[i];
                    memcpy(saved.original, matches[i], sizeof(saved.original));
                    memcpy(matches[i], patch, sizeof(patch));
                    FlushInstructionCache(GetCurrentProcess(), matches[i], sizeof(patch));
                    VirtualProtect(matches[i], sizeof(patch), oldProtect, &oldProtect);

                    char rvaText[16];
                    sprintf_s(rvaText, "0x%08X",
                              (DWORD)(matches[i] - (BYTE*)hNWindow));
                    WriteLog("[L2Arabic] Patched Unicode input whitelist at NWindow RVA " +
                             std::string(rvaText));
                } else {
                    WriteLog("[L2Arabic] ERROR: cannot change protection for Unicode whitelist");
                }
            }
        } else if (tooManyMatches) {
            WriteLog("[L2Arabic] Too many Unicode whitelist candidates; no whitelist bytes changed.");
        } else {
            WriteLog("[L2Arabic] Unicode input whitelist signature not found in executable sections.");
        }

        auto FindOnCharEntry = [&](BYTE* whitelistSite) -> BYTE* {
            IMAGE_SECTION_HEADER* sections = IMAGE_FIRST_SECTION(nt);
            for (WORD i = 0; i < nt->FileHeader.NumberOfSections; ++i) {
                if ((sections[i].Characteristics & IMAGE_SCN_MEM_EXECUTE) == 0 ||
                    sections[i].VirtualAddress >= nt->OptionalHeader.SizeOfImage)
                    continue;

                BYTE* sectionStart = (BYTE*)hNWindow + sections[i].VirtualAddress;
                DWORD sectionSize = sections[i].Misc.VirtualSize;
                DWORD imageRemaining =
                    nt->OptionalHeader.SizeOfImage - sections[i].VirtualAddress;
                if (sectionSize > imageRemaining) sectionSize = imageRemaining;
                BYTE* sectionEnd = sectionStart + sectionSize;
                if (whitelistSite < sectionStart || whitelistSite >= sectionEnd)
                    continue;

                BYTE* lowerBound = whitelistSite - sectionStart > 0x180
                    ? whitelistSite - 0x180
                    : sectionStart;
                BYTE* foundEntry = nullptr;
                size_t entryCount = 0;
                const BYTE entryPrefix[] = { 0x55,0x8B,0xEC,0x6A,0xFF,0x68 };
                const BYTE sehPrologue[] = {
                    0x64,0xA1,0,0,0,0,0x50,0x64,0x89,0x25,0,0,0,0
                };
                const BYTE savedFrame[] = { 0x53,0x56,0x57,0x89,0x65,0xF0 };

                for (BYTE* entry = lowerBound; entry + 0x40 < whitelistSite; ++entry) {
                    if (memcmp(entry, entryPrefix, sizeof(entryPrefix)) != 0 ||
                        memcmp(entry + 10, sehPrologue, sizeof(sehPrologue)) != 0)
                        continue;

                    for (size_t offset = 27; offset < 44; ++offset) {
                        BYTE* save = entry + offset;
                        if (save + sizeof(savedFrame) + 2 >= whitelistSite ||
                            memcmp(save, savedFrame, sizeof(savedFrame)) != 0)
                            continue;

                        BYTE* thisSave = save + sizeof(savedFrame);
                        if ((thisSave[0] == 0x8B && thisSave[1] == 0xD9) ||
                            (thisSave[0] == 0x89 && thisSave[1] == 0x4D)) {
                            foundEntry = entry;
                            ++entryCount;
                            break;
                        }
                    }
                }
                return entryCount == 1 ? foundEntry : nullptr;
            }
            return nullptr;
        };

        for (size_t i = 0; !tooManyMatches &&
             i < matchCount && i < _countof(matches); ++i) {
            BYTE* entry = FindOnCharEntry(matches[i]);
            if (!entry) {
                WriteLog("[L2Arabic] Could not uniquely identify edit-box handler for whitelist gate");
                continue;
            }

            bool alreadyHooked = false;
            for (size_t j = 0; j < g_onCharHookCount; ++j) {
                if (g_onCharHooks[j].target == entry) alreadyHooked = true;
            }
            if (alreadyHooked) continue;
            if (g_onCharHookCount >= _countof(g_onCharHooks)) {
                WriteLog("[L2Arabic] Edit-box handler hook capacity exceeded");
                break;
            }

            size_t hookIndex = g_onCharHookCount;
            void* hook = hookIndex == 0
                ? (void*)Hook_NCEditBox_OnChar_Direct0
                : (void*)Hook_NCEditBox_OnChar_Direct1;
            DWORD oldProtect;
            if (!VirtualProtect(entry, 5, PAGE_EXECUTE_READWRITE, &oldProtect)) {
                WriteLog("[L2Arabic] ERROR: cannot change edit-box handler protection");
                continue;
            }

            memcpy(g_onCharHooks[hookIndex].original, entry, 5);
            g_onCharHooks[hookIndex].target = entry;
            if (hookIndex == 0) g_pOnCharContinuation0 = entry + 5;
            else g_pOnCharContinuation1 = entry + 5;

            DWORD relativeJump = (DWORD)hook - ((DWORD)entry + 5);
            entry[0] = 0xE9;
            memcpy(entry + 1, &relativeJump, sizeof(relativeJump));
            FlushInstructionCache(GetCurrentProcess(), entry, 5);
            VirtualProtect(entry, 5, oldProtect, &oldProtect);
            ++g_onCharHookCount;

            char rvaText[16];
            sprintf_s(rvaText, "0x%08X", (DWORD)(entry - (BYTE*)hNWindow));
            WriteLog("[L2Arabic] Installed edit-box this-capture hook at NWindow RVA " +
                     std::string(rvaText));
        }
    } else {
        WriteLog("[L2Arabic] ArabicChat=false - Skipping chat patches");
    }

    g_bNWindowPatched = true;
}

// يبحث عن نافذة اللعبة ويركب خطافات رسائلها.
static DWORD WINAPI HookThread(LPVOID)
{
    DWORD pid = GetCurrentProcessId();
    WriteLog("[L2Arabic] HookThread started, PID=" + std::to_string(pid));

    while (!IsShuttingDown() && (!g_bNWindowPatched || !g_oWndProc)) {
        if (!g_bNWindowPatched) {
            PatchNWindowNativeFunctions();
        }

        if (!g_hGameWnd) {
            EnumWindows([](HWND hWnd, LPARAM lParam) -> BOOL {
                DWORD wPid = 0;
                GetWindowThreadProcessId(hWnd, &wPid);
                if (wPid == (DWORD)lParam && IsWindowVisible(hWnd)) {
                    char cls[256] = {};
                    GetClassNameA(hWnd, cls, sizeof(cls));

                    if (strcmp(cls, "#32770") == 0) return TRUE;

                    char title[256] = {};
                    GetWindowTextA(hWnd, title, sizeof(title));

                    if (_stricmp(cls, "L2UnrealWWindowsViewportWindow") == 0 ||
                        strstr(cls, "ViewportWindow") != nullptr ||
                        (strstr(cls, "Unreal") != nullptr && strcmp(cls, "#32770") != 0))
                    {
                        g_hGameWnd = hWnd;
                        WriteLog(std::string("[L2Arabic] Found game window: '") +
                                 title + "' cls='" + cls + "'");
                        return FALSE;
                    }
                }
                return TRUE;
            }, (LPARAM)pid);

            if (g_hGameWnd) {
                DWORD uiTid = GetWindowThreadProcessId(g_hGameWnd, NULL);
                WriteLog("[L2Arabic] UI Thread ID=" + std::to_string(uiTid));

                g_hHook = SetWindowsHookExW(WH_GETMESSAGE, GetMsgProc,
                                            g_hInstance, uiTid);
                WriteLog(g_hHook
                    ? "[L2Arabic] WH_GETMESSAGE hook installed"
                    : "[L2Arabic] WH_GETMESSAGE failed err=" +
                      std::to_string(GetLastError()));

                PostMessageW(g_hGameWnd, WM_NULL, 0, 0);

                SetLastError(0);
                WNDPROC old = (WNDPROC)SetWindowLongPtrW(
                    g_hGameWnd, GWLP_WNDPROC, (LONG_PTR)Hooked_WndProc);
                if (old) {
                    g_oWndProc = old;
                    WriteLog("[L2Arabic] Direct subclass succeeded");
                }
            }
        } else if (!g_oWndProc) {
            SetLastError(0);
            WNDPROC old = (WNDPROC)SetWindowLongPtrW(
                g_hGameWnd, GWLP_WNDPROC, (LONG_PTR)Hooked_WndProc);
            if (old) {
                g_oWndProc = old;
                WriteLog("[L2Arabic] Direct subclass retry succeeded");
            } else {
                PostMessageW(g_hGameWnd, WM_NULL, 0, 0);
            }
        }

        for (int i = 0; i < 4; ++i) {
            if (IsShuttingDown()) break;
            Sleep(50);
        }
    }

    if (IsShuttingDown()) {
        WriteLog("[L2Arabic] HookThread: shutdown signal - exiting");
    } else {
        WriteLog("[L2Arabic] HookThread completed: hooks active");
    }
    return 0;
}

// يهيئ البروكسي عند تحميله ويبدأ تنظيفه عند إزالته.
BOOL WINAPI DllMain(HINSTANCE hInst, DWORD reason, LPVOID)
{
    if (reason == DLL_PROCESS_ATTACH) {
        g_hInstance = hInst;
        DisableThreadLibraryCalls(hInst);
        WriteLog("[L2Arabic] DLL_PROCESS_ATTACH");

        LoadAppConfig(hInst); // اقرأ الإعدادات من الملف المجاور للمكتبة.
        WriteLog("[L2Arabic] Config: openAllchat=" +
                 std::string(g_Config.openAllchat ? "true" : "false") +
                 ", ArabicWindows=" +
                 std::string(g_Config.ArabicWindows ? "true" : "false") +
                 ", ArabicChat=" +
                 std::string(g_Config.ArabicChat ? "true" : "false") +
                 ", MaxChatLineLength=" +
                 std::to_string(g_Config.MaxChatLineLength));

        VerifyExports();
        LoadRealD3D9();

        CreateThread(NULL, 0, HookThread, NULL, 0, NULL);
    }
    else if (reason == DLL_PROCESS_DETACH) {
        SignalShutdown();
        WriteLog("[L2Arabic] DLL_PROCESS_DETACH - shutdown signaled");

        if (g_hHook) {
            UnhookWindowsHookEx(g_hHook);
            g_hHook = NULL;
        }

        if (g_hGameWnd && g_oWndProc) {
            SetWindowLongPtrW(g_hGameWnd, GWLP_WNDPROC,
                              (LONG_PTR)g_oWndProc);
            g_oWndProc = NULL;
        }

        for (size_t i = g_arabicWhitelistPatchCount; i > 0; --i) {
            ArabicWhitelistPatch& patch = g_arabicWhitelistPatches[i - 1];
            DWORD oldProtect;
            if (VirtualProtect(patch.address, sizeof(patch.original),
                               PAGE_EXECUTE_READWRITE, &oldProtect)) {
                memcpy(patch.address, patch.original, sizeof(patch.original));
                FlushInstructionCache(GetCurrentProcess(), patch.address,
                                      sizeof(patch.original));
                VirtualProtect(patch.address, sizeof(patch.original),
                               oldProtect, &oldProtect);
            }
        }
        g_arabicWhitelistPatchCount = 0;

        for (size_t i = g_onCharHookCount; i > 0; --i) {
            NCEditBoxOnCharHook& hook = g_onCharHooks[i - 1];
            DWORD oldProtect;
            if (VirtualProtect(hook.target, sizeof(hook.original),
                               PAGE_EXECUTE_READWRITE, &oldProtect)) {
                memcpy(hook.target, hook.original, sizeof(hook.original));
                FlushInstructionCache(GetCurrentProcess(), hook.target,
                                      sizeof(hook.original));
                VirtualProtect(hook.target, sizeof(hook.original),
                               oldProtect, &oldProtect);
            }
        }
        g_onCharHookCount = 0;

        ArabicBiDi::ReshapeCache::Instance().Clear();
        Sleep(200);
    }
    return TRUE;
}