# المرجع الهندسي الشامل والكامل لإعادة بناء مشروع L2ArabicProxy (Lineage II Arabic System)

---

## 1. السياق العام للمشروع والغرض الهندسي

مشروع **L2ArabicProxy** هو نظام متقدم للحقن البرمجي والهندسة العكسية تم تطويره خصيصاً للعبة **Lineage II** (يعمل بكفاءة على إصدارات **EU 542 - Samurai Crow** وإصدارات **Essence P520**، بنية العميل 32-bit x86 لمكتبات DirectX 9 ونظام Windows).

يقوم النظام باستبدال مكتبة `d3d9.dll` بمكتبة بروكسي مخصصة تُحمّل تلقائياً بواسطة اللعبة (`L2.exe`) لتنفيذ ثلاثة أهداف هندسية رئيسية:
1. **نظام الكتابة والإدخال الحي للشات ومربعات النصوص (RTL Live Input Engine):** تحويل صندوق الشات غير القياسي (`NCEditBox`) وحقول تسجيل الدخول وإنشاء الشخصيات لتقبل الكتابة باللغة العربية المتصلة والصحيحة من اليمين إلى اليسار في الوقت الفعلي دون انقطاع.
2. **نظام توحيد وتحديث الخطوط (Unified Dual Font Engine):** استبدال خطوط اللعبة بالكامل بخط عربي حديث فائق الدقة (**Cairo / Cairo-Bold**) عبر طبقتي المحرك المعزولتين: واجهات الفلاش (`Scaleform`) ومحرك اللعبة الأساسي (`Core Engine UI`).
3. **نظام تشكيل وعكس نصوص اللعبة في الذاكرة (Dynamic Text Reshaping Engine):** فحص واعتراض كافة نصوص الواجهة، التولتيبات (Tooltips)، وصف الأسلحة والدروع، الشات، نصوص الـ HTML، أسماء الخرائط والمناطق، لتظهر معربة ومنسقة دون حروف مقطعة أو معكوسة.

---

## 2. المعمارية الهندسية الشاملة وتدفق البيانات (System Architecture)

```
                    +------------------------------------------+
                    |           L2.exe (Process)               |
                    +------------------------------------------+
                                         |
                                         v
                         +-------------------------------+
                         |      d3d9.dll (Proxy DLL)     |
                         |   (Direct3D 9 Forwarding)     |
                         +-------------------------------+
                                  /            \
                                 /              \
                                v                v
   +-------------------------------+   +------------------------------------+
   |   HookThread & Subclassing    |   |     arabic_nwindow_hook.h          |
   |   (Window: ViewportWindow)    |   |     (NWindow.dll & Engine.dll)     |
   +-------------------------------+   +------------------------------------+
              |                                          |
              v                                          |
   +-------------------------------+                     v
   | NCEditBox VTable Hook (0x258) |   +------------------------------------+
   | Direct Memory Write (FString) |   | 1. Dual Font Hooks:                |
   | IME & Unicode Patch (0xaae074)|   |    - CreateFontW (Scaleform Flash) |
   | Fallback Input Engine         |   |    - CreateFontFace (Core Engine)  |
   +-------------------------------+   | 2. IAT & Canvas Hooks (UCanvas)    |
              \                        | 3. Zone & Map Hooks (FL2GameData)  |
               \                       | 4. Inline Detour Trampolines       |
                \                      | 5. RuntimeStringReshaperThread     |
                 \                     +------------------------------------+
                  \                                      |
                   \                                    /
                    v                                  v
              +---------------------------------------------+
              |              arabic_reshaper.h              |
              |   - Arabic BiDi & Ligatures (Lam-Alef)      |
              |   - Bracket Mirroring & Neutral Resolving   |
              |   - High-Speed LRU Cache                    |
              +---------------------------------------------+
                                     |
                                     v
              +---------------------------------------------+
              |           l2arabic.ini & config.h           |
              |   (Real-time Configuration & Hard Limits)   |
              +---------------------------------------------+
```

