// ============================================================================
// تم الإنشاء والتطوير بواسطة: Mohamed Tarek
// All rights and development reserved to: Mohamed Tarek
// ============================================================================
// ملف تشكيل ومعالجة اللغة العربية: arabic_reshaper.h
// ============================================================================

#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <list>
#include <mutex>
#include "config.h"

namespace ArabicBiDi {

    // ------------------------------------------------------------------------
    // تصنيف الأحرف
    // ------------------------------------------------------------------------
    inline bool IsArabicLetter(wchar_t c) {
        return (c >= 0x0621 && c <= 0x064A);
    }

    inline bool IsArabicChar(wchar_t c) {
        return (c >= 0x0600 && c <= 0x06FF) ||
               (c >= 0xFB50 && c <= 0xFDFF) ||
               (c >= 0xFE70 && c <= 0xFEFF);
    }

    inline bool IsAsciiDigit(wchar_t c) {
        return c >= L'0' && c <= L'9';
    }

    inline bool IsArabicDigit(wchar_t c) {
        return (c >= 0x0660 && c <= 0x0669) || (c >= 0x06F0 && c <= 0x06F9);
    }

    inline bool IsDigit(wchar_t c) {
        return IsAsciiDigit(c) || IsArabicDigit(c);
    }

    inline bool IsLatinLetter(wchar_t c) {
        return (c >= L'A' && c <= L'Z') ||
               (c >= L'a' && c <= L'z') ||
               (c >= 0x00C0 && c <= 0x024F);
    }

    inline bool IsHaraka(wchar_t c) {
        return c >= 0x064B && c <= 0x065F;
    }

    inline bool IsTatweel(wchar_t c) {
        return c == 0x0640;
    }

    // ------------------------------------------------------------------------
    // فحص وجود حروف عربية أصلية
    // ------------------------------------------------------------------------
    inline bool HasArabic(const std::wstring& str) {
        for (wchar_t c : str) {
            if (IsArabicLetter(c)) return true;
        }
        return false;
    }

    // ------------------------------------------------------------------------
    // قواعد الاتصال
    // ------------------------------------------------------------------------
    inline bool IsNonConnectingLeft(wchar_t c) {
        switch (c) {
            case 0x0621: case 0x0622: case 0x0623: case 0x0624:
            case 0x0625: case 0x0627: case 0x0629: case 0x062F:
            case 0x0630: case 0x0631: case 0x0632: case 0x0648:
            case 0x0649:
                return true;
            default:
                return false;
        }
    }

    inline bool CanConnectRight(wchar_t c) {
        return (c >= 0x0622 && c <= 0x064A) && (c != 0x0621);
    }

    inline bool CanConnectLeft(wchar_t c) {
        if (!CanConnectRight(c)) return false;
        return !IsNonConnectingLeft(c);
    }

    // ------------------------------------------------------------------------
    // بنية أشكال الحرف
    // ------------------------------------------------------------------------
    struct CharForms {
        wchar_t isolated;
        wchar_t final_;
        wchar_t initial;
        wchar_t medial;
    };

