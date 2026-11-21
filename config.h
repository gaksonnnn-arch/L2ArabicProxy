// ============================================================================
// تم الإنشاء والتطوير بواسطة: Mohamed Tarek
// All rights and development reserved to: Mohamed Tarek
// ============================================================================
// ملف الإعدادات: config.h
// يحتوي على خيارات التحكم في ميزات التعريب والشات المتاحة قبل البناء أو أثناء التشغيل
// ============================================================================

#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>

// ----------------------------------------------------------------------------
// إعدادات مسبقة قبل البناء (Compile-time Pre-Build Defaults)
// تم التطوير بواسطة: Mohamed Tarek
// ----------------------------------------------------------------------------
#define DEFAULT_OPEN_ALL_CHAT     false // افتراضياً: الالتزام بحد سطر الشات (45 حرفاً) مع التنبيه الصوتي
#define DEFAULT_ARABIC_WINDOWS    true  // ترجمة وتصحيح نصوص ملفات اللعبة ونوافذها
#define DEFAULT_ARABIC_CHAT       true  // تشغيل المحادثة والشات باللغة العربية
#define DEFAULT_MAX_LINE_LENGTH   45    // الحد الأقصى الافتراضي لعدد الحروف في سطر الشات
#define DEFAULT_MAX_USERNAME_LENGTH 25  // الحد الأقصى الافتراضي لعدد الحروف في مربع اسم المستخدم (25 حرفاً ثابتاً)
#define DEFAULT_MIRROR_BRACKETS   true  // عكس الأقواس لعرضها باتجاه صحيح داخل النص العربي
#define DEFAULT_CACHE_SIZE        2000  // حجم الذاكرة المؤقتة للتشكيل
#define DEFAULT_BEEP_THROTTLE_MS  400   // زمن الخمول بين كل تنبيه صوتي والآخر (بالميلي ثانية)
#define DEFAULT_USE_CUSTOM_FONT   true  // استخدام خط عربي مخصص (Cairo) بدلاً من خط النظام الافتراضي
#define DEFAULT_CUSTOM_FONT_NAME  L"Cairo" // اسم عائلة الخط المخصص

// ----------------------------------------------------------------------------
// بنية كائن الإعدادات الموحد للبرنامج
// ----------------------------------------------------------------------------
struct AppConfig {
    bool   openAllchat;         // إعداد قديم؛ لا يتجاوز الحد الصارم لعدد الحروف
    bool   ArabicWindows;       // تعريب نوافذ وقوائم اللعبة
    bool   ArabicChat;          // تفعيل الشات العربي
    int    MaxChatLineLength;   // الحد الأقصى لعدد حروف سطر الشات
    int    MaxUsernameLength;   // الحد الأقصى لعدد حروف مربع اسم المستخدم (25 حرفاً)
    bool   MirrorBrackets;      // عكس الأقواس
    int    MaxReshapeCacheSize; // حجم الكاش
    int    BeepThrottleMs;      // مهلة التنبيه الصوتي
    bool   UseCustomFont;       // تفعيل الخط المخصص (Cairo)
    std::wstring CustomFontName;// اسم الخط المخصص
};

inline size_t GetInputCharacterLimit(bool isLoginBox, const AppConfig& config) {
    const size_t hardLimit = isLoginBox
        ? DEFAULT_MAX_USERNAME_LENGTH
        : DEFAULT_MAX_LINE_LENGTH;
    const int configuredLimit = isLoginBox
        ? config.MaxUsernameLength
        : config.MaxChatLineLength;
    if (configuredLimit < 1) return hardLimit;
    return (size_t)configuredLimit < hardLimit
        ? (size_t)configuredLimit
        : hardLimit;
}

inline bool CanFitInputCharacters(
    size_t currentLength, size_t additionalLength, size_t limit) {
    return currentLength <= limit &&
        additionalLength <= limit - currentLength;
}

inline bool IsChatEditBoxFlag(int flag, bool essenceP520) {
    return essenceP520 ? (flag == 1) : (flag == 0);
}