---

## 3. تفاصيل ملفات المشروع وهيكليتها البرمجية

| اسم الملف | المسار النسبي | الوظيفة الدقيقة والمسؤولية التقنية |
|---|---|---|
| `d3d9_proxy.cpp` | `V2\d3d9_proxy.cpp` | تنفيذ تصديرات Direct3D 9، إنشاء خيط HookThread، البحث عن نافذة اللعبة وتثبيت WndProc Subclassing، تثبيت هوك vtable لـ NCEditBox، باتش فلتر اليونيكود، إدارة لوحة المفاتيح والـ Backspace، ونافذة الترحيب. |
| `arabic_nwindow_hook.h` | `V2\arabic_nwindow_hook.h` | اعتراض دوال `NWindow.dll` و `Engine.dll` عبر جداول الـ IAT، هوك `CreateFontFace` و `CreateFontW`، هوكات الرسم `UCanvas`، دوال المناطق والخرائط `FL2GameData`، وتثبيت الترامبولين. |
| `arabic_reshaper.h` | `V2\arabic_reshaper.h` | محرك التشكيل المنطقي (Shaping) وعكس النصوص (BiDi) وتحويل الحروف إلى Presentation Forms-B، دمج حركات اللام-ألف، عكس الأقواس، وإدارة الذاكرة المؤقتة. |
| `config.h` | `V2\config.h` | هيكل الإعدادات `AppConfig` ومؤشر `g_Config` العام، ودوال قراءة ملف `l2arabic.ini` والتحقق من حدود الحروف الصارمة. |
| `l2arabic.ini` | `V2\l2arabic.ini` | ملف الإعدادات الخارجي (الحدود القصوى، تفعيل الشات، تفعيل تعريب النوافذ، اسم الخط Cairo). |
| `d3d9.def` | `V2\d3d9.def` | ملف التصدير لتثبيت أسماء ومطابقة دوال DirectX 9 بالبايت. |
| `deploy.py` | `V2\deploy.py` | سكريبت النشر المتقدم الداعم لتقنية **Hot-Swap / Safe-Rename** لتحديث الملفات أثناء تشغيل اللعبة دون تعارض الوصول. |
| `build.bat` | `V2\build.bat` | سكريبت الترجمة والتجميع عبر Microsoft Visual C++ x86 32-bit. |

---

## 4. المرجع الهندسي للعناوين النسبية (RVAs) ومخطط الذاكرة

### أ. جدول مقارنة العناوين النسبية (RVAs) بين إصداري اللعبة