    inline CharForms GetCharForms(wchar_t c) {
        switch (c) {
            case 0x0621: return { 0xFE80, 0xFE80, 0xFE80, 0xFE80 };
            case 0x0622: return { 0xFE81, 0xFE82, 0xFE81, 0xFE82 };
            case 0x0623: return { 0xFE83, 0xFE84, 0xFE83, 0xFE84 };
            case 0x0624: return { 0xFE85, 0xFE86, 0xFE85, 0xFE86 };
            case 0x0625: return { 0xFE87, 0xFE88, 0xFE87, 0xFE88 };
            case 0x0626: return { 0xFE89, 0xFE8A, 0xFE8B, 0xFE8C };
            case 0x0627: return { 0xFE8D, 0xFE8E, 0xFE8D, 0xFE8E };
            case 0x0628: return { 0xFE8F, 0xFE90, 0xFE91, 0xFE92 };
            case 0x0629: return { 0xFE93, 0xFE94, 0xFE93, 0xFE94 };
            case 0x062A: return { 0xFE95, 0xFE96, 0xFE97, 0xFE98 };
            case 0x062B: return { 0xFE99, 0xFE9A, 0xFE9B, 0xFE9C };
            case 0x062C: return { 0xFE9D, 0xFE9E, 0xFE9F, 0xFEA0 };
            case 0x062D: return { 0xFEA1, 0xFEA2, 0xFEA3, 0xFEA4 };
            case 0x062E: return { 0xFEA5, 0xFEA6, 0xFEA7, 0xFEA8 };
            case 0x062F: return { 0xFEA9, 0xFEAA, 0xFEA9, 0xFEAA };
            case 0x0630: return { 0xFEAB, 0xFEAC, 0xFEAB, 0xFEAC };
            case 0x0631: return { 0xFEAD, 0xFEAE, 0xFEAD, 0xFEAE };
            case 0x0632: return { 0xFEAF, 0xFEB0, 0xFEAF, 0xFEB0 };
            case 0x0633: return { 0xFEB1, 0xFEB2, 0xFEB3, 0xFEB4 };
            case 0x0634: return { 0xFEB5, 0xFEB6, 0xFEB7, 0xFEB8 };
            case 0x0635: return { 0xFEB9, 0xFEBA, 0xFEBB, 0xFEBC };
            case 0x0636: return { 0xFEBD, 0xFEBE, 0xFEBF, 0xFEC0 };
            case 0x0637: return { 0xFEC1, 0xFEC2, 0xFEC3, 0xFEC4 };
            case 0x0638: return { 0xFEC5, 0xFEC6, 0xFEC7, 0xFEC8 };
            case 0x0639: return { 0xFEC9, 0xFECA, 0xFECB, 0xFECC };
            case 0x063A: return { 0xFECD, 0xFECE, 0xFECF, 0xFED0 };
            case 0x0641: return { 0xFED1, 0xFED2, 0xFED3, 0xFED4 };
            case 0x0642: return { 0xFED5, 0xFED6, 0xFED7, 0xFED8 };
            case 0x0643: return { 0xFED9, 0xFEDA, 0xFEDB, 0xFEDC };
            case 0x0644: return { 0xFEDD, 0xFEDE, 0xFEDF, 0xFEE0 };
            case 0x0645: return { 0xFEE1, 0xFEE2, 0xFEE3, 0xFEE4 };
            case 0x0646: return { 0xFEE5, 0xFEE6, 0xFEE7, 0xFEE8 };
            case 0x0647: return { 0xFEE9, 0xFEEA, 0xFEEB, 0xFEEC };
            case 0x0648: return { 0xFEED, 0xFEEE, 0xFEED, 0xFEEE };
            case 0x0649: return { 0xFEEF, 0xFEF0, 0xFEEF, 0xFEF0 };
            case 0x064A: return { 0xFEF1, 0xFEF2, 0xFEF3, 0xFEF4 };
            default:     return { c, c, c, c };
        }
    }

    // ------------------------------------------------------------------------
    // ★ جديد: عكس الأقواس للسياق RTL
    // ------------------------------------------------------------------------
    inline wchar_t MirrorBracket(wchar_t c) {
        if (!g_Config.MirrorBrackets) return c;
        switch (c) {
            case L'(': return L')';
            case L')': return L'(';
            case L'[': return L']';
            case L']': return L'[';
            case L'{': return L'}';
            case L'}': return L'{';
            case L'<': return L'>';
            case L'>': return L'<';
            case 0x00AB: return 0x00BB; // «  »
            case 0x00BB: return 0x00AB;
            default: return c;
        }
    }