inline bool IsLoginInputAfterEnter(
    bool isLoginInput, size_t currentInputLength) {
    return !isLoginInput || currentInputLength > 0;
}

// تعريف واحد مشترك بين كل ملفات المشروع — التعريف الفعلي في d3d9_proxy.cpp
// تم التطوير بواسطة: Mohamed Tarek
extern AppConfig g_Config;

// ----------------------------------------------------------------------------
// دالة تحويل النص إلى قيمة منطقية (Boolean)
// ----------------------------------------------------------------------------
inline bool ParseBool(const char* s, bool def) {
    if (!s) return def;
    if (_stricmp(s, "true") == 0) return true;
    if (_stricmp(s, "false") == 0) return false;
    if (strcmp(s, "1") == 0) return true;
    if (strcmp(s, "0") == 0) return false;
    return def;
}

// ----------------------------------------------------------------------------
// دالة تحويل النص إلى قيمة رقمية (Integer)
// ----------------------------------------------------------------------------
inline int ParseInt(const char* s, int def) {
    if (!s || !*s) return def;
    char* end = nullptr;
    long v = strtol(s, &end, 10);
    if (end == s) return def;
    if (v < 1) v = 1;
    if (v > 100000) v = 100000;
    return (int)v;
}

// ----------------------------------------------------------------------------
// دالة تحميل وتطبيق الإعدادات من ملف l2arabic.ini الموجود بجانب ملف المكتبة
// ----------------------------------------------------------------------------
inline void LoadAppConfig(HMODULE hModule = NULL) {
    wchar_t iniPathW[MAX_PATH] = { 0 };

    // 1. محاولة قراءة المسار من مجلد وجود المكتبة d3d9.dll مباشرة
    if (hModule) {
        GetModuleFileNameW(hModule, iniPathW, MAX_PATH);
        wchar_t* lastSlash = wcsrchr(iniPathW, L'\\');
        if (lastSlash) {
            *(lastSlash + 1) = L'\0';
            wcscat_s(iniPathW, L"l2arabic.ini");
        }
    }

    // 2. إذا لم يحدد مقبض المكتبة أو لم يتوفر المسار، نستخدم المجلد الحالي
    if (iniPathW[0] == L'\0' || GetFileAttributesW(iniPathW) == INVALID_FILE_ATTRIBUTES) {
        wchar_t curDir[MAX_PATH] = { 0 };
        GetCurrentDirectoryW(MAX_PATH, curDir);
        swprintf_s(iniPathW, L"%s\\l2arabic.ini", curDir);
    }

    DWORD fileAttr = GetFileAttributesW(iniPathW);
    if (fileAttr == INVALID_FILE_ATTRIBUTES) {
        // إنشاء ملف الإعدادات بالقيم الافتراضية عند عدم وجوده
        WritePrivateProfileStringW(L"Config", L"openAllchat",         DEFAULT_OPEN_ALL_CHAT ? L"true" : L"false", iniPathW);
        WritePrivateProfileStringW(L"Config", L"ArabicWindows",       DEFAULT_ARABIC_WINDOWS ? L"true" : L"false", iniPathW);
        WritePrivateProfileStringW(L"Config", L"ArabicChat",          DEFAULT_ARABIC_CHAT ? L"true" : L"false", iniPathW);
        WritePrivateProfileStringW(L"Config", L"MaxChatLineLength",   L"45", iniPathW);
        WritePrivateProfileStringW(L"Config", L"MaxUsernameLength",   L"25", iniPathW);
        WritePrivateProfileStringW(L"Config", L"MirrorBrackets",      L"true", iniPathW);
        WritePrivateProfileStringW(L"Config", L"MaxReshapeCacheSize", L"2000", iniPathW);
        WritePrivateProfileStringW(L"Config", L"BeepThrottleMs",      L"400", iniPathW);
        WritePrivateProfileStringW(L"Config", L"UseCustomFont",       DEFAULT_USE_CUSTOM_FONT ? L"true" : L"false", iniPathW);
        WritePrivateProfileStringW(L"Config", L"CustomFontName",      DEFAULT_CUSTOM_FONT_NAME, iniPathW);
    }

    wchar_t bufferW[64];
    char bufA[64];

    memset(bufferW, 0, sizeof(bufferW));
    GetPrivateProfileStringW(L"Config", L"openAllchat", DEFAULT_OPEN_ALL_CHAT ? L"true" : L"false", bufferW, _countof(bufferW), iniPathW);
    sprintf_s(bufA, "%S", bufferW);
    g_Config.openAllchat = ParseBool(bufA, DEFAULT_OPEN_ALL_CHAT);

    memset(bufferW, 0, sizeof(bufferW));
    GetPrivateProfileStringW(L"Config", L"ArabicWindows", DEFAULT_ARABIC_WINDOWS ? L"true" : L"false", bufferW, _countof(bufferW), iniPathW);
    sprintf_s(bufA, "%S", bufferW);
    g_Config.ArabicWindows = ParseBool(bufA, DEFAULT_ARABIC_WINDOWS);

    memset(bufferW, 0, sizeof(bufferW));
    GetPrivateProfileStringW(L"Config", L"ArabicChat", DEFAULT_ARABIC_CHAT ? L"true" : L"false", bufferW, _countof(bufferW), iniPathW);
    sprintf_s(bufA, "%S", bufferW);
    g_Config.ArabicChat = ParseBool(bufA, DEFAULT_ARABIC_CHAT);

    memset(bufferW, 0, sizeof(bufferW));
    GetPrivateProfileStringW(L"Config", L"MaxChatLineLength", L"45", bufferW, _countof(bufferW), iniPathW);
    sprintf_s(bufA, "%S", bufferW);
    g_Config.MaxChatLineLength = ParseInt(bufA, DEFAULT_MAX_LINE_LENGTH);

    memset(bufferW, 0, sizeof(bufferW));
    GetPrivateProfileStringW(L"Config", L"MaxUsernameLength", L"25", bufferW, _countof(bufferW), iniPathW);
    sprintf_s(bufA, "%S", bufferW);
    g_Config.MaxUsernameLength = ParseInt(bufA, DEFAULT_MAX_USERNAME_LENGTH);

    memset(bufferW, 0, sizeof(bufferW));
    GetPrivateProfileStringW(L"Config", L"MirrorBrackets", L"true", bufferW, _countof(bufferW), iniPathW);
    sprintf_s(bufA, "%S", bufferW);
    g_Config.MirrorBrackets = ParseBool(bufA, DEFAULT_MIRROR_BRACKETS);

    memset(bufferW, 0, sizeof(bufferW));
    GetPrivateProfileStringW(L"Config", L"MaxReshapeCacheSize", L"2000", bufferW, _countof(bufferW), iniPathW);
    sprintf_s(bufA, "%S", bufferW);
    g_Config.MaxReshapeCacheSize = ParseInt(bufA, DEFAULT_CACHE_SIZE);

    memset(bufferW, 0, sizeof(bufferW));
    GetPrivateProfileStringW(L"Config", L"BeepThrottleMs", L"400", bufferW, _countof(bufferW), iniPathW);
    sprintf_s(bufA, "%S", bufferW);
    g_Config.BeepThrottleMs = ParseInt(bufA, DEFAULT_BEEP_THROTTLE_MS);

    memset(bufferW, 0, sizeof(bufferW));
    GetPrivateProfileStringW(L"Config", L"UseCustomFont", DEFAULT_USE_CUSTOM_FONT ? L"true" : L"false", bufferW, _countof(bufferW), iniPathW);
    sprintf_s(bufA, "%S", bufferW);
    g_Config.UseCustomFont = ParseBool(bufA, DEFAULT_USE_CUSTOM_FONT);

    memset(bufferW, 0, sizeof(bufferW));
    GetPrivateProfileStringW(L"Config", L"CustomFontName", DEFAULT_CUSTOM_FONT_NAME, bufferW, _countof(bufferW), iniPathW);
    g_Config.CustomFontName = bufferW[0] ? bufferW : DEFAULT_CUSTOM_FONT_NAME;
}