```cpp
// ============================================================================
// العناوين النسبية المستخرجة عبر الهندسة العكسية لمكتبة NWindow.dll
// ============================================================================

// 1. دوال محرك FString الأساسية:
RVA_FnPtr_GlobalSetText      : P520 = 0x00C7C114 | EU 542 = 0x00C8F114  // FString::operator=(const FString&)
RVA_FnPtr_FStringAssignW     : P520 = 0x00C7C1AC | EU 542 = 0x00C8F1AC  // FString::operator=(const wchar_t*)
RVA_FnPtr_FStringCtorW       : P520 = 0x00C7C118 | EU 542 = 0x00C8F118  // FString::FString(const wchar_t*)

// 2. دوال رسم وقياس النصوص UCanvas في Engine.dll:
RVA_FnPtr_DrawTextCanvas     : P520 = 0x00C7D2F0 | EU 542 = 0x00C9035C  // UCanvas::DrawTextTTFToCanvas
RVA_FnPtr_GetTextSizeTTF     : P520 = 0x00C7CC20 | EU 542 = 0x00C8FC54  // UCanvas::GetTextSizeTTF
RVA_FnPtr_GetTextSizeList    : P520 = 0x00C7DC70 | EU 542 = 0x00C90368  // UCanvas::GetTextSizeTTFList
RVA_FnPtr_DrawTextFormat     : P520 = 0x00C7D58C | EU 542 = 0x00C90600  // UCanvas::DrawTextTTFFormatStringToCanvas
RVA_FnPtr_GetTextSizeFormat  : P520 = 0x00C7D588 | EU 542 = 0x00C905FC  // UCanvas::GetTextSizeTTFFormatString
RVA_FnPtr_ParserGetSize      : P520 = 0x00C7D2F4 | EU 542 = 0x00C90360  // FL2FormatStringParser::GetSize
RVA_FnPtr_ParserGetAt        : P520 = 0x00C7D2F8 | EU 542 = 0x00C90364  // FL2FormatStringParser::GetAt

// 3. دوال نظام الخطوط:
RVA_FnPtr_CreateFontW        : P520 = 0x00C7DD18 | EU 542 = 0x00C90D18  // gdi32!CreateFontW (Flash Scaleform)
RVA_FnPtr_CreateFontFace     : P520 = N/A        | EU 542 = 0x00C90764  // Engine.dll!UFontManager::CreateFontFace

// 4. دوال بيانات المناطق والخرائط FL2GameData:
RVA_FnPtr_GetMinimapRegion   : P520 = 0x00C7D7E4 | EU 542 = 0x00C9085C  // FL2GameData::GetMinimapRegionData
RVA_FnPtr_GetZoneNameInfo    : P520 = 0x00C7DC28 | EU 542 = 0x00C90CA8  // FL2GameData::GetZoneNameInfoByID
RVA_FnPtr_GetZoneName        : P520 = 0x00C7D714 | EU 542 = 0x00C9078C  // FL2GameData::GetZoneName
RVA_FnPtr_GetCurrentZoneName : P520 = 0x00C7D244 | EU 542 = 0x00C902A8  // FL2GameData::GetCurrentZoneName
RVA_FnPtr_GetZoneNameFromID  : P520 = 0x00C7D96C | EU 542 = 0x00C909E8  // FL2GameData::GetZoneNameFromID
RVA_FnPtr_GetHuntingZoneData : P520 = 0x00C7D294 | EU 542 = 0x00C902FC  // FL2GameData::GetHuntingZoneData
RVA_FnPtr_FNameGetEntry      : P520 = 0x00C7C27C | EU 542 = 0x00C8F27C  // FName::GetEntry

// 5. عناوين الترامبولين لدوال الفلاش المباشرة (Inline Detours):
RVA_Fn_BuildZoneFlashObject  : P520 = 0x00B78940 | EU 542 = 0x00B8B9D0
RVA_Fn_BuildHuntingZoneFlash : P520 = 0x00B74A00 | EU 542 = 0x00B87A70

// 6. إزاحات جداول بيانات النصوص الثابتة (Offsets in NWindow.dll):
OFFSET_SysStringTable        : P520 = 0x002EE8C0 | EU 542 = 0x002F18FC
OFFSET_CastleTable           : P520 = 0x00329294 | EU 542 = 0x0032C2D0
OFFSET_ZoneTable             : P520 = 0x0032D330 | EU 542 = 0x0033036C
```

---

### ب. تشريح كائن NCEditBox في الذاكرة (Memory Layout)

كائن `NCEditBox` هو ويدجت داخلي مشتق من `UEditBox` ويحتوي على بنية الذاكرة الدقيقة التالية:

```
[pBox + 0x000] : Pointer to NCEditBox VTable (Base + 0xe181a8)
...
[pBox + 0x240] : FString.Data*   (wchar_t*) -> مؤشر مصفوفة الحروف الموسعة
[pBox + 0x244] : FString.Num    (int32)     -> عدد الحروف الحالية شاملة Null Terminator
[pBox + 0x248] : FString.Max    (int32)     -> السعة القصوى المخصصة في الذاكرة (Capacity)
...
[pBox + 0x384] : CursorPos      (int32)     -> موضع المؤشر البصري الداخلي (Caret Offset)
```