    // ------------------------------------------------------------------------
    // دالة تشكيل الكلمات
    // ------------------------------------------------------------------------
    inline std::wstring Reshape(const std::wstring& text) {
        if (!HasArabic(text)) return text;

        std::wstring result;
        result.reserve(text.size());
        size_t n = text.size();

        for (size_t i = 0; i < n; ++i) {
            wchar_t c = text[i];

            if (IsHaraka(c)) continue;

            if (!IsArabicChar(c)) {
                result.push_back(c);
                continue;
            }

            // Lam-Alef ligatures
            if (c == 0x0644 && i + 1 < n) {
                wchar_t next = text[i + 1];
                wchar_t lig = 0;
                if (next == 0x0622) lig = 0xFEF5;
                else if (next == 0x0623) lig = 0xFEF7;
                else if (next == 0x0625) lig = 0xFEF9;
                else if (next == 0x0627) lig = 0xFEFB;

                if (lig != 0) {
                    bool prevConnected = (i > 0 && CanConnectLeft(text[i - 1]));
                    if (prevConnected) lig += 1;
                    result.push_back(lig);
                    ++i;
                    continue;
                }
            }

            // اتصال مع تجاوز الحركات
            bool prevConnected = false;
            for (size_t j = i; j > 0; --j) {
                wchar_t p = text[j - 1];
                if (IsHaraka(p)) continue;
                if (IsTatweel(p)) { prevConnected = true; break; }
                prevConnected = IsArabicLetter(p) && CanConnectLeft(p);
                break;
            }

            bool nextConnected = false;
            for (size_t j = i + 1; j < n; ++j) {
                wchar_t nx = text[j];
                if (IsHaraka(nx)) continue;
                if (IsTatweel(nx)) { nextConnected = true; break; }
                nextConnected = IsArabicLetter(nx) && CanConnectRight(nx);
                break;
            }

            CharForms forms = GetCharForms(c);
            if (prevConnected && nextConnected)
                result.push_back(forms.medial);
            else if (prevConnected)
                result.push_back(forms.final_);
            else if (nextConnected)
                result.push_back(forms.initial);
            else
                result.push_back(forms.isolated);
        }
        return result;
    }

    // ------------------------------------------------------------------------
    // ★ محسّن: BiDi مع معالجة صحيحة للأرقام والأقواس
    // ------------------------------------------------------------------------
    enum class RunClass {
        Arabic,   // RTL
        LTR,      // Latin + Digits (يجتمعان في run واحد)
        Neutral   // مسافات + علامات ترقيم
    };

    inline RunClass ClassifyChar(wchar_t c) {
        if (IsArabicChar(c) && !IsArabicDigit(c)) return RunClass::Arabic;
        if (IsLatinLetter(c) || IsDigit(c)) return RunClass::LTR;
        return RunClass::Neutral;
    }

    struct ProcessedResult {
        std::wstring visual;
        std::vector<size_t> v2l; // مصفوفة الربط: كل حرف بصري يقابله موقعه المنطقي الأصلي بدقة
    };

    inline size_t VisualCaretToLogicalInsertion(
        const std::wstring& logical, const ProcessedResult& mapping,
        size_t visualCaret) {
        if (logical.empty() || visualCaret == 0) return logical.size();
        if (visualCaret >= mapping.v2l.size())
            return HasArabic(logical) ? 0 : logical.size();
        const size_t visualLeft = mapping.v2l[visualCaret - 1];
        const size_t visualRight = mapping.v2l[visualCaret];
        return visualLeft > visualRight ? visualLeft : visualRight;
    }

    struct IndexedChar {
        wchar_t ch;
        size_t origIdx;
    };

    struct IndexedRun {
        std::vector<IndexedChar> chars;
        RunClass cls;
    };

    struct Run {
        std::wstring text;
        RunClass cls;
    };

