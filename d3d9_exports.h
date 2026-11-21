// ============================================================================
// ملف التحقق من تطابق الدوال المُصدَّرة: d3d9_exports.h
// ============================================================================
#pragma once

// قائمة بأسماء الدوال المتوقعة من مكتبة d3d9.dll الأصلية
// تُستخدم للتحقق عند بدء التشغيل (runtime validation)
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
static const int EXPECTED_D3D9_EXPORTS_COUNT = 14;