#### بنية هوك الـ VTable:
- قاعدة جدول الدوال الافتراضية: `NWindow.dll + 0xe181a8`
- خانة دالة `OnChar` في الـ VTable هي **Index 150** (الإزاحة بالبايت: `0x258`):
  `pVtableSlot = (void**)((BYTE*)hNWindow + 0xe181a8 + 0x258);`
- عند استدعاء `OnChar`، نلتقط مؤشر `pThis` الخاص بصندوق الشات ونخزنه بشكل متزامن ذرّياً (`InterlockedExchange(&s_pNCEditBox, (LONG)pThis)`).

#### باتش فلتر اليونيكود (Unicode Bypass Patch):
- الموقع: `NWindow.dll + 0xaae074`
- التعليمات الأصلية: `83 FF 7E 0F 86 CF 00 00 00` (مقارنة `cmp esi, 0x7E` ثم قفز شرطي يتجاهل أي حرف أعلى من ASCII).
- الباتش: استبدال التعليمات بـ `E9 D3 00 00 00 90 90 90 90` (قفز غير مشروط `JMP` لتخطي الفلتر والسماح بالحروف العربية).

#### باتش دوال الـ IME:
- يتم حقن `safeReturnStub` في مستهل دالتي `execSetAsChatEditBox` و `execSetIME` لمنع تعطيل إدخال الأحرف الموسعة من النظام.

---

## 5. المعمارية الهندسية لنظام الخطوط المزدوجة (Dual Font Engine)

أثبتت الهندسة العكسية لمحرك اللعبة أنه لا يوجد نظام خطوط موحد داخل Unreal Engine 2.5 الخاص بلعبة Lineage II، بل نظامان مستقلان تماماً:

```
                              [واجهة مستخدم Lineage II]
                                         |
               +-------------------------+-------------------------+
               |                                                   |
               v                                                   v
     [نظام Flash / Scaleform]                             [نظام المحرك الأساسي]
  (عناوين النوافذ، الأزرار، الخرائط)                    (التولتيب، وصف العناصر، الشات،
               |                                        نصوص الـ HTML، أرقام الضربات)
               v                                                   |
      gdi32!CreateFontW                                            v
  (مستوردة في NWindow.dll)                          Engine.dll!UFontManager::CreateFontFace
               |                                        (IAT RVA: 0x00C90764)
               v                                                   |
     [Hook_CreateFontW]                                            v
               |                                         [Hook_CreateFontFace]
               +-------------------------+-------------------------+
                                         |
                                         v
                         توجيه المسار والاسم تلقائياً إلى:
                            - Cairo.ttf (Regular)
                            - Cairo-Bold.ttf (Weight >= 700)
```