    // ------------------------------------------------------------------------
    // معالجة وتشكيل النص العربي مع خريطة الربط الثنائية (Visual -> Logical)
    // تم الإنشاء والتطوير بواسطة: Mohamed Tarek
    // ------------------------------------------------------------------------
    inline ProcessedResult ProcessArabicWithMap(const std::wstring& input) {
        if (!HasArabic(input)) {
            std::vector<size_t> v2l(input.size());
            for (size_t i = 0; i < input.size(); ++i) v2l[i] = i;
            return { input, v2l };
        }

        // 1) تقسيم النص إلى مجموعات (Runs) مع حفظ فهرس كل حرف أصلي
        std::vector<IndexedRun> runs;
        size_t i = 0, n = input.size();
        while (i < n) {
            RunClass cc = ClassifyChar(input[i]);
            IndexedRun r;
            r.cls = cc;
            while (i < n && ClassifyChar(input[i]) == cc) {
                r.chars.push_back({ input[i], i });
                ++i;
            }
            runs.push_back(r);
        }

        // 2) تحديد الاتجاه الأساسي للنص
        bool baseRTL = false;
        for (const auto& r : runs) {
            if (r.cls == RunClass::Arabic) { baseRTL = true; break; }
            if (r.cls == RunClass::LTR)    { baseRTL = false; break; }
        }

        // 3) دمج الفواصل والرموز (Neutrals) مع السياق الصحيح
        for (size_t k = 0; k < runs.size(); ++k) {
            if (runs[k].cls != RunClass::Neutral) continue;

            bool leftArabic  = (k > 0 && runs[k-1].cls == RunClass::Arabic);
            bool rightArabic = (k + 1 < runs.size() && runs[k+1].cls == RunClass::Arabic);
            bool leftLTR     = (k > 0 && runs[k-1].cls == RunClass::LTR);
            bool rightLTR    = (k + 1 < runs.size() && runs[k+1].cls == RunClass::LTR);

            if (leftArabic || rightArabic) {
                runs[k].cls = RunClass::Arabic;
            } else if (leftLTR || rightLTR) {
                runs[k].cls = RunClass::LTR;
            } else {
                runs[k].cls = baseRTL ? RunClass::Arabic : RunClass::LTR;
            }
        }

        // 4) دمج الـ runs المتجاورة من نفس النوع
        std::vector<IndexedRun> merged;
        for (const auto& r : runs) {
            if (!merged.empty() && merged.back().cls == r.cls) {
                merged.back().chars.insert(merged.back().chars.end(), r.chars.begin(), r.chars.end());
            } else {
                merged.push_back(r);
            }
        }

        // 5) تشكيل الحروف العربية وعكسها مع الحفاظ على الربط بالأساس المنطقي
        for (auto& r : merged) {
            if (r.cls == RunClass::Arabic) {
                std::wstring raw;
                for (const auto& ic : r.chars) raw += ic.ch;

                std::wstring reshaped = Reshape(raw);

                // مواءمة المؤشرات بين النص المشكّل والمدخل الأصلي (حساب دمج لام-ألف)
                std::vector<size_t> rIndices;
                size_t rawI = 0;
                for (size_t resI = 0; resI < reshaped.size(); ++resI) {
                    wchar_t c = reshaped[resI];
                    // فحص مركب لام-ألف
                    if ((c >= 0xFEF5 && c <= 0xFEFC) && rawI + 1 < r.chars.size() &&
                        r.chars[rawI].ch == 0x0644 && 
                        (r.chars[rawI+1].ch == 0x0622 || r.chars[rawI+1].ch == 0x0623 || 
                         r.chars[rawI+1].ch == 0x0625 || r.chars[rawI+1].ch == 0x0627)) {
                        rIndices.push_back(r.chars[rawI].origIdx);
                        rawI += 2;
                    } else {
                        if (rawI < r.chars.size()) {
                            rIndices.push_back(r.chars[rawI].origIdx);
                            rawI++;
                        } else {
                            rIndices.push_back(r.chars.empty() ? 0 : r.chars.back().origIdx);
                        }
                    }
                }

                // عكس اتجاه الحروف والمؤشرات للـ RTL
                std::reverse(reshaped.begin(), reshaped.end());
                std::reverse(rIndices.begin(), rIndices.end());
                for (wchar_t& c : reshaped) c = MirrorBracket(c);

                r.chars.clear();
                for (size_t idx = 0; idx < reshaped.size(); ++idx) {
                    r.chars.push_back({ reshaped[idx], rIndices[idx] });
                }
            }
        }

        // 6) عكس ترتيب المقاطع إذا كان الاتجاه العام RTL
        if (baseRTL) {
            std::reverse(merged.begin(), merged.end());
        }

        // 7) تجميع الناتج البصري النهائي وخريطة المطابقة المنطقية
        ProcessedResult result;
        for (const auto& r : merged) {
            for (const auto& ic : r.chars) {
                result.visual += ic.ch;
                result.v2l.push_back(ic.origIdx);
            }
        }
        return result;
    }

    // ------------------------------------------------------------------------
    // الدالة القياسية لإرجاع النص المعروض بصرياً
    // تم الإنشاء والتطوير بواسطة: Mohamed Tarek
    // ------------------------------------------------------------------------
    inline std::wstring ProcessArabic(const std::wstring& input) {
        return ProcessArabicWithMap(input).visual;
    }

    // ------------------------------------------------------------------------
    // فحص وجود أي حرف عربي (خام أو مشكّل)
    // ------------------------------------------------------------------------
    inline bool HasAnyArabic(const std::wstring& str) {
        for (wchar_t c : str) {
            if (IsArabicChar(c)) return true;
        }
        return false;
    }

    // ------------------------------------------------------------------------
    // إرجاع أشكال الحروف المركبة والمشكلة إلى أصلها الأبجدي الخام (Unicode Base)
    // ------------------------------------------------------------------------
    inline std::wstring Unshape(const std::wstring& text) {
        std::wstring res;
        res.reserve(text.size());
        for (wchar_t c : text) {
            switch (c) {
                case 0xFE80: res.push_back(0x0621); break; // همزة منفردة
                case 0xFE81: case 0xFE82: res.push_back(0x0622); break; // ألف مدة
                case 0xFE83: case 0xFE84: res.push_back(0x0623); break; // ألف همزة فوق
                case 0xFE85: case 0xFE86: res.push_back(0x0624); break; // واو همزة
                case 0xFE87: case 0xFE88: res.push_back(0x0625); break; // ألف همزة تحت
                case 0xFE89: case 0xFE8A: case 0xFE8B: case 0xFE8C: res.push_back(0x0626); break; // ياء همزة
                case 0xFE8D: case 0xFE8E: res.push_back(0x0627); break; // ألف
                case 0xFE8F: case 0xFE90: case 0xFE91: case 0xFE92: res.push_back(0x0628); break; // باء
                case 0xFE93: case 0xFE94: res.push_back(0x0629); break; // تاء مربوطة
                case 0xFE95: case 0xFE96: case 0xFE97: case 0xFE98: res.push_back(0x062A); break; // تاء
                case 0xFE99: case 0xFE9A: case 0xFE9B: case 0xFE9C: res.push_back(0x062B); break; // ثاء
                case 0xFE9D: case 0xFE9E: case 0xFE9F: case 0xFEA0: res.push_back(0x062C); break; // جيم
                case 0xFEA1: case 0xFEA2: case 0xFEA3: case 0xFEA4: res.push_back(0x062D); break; // حاء
                case 0xFEA5: case 0xFEA6: case 0xFEA7: case 0xFEA8: res.push_back(0x062E); break; // خاء
                case 0xFEA9: case 0xFEAA: res.push_back(0x062F); break; // دال
                case 0xFEAB: case 0xFEAC: res.push_back(0x0630); break; // ذال
                case 0xFEAD: case 0xFEAE: res.push_back(0x0631); break; // راء
                case 0xFEAF: case 0xFEB0: res.push_back(0x0632); break; // زاي
                case 0xFEB1: case 0xFEB2: case 0xFEB3: case 0xFEB4: res.push_back(0x0633); break; // سين
                case 0xFEB5: case 0xFEB6: case 0xFEB7: case 0xFEB8: res.push_back(0x0634); break; // شين
                case 0xFEB9: case 0xFEBA: case 0xFEBB: case 0xFEBC: res.push_back(0x0635); break; // صاد
                case 0xFEBD: case 0xFEBE: case 0xFEBF: case 0xFEC0: res.push_back(0x0636); break; // ضاد
                case 0xFEC1: case 0xFEC2: case 0xFEC3: case 0xFEC4: res.push_back(0x0637); break; // طاء
                case 0xFEC5: case 0xFEC6: case 0xFEC7: case 0xFEC8: res.push_back(0x0638); break; // ظاء
                case 0xFEC9: case 0xFECA: case 0xFECB: case 0xFECC: res.push_back(0x0639); break; // عين
                case 0xFECD: case 0xFECE: case 0xFECF: case 0xFED0: res.push_back(0x063A); break; // غين
                case 0xFED1: case 0xFED2: case 0xFED3: case 0xFED4: res.push_back(0x0641); break; // فاء
                case 0xFED5: case 0xFED6: case 0xFED7: case 0xFED8: res.push_back(0x0642); break; // قاف
                case 0xFED9: case 0xFEDA: case 0xFEDB: case 0xFEDC: res.push_back(0x0643); break; // كاف
                case 0xFEDD: case 0xFEDE: case 0xFEDF: case 0xFEE0: res.push_back(0x0644); break; // لام
                case 0xFEE1: case 0xFEE2: case 0xFEE3: case 0xFEE4: res.push_back(0x0645); break; // ميم
                case 0xFEE5: case 0xFEE6: case 0xFEE7: case 0xFEE8: res.push_back(0x0646); break; // نون
                case 0xFEE9: case 0xFEEA: case 0xFEEB: case 0xFEEC: res.push_back(0x0647); break; // هاء
                case 0xFEED: case 0xFEEE: res.push_back(0x0648); break; // واو
                case 0xFEEF: case 0xFEF0: res.push_back(0x0649); break; // ألف مقصورة
                case 0xFEF1: case 0xFEF2: case 0xFEF3: case 0xFEF4: res.push_back(0x064A); break; // ياء
                // تشكيلات لام ألف المركبة
                case 0xFEF5: case 0xFEF6: res.push_back(0x0644); res.push_back(0x0622); break;
                case 0xFEF7: case 0xFEF8: res.push_back(0x0644); res.push_back(0x0623); break;
                case 0xFEF9: case 0xFEFA: res.push_back(0x0644); res.push_back(0x0625); break;
                case 0xFEFB: case 0xFEFC: res.push_back(0x0644); res.push_back(0x0627); break;
                default: res.push_back(c); break;
            }
        }
        return res;
    }