### آليات التحميل والتعرف على الخطوط:
1. **التسجيل النظامي المباشر:**
   - يتم تثبيت الخطين في `C:\Windows\Fonts\` وتسجيلهما في الريجستري تحت:
     `HKLM:\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Fonts`
     (القيم: `"Cairo (TrueType)" = "Cairo.ttf"` و `"Cairo Bold (TrueType)" = "Cairo-Bold.ttf"`).
2. **الاستدعاء الخاص في الذاكرة:**
   - يتم استدعاء `AddFontResourceExW(fontPath, FR_PRIVATE, 0)` لضمان تسجيل الخط لعملية اللعبة فقط دون الحاجة لصلاحيات مدير إضافية.
3. **التعامل مع خط أرقام الضربات والضرر (`$DamageTextFont`):**
   - في ملف `TTFontInfo.ini`، يُعرف خط أرقام الضربات كـ `Sylfaen`.
   - كود الهوك يدعم استبداله بخط `Cairo` الموحد أو استثناءه بناءً على رغبة المستخدم.

---

## 6. نموذج وخوارزمية الكتابة الحية للشات (RTL Live Input Engine)

### أ. منطق المخزن المنطقي والبصري
- **المخزن المنطقي (`s_arabicWord`):** يخزن الحروف بترتيب ورودها الطبيعي من لوحة المفاتيح.
- **المخزن البصري المعروض:** ناتج تشكيل وعكس النص عبر `ArabicBiDi::ProcessArabic(s_arabicWord)`.
- **موضع المؤشر (`s_visualCaret`):**
  - في الحالة الطبيعية (Append-only)، يضاف الحرف دائماً في نهاية `s_arabicWord`.
  - يوضع المؤشر البصري عند `0` (أقصى يسار النص العربي المعكوس = رأس الكتابة النشط).

### ب. خوارزمية الرسم المزدوجة (`DoRedraw`)

```cpp
void DoRedraw(HWND hWnd, const std::wstring& prevVisual, const std::wstring& nextVisual, int targetCaret)
{
    // 1. المسار فائق السرعة: الكتابة المباشرة في كائن NCEditBox
    if (WriteDirectToNCEditBox(nextVisual, targetCaret)) {
        return; // نجح التحديث الفوري في الذاكرة دون محاكاة رسائل
    }

    // 2. المسار الاحتياطي عبر رسائل النافذة (Message Fallback):
    // أ. حماية التداخل (Anti-Recursion Shield)
    InterlockedIncrement(&s_internalDepth);

    // ب. تحديد كامل النص الحالي
    BYTE keyState[256];
    GetKeyboardState(keyState);
    keyState[VK_CONTROL] |= 0x80;
    SetKeyboardState(keyState);
    SendMessageW(hWnd, WM_KEYDOWN, 'A', 0x001E0001);
    SendMessageW(hWnd, WM_KEYUP, 'A', 0xC01E0001);
    keyState[VK_CONTROL] &= ~0x80;
    SetKeyboardState(keyState);

    // ج. حذف النص المحدد
    SendMessageW(hWnd, WM_KEYDOWN, VK_BACK, 0x000E0001);
    SendMessageW(hWnd, WM_KEYUP, VK_BACK, 0xC00E0001);

    // د. حلقة أمان إضافية لضمان تفريغ الصندوق تماماً
    size_t wipeCount = max(prevVisual.size(), nextVisual.size()) + 4;
    for (size_t i = 0; i < wipeCount; ++i) {
        SendMessageW(hWnd, WM_CHAR, VK_BACK, 0x000E0001);
    }

    // هـ. كتابة النص المشكل الجديد حرفاً بحرف
    for (wchar_t ch : nextVisual) {
        SendMessageW(hWnd, WM_CHAR, ch, 1);
    }

    // و. ضبط موضع المؤشر البصري
    if (targetCaret == 0) {
        SendMessageW(hWnd, WM_KEYDOWN, VK_HOME, 0x01470001);
        SendMessageW(hWnd, WM_KEYUP, VK_HOME, 0xC1470001);
    } else if (targetCaret < (int)nextVisual.size()) {
        SendMessageW(hWnd, WM_KEYDOWN, VK_HOME, 0x01470001);
        SendMessageW(hWnd, WM_KEYUP, VK_HOME, 0xC1470001);
        for (int i = 0; i < targetCaret; ++i) {
            SendMessageW(hWnd, WM_KEYDOWN, VK_RIGHT, 0x014D0001);
            SendMessageW(hWnd, WM_KEYUP, VK_RIGHT, 0xC14D0001);
        }
    }

    InterlockedDecrement(&s_internalDepth);
}
```

### ج. معالجة زر الرجوع والحذف (`VK_BACK` و `VK_DELETE`)
- يتم استهلاك زر `VK_BACK` داخل `WM_KEYDOWN` حصراً لحذف آخر حرف منطقي تم إدخاله، ثم إعادة تشكيل النص واستدعاء `DoRedraw`.
- يتم حظر `WM_CHAR` للحرف `0x08` (`VK_BACK`) تماماً عندما يكون `!s_arabicWord.empty()` لمنع حذف حرفين بنقرة واحدة.

---

## 7. القواعد الحسابية لمحرك التشكيل العربي وعكس الأقواس (`arabic_reshaper.h`)

1. **تصنيف الحروف وقواعد الاتصال (Connecting Rules):**
   - الحروف غير المتصلة بما بعدها (Non-connecting Left): `أ، إ، آ، ا، د، ذ، ر، ز، و، ؤ، ى`.
   - استخراج أشكال الحروف من جدول **Unicode Presentation Forms-B** (نطاق `0xFE70 - 0xFEFF`):
     - الشكل المنعزل (Isolated)
     - الشكل النهائي (Final)
     - الشكل البداية (Initial)
     - الشكل الأوسط (Medial)
2. **دمج حركات اللام-ألف (Lam-Alef Ligatures):**
   - دمج `0x0644` (لام) مع `[0x0622, 0x0623, 0x0625, 0x0627]` في شكل مركب واحد:
     - `0xFEF5 / 0xFEF6` (لآ)
     - `0xFEF7 / 0xFEF8` (لأ)
     - `0xFEF9 / 0xFEFA` (لإ)
     - `0xFEFB / 0xFEFC` (لا)
3. **معالجة النصوص الهجينة والأرقام (BiDi Segmentation):**
   - النصوص الإنجليزية والأرقام (ASCII Digits / Arabic-Indic Digits) تعامل ككتل LTR مستقلة وتحافظ على ترتيبها الداخلي.
   - الجمل العربية تقلب RTL لعرضها على محرك الرسم.
4. **عكس الأقواس المنطقية (`MirrorBrackets`):**
   - يتم تبديل `(` بـ `)`، و `[` بـ `]`، و `{` بـ `}`، و `<` بـ `>`، و `«` بـ `»` حتى تظهر الأقواس مغلقة وسليمة بصرياً في السياق العربي المعكوس.
5. **الذاكرة المؤقتة الذكية (High-Speed LRU Cache):**
   - يتم تخزين نتائج النصوص المشكلة مسبقاً في `std::map` مع قائمة LRU بحجم افتراضي 2000 عنصر لتقليل استهلاك المعالج إلى الصفر أثناء رسم الإطارات المتكررة.

---

## 8. قيود الإدخال الصارمة والتنبيه الصوتي المهدأ (`ThrottledBeep`)

```cpp
inline size_t GetInputCharacterLimit(bool isLoginBox, const AppConfig& config) {
    const size_t hardLimit = isLoginBox ? DEFAULT_MAX_USERNAME_LENGTH : DEFAULT_MAX_LINE_LENGTH;
    const int configuredLimit = isLoginBox ? config.MaxUsernameLength : config.MaxChatLineLength;
    if (configuredLimit < 1) return hardLimit;
    return (size_t)configuredLimit < hardLimit ? (size_t)configuredLimit : hardLimit;
}
```

- **حقول تسجيل الدخول والحساب (`isLoginBox = true`):** حد أقصى صارم **25 حرفاً**.
- **صندوق الشات الرئيسي (`isLoginBox = false`):** حد أقصى صارم **45 حرفاً**.
- **التنبيه الصوتي المهدأ (`ThrottledBeep`):**
  - عند محاولة تجاوز الحد الأقصى، يتم فحص الوقت المنقضي عبر `GetTickCount64()`.
  - إذا كان الفارق أقل من `BeepThrottleMs` (400 مللي ثانية)، يتم كتم الصوت لمنع الإزعاج الصوتي المستمر.

---

## 9. مصفوفة المشاكل الهندسية الشائعة وحلولها النهائية

| الرقم | العَرَض أو المشكلة | السبب الجذري في المحرك | الحل الهندسي الدقيق |
|:---:|---|---|---|
| **1** | عدم ظهور الخط العربي في التولتيب والشات والـ HTML | هذه العناصر لا تستخدم `CreateFontW` بل تمر عبر `Engine.dll!UFontManager::CreateFontFace` | اعتراض IAT RVA `0x00C90764` وتوجيه `targetFace` إلى مسار `Cairo.ttf` و `Cairo-Bold.ttf`. |
| **2** | خطأ `Access Denied / File in use` أثناء تجميع `d3d9.dll` واللعبة مفتوحة | نظام Windows يقفل ملفات DLL المحملة في الذاكرة التنفيذية | استخدام ميزة **Safe-Rename (Hot-swap)** في `deploy.py`؛ إعادة تسمية الملف المقفول ثم نسخ الملف الجديد. |
| **3** | تكرار ومضاعفة الحروف العربية عند كتابة حرف واحد | إرسال الويندوز رسائل `WM_IME_CHAR` بالتوازي مع `WM_CHAR` | حظر رسالة `WM_IME_CHAR` فقط، وتمرير رسائل `WM_IME_COMPOSITION` الأخرى لتجنب تعليق الـ IME. |
| **4** | تجمد مؤشر الفأرة أو الكيبورد عند الضغط على الأسهم | إرجاع `return 0` عند معالجة أزرار `VK_LEFT / VK_RIGHT` | تحديث المؤشر الداخلي ثم تمرير الرسالة دائماً عبر `CallWindowProcW` للنافذة الأصلية. |
| **5** | مسح حرفين عند الضغط على Backspace مرة واحدة | معالجة زر المسح في رسالتي `WM_KEYDOWN` و `WM_CHAR` معاً | تنفيذ الحذف في `WM_KEYDOWN` فقط وحظر الـ `WM_CHAR` المقابل (كود `0x08`). |
| **6** | انهيار اللعبة (Crash) عند الخروج | بقاء مؤشرات VTable وهوكات الـ IAT موجهة نحو مساحات ذاكرة تم تفريغها | استعادة القيم الأصلية للـ VTable و IAT بدقة داخل حدث `DLL_PROCESS_DETACH`. |
| **7** | عدم ظهور أسماء بعض المناطق عند الانتقال | استخدام اللعبة دوال فلاش مباشرة (`BuildZoneFlashObject`) تتجاوز الـ IAT | تثبيت Inline Detour ترامبولين (5 بايت JMP مع 64 بايت ترامبولين) لحقن النصوص المشكلة. |

---

## 10. تعليمات البناء، التجميع، والنشر المؤتمت

### أ. بيئة البناء المطلوبة:
- **المترجم:** Microsoft Visual C++ (MSVC) x86 32-bit (v143 أو أعلى ضمن Visual Studio 2022 / 2026).
- **المكتبات المرتبطة:** `user32.lib`, `gdi32.lib`, `gdiplus.lib`.
- **المعايير:** `/std:c++17`, `/utf-8`, `/EHsc`, `/O2` (Optimization for Speed), `/LD` (Create DLL).

### ب. أمر البناء الصريح:
```bat
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat"
cl.exe /LD /O2 /EHsc /std:c++17 /utf-8 d3d9_proxy.cpp /link /OUT:d3d9.dll /DEF:d3d9.def user32.lib gdi32.lib gdiplus.lib
```

### ج. النشر المؤتمت عبر `deploy.py`:
يقوم سكريبت `deploy.py` بنقل الملفات الأربعة الأساسية تلقائياً إلى مجلد اللعبة:
1. `d3d9.dll`
2. `l2arabic.ini`
3. `Cairo.ttf`
4. `Cairo-Bold.ttf`
مع تفعيل آلية الـ Hot-swap التي تسمح بإعادة البناء والتجربة الحية حتى أثناء تشغيل عميل اللعبة.

### د. شروط التوثيق البرمجي:
يجب أن تتصدر الترويسة الرسمية كافة الملفات البرمجية للمشروع:
```cpp
// ============================================================================
// تم الإنشاء والتطوير بواسطة: Mohamed Tarek
// All rights and development reserved to: Mohamed Tarek
// ============================================================================
```