    // ------------------------------------------------------------------------
    // تحويل النص المعروض بصرياً (Visual) إلى أصله المنطقي الأصلي (Logical)
    // ------------------------------------------------------------------------
    inline std::wstring VisualToLogical(const std::wstring& visual) {
        if (!HasAnyArabic(visual)) return visual;

        std::wstring unshaped = Unshape(visual);
        for (wchar_t& c : unshaped) c = MirrorBracket(c);

        // 1) تقسيم إلى runs
        std::vector<Run> runs;
        size_t i = 0, n = unshaped.size();
        while (i < n) {
            RunClass cc = ClassifyChar(unshaped[i]);
            size_t start = i;
            ++i;
            while (i < n && ClassifyChar(unshaped[i]) == cc) ++i;
            runs.push_back({ unshaped.substr(start, i - start), cc });
        }

        // 2) تحديد الاتجاه الأساسي
        bool baseRTL = false;
        for (const auto& r : runs) {
            if (r.cls == RunClass::Arabic) { baseRTL = true; break; }
            if (r.cls == RunClass::LTR)    { baseRTL = false; break; }
        }

        // 3) دمج الـ Neutrals مع الجار المناسب
        for (size_t k = 0; k < runs.size(); ++k) {
            if (runs[k].cls != RunClass::Neutral) continue;
            bool leftArabic  = (k > 0 && runs[k-1].cls == RunClass::Arabic);
            bool rightArabic = (k + 1 < runs.size() && runs[k+1].cls == RunClass::Arabic);
            bool leftLTR     = (k > 0 && runs[k-1].cls == RunClass::LTR);
            bool rightLTR    = (k + 1 < runs.size() && runs[k+1].cls == RunClass::LTR);

            if (leftArabic || rightArabic) {
                runs[k].cls = RunClass::Arabic;
            } else if (leftLTR || rightLTR) {
                runs[k].cls = RunClass::LTR;
            } else {
                runs[k].cls = baseRTL ? RunClass::Arabic : RunClass::LTR;
            }
        }

        // 4) دمج الـ runs المتجاورة
        std::vector<Run> merged;
        for (const auto& r : runs) {
            if (!merged.empty() && merged.back().cls == r.cls) {
                merged.back().text += r.text;
            } else {
                merged.push_back(r);
            }
        }

        // 5) عكس ترتيب الـ runs إذا كان الاتجاه RTL
        if (baseRTL) {
            std::reverse(merged.begin(), merged.end());
        }

        // 6) عكس حروف الـ Arabic runs
        for (auto& r : merged) {
            if (r.cls == RunClass::Arabic) {
                std::reverse(r.text.begin(), r.text.end());
            }
        }

        // 7) تجميع النص المنطقي
        std::wstring logical;
        for (const auto& r : merged) logical += r.text;
        return logical;
    }

    // ------------------------------------------------------------------------
    // ★ إصلاح: LRU Cache مع حد أقصى للحجم (يمنع تسريب الذاكرة)
    // ------------------------------------------------------------------------
    class ReshapeCache {
    public:
        static ReshapeCache& Instance() {
            static ReshapeCache inst;
            return inst;
        }

        // إرجاع نسخة (by value) لتجنب مشاكل dangling references
        std::wstring Get(const std::wstring& raw) {
            std::lock_guard<std::mutex> lk(mx_);
            auto it = map_.find(raw);
            if (it != map_.end()) {
                // تحديث ترتيب LRU: نقل للـ front
                order_.splice(order_.begin(), order_, it->second.second);
                return it->second.first;
            }

            std::wstring reshaped = ProcessArabic(raw);

            int maxSize = g_Config.MaxReshapeCacheSize;
            if (maxSize < 1) maxSize = 1;

            // إخلاء أقدم عنصر إذا تجاوزنا الحد
            while ((int)map_.size() >= maxSize && !order_.empty()) {
                std::wstring oldest = order_.back();
                order_.pop_back();
                map_.erase(oldest);
            }

            order_.push_front(raw);
            map_[raw] = { reshaped, order_.begin() };
            return reshaped;
        }

        void Clear() {
            std::lock_guard<std::mutex> lk(mx_);
            map_.clear();
            order_.clear();
        }

        size_t Size() {
            std::lock_guard<std::mutex> lk(mx_);
            return map_.size();
        }

    private:
        ReshapeCache() {}
        ReshapeCache(const ReshapeCache&) = delete;
        ReshapeCache& operator=(const ReshapeCache&) = delete;

        std::map<std::wstring,
                 std::pair<std::wstring,
                           std::list<std::wstring>::iterator>> map_;
        std::list<std::wstring> order_;
        std::mutex mx_;
    };

} // namespace ArabicBiDi