// ============================================================================
// Lineage2 Professional Welcome Dialog
// ----------------------------------------------------------------------------
#include <windows.h>
#include <windowsx.h>
#include <objidl.h>
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "gdi32.lib")
using namespace Gdiplus;

// ---------------------------------------------------------------------------
// Globals
// ---------------------------------------------------------------------------
static HWND      g_hWelcomeWnd   = NULL;
static bool      g_btnHover      = false;
static bool      g_btnPressed    = false;
static bool      g_closeHover    = false;
static bool      g_dragging      = false;
static bool      g_tracking      = false;
static POINT     g_dragOrigin    = {0};

static const wchar_t* kWndClass  = L"L2WelcomeDialogClass";
static const wchar_t* kTitle     = L"تعريب اللعبة";
static const wchar_t* kSubtitle  = L"Mohamed Tarek  •  Arabic Edition";
static const wchar_t* kBodyL1    = L"جميع الصلاحيات والترجمة العربية بواسطة";
static const wchar_t* kBodyL2    = L"★   Mohamed Tarek   ★";
static const wchar_t* kBodyL3    = L"نتمنى لك تجربة ممتعة داخل اللعبة!";
static const wchar_t* kBtnText   = L"استمتع باللعبة";

#define WND_W        580
#define WND_H        400
#define BTN_W        220
#define BTN_H        46
#define BTN_BOTTOM   34
#define CLOSE_SIZE   28
#define CLOSE_MARGIN 14

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------
static void DrawRoundRect(Graphics& g, const RectF& rc, REAL r,
                          Pen* pen, Brush* brush)
{
    REAL d = r * 2.0f;
    GraphicsPath path;
    path.AddArc(rc.X, rc.Y, d, d, 180.0f, 90.0f);
    path.AddArc(rc.GetRight() - d, rc.Y, d, d, 270.0f, 90.0f);
    path.AddArc(rc.GetRight() - d, rc.GetBottom() - d, d, d, 0.0f, 90.0f);
    path.AddArc(rc.X, rc.GetBottom() - d, d, d, 90.0f, 90.0f);
    path.CloseFigure();
    if (brush) g.FillPath(brush, &path);
    if (pen)   g.DrawPath(pen, &path);
}

static RectF GetButtonRect(HWND h)
{
    RECT rc; GetClientRect(h, &rc);
    REAL x = (rc.right - BTN_W) / 2.0f;
    REAL y = (REAL)(rc.bottom - BTN_BOTTOM - BTN_H);
    return RectF(x, y, (REAL)BTN_W, (REAL)BTN_H);
}

static RectF GetCloseRect(HWND h)
{
    RECT rc; GetClientRect(h, &rc);
    return RectF((REAL)(rc.right - CLOSE_MARGIN - CLOSE_SIZE),
                 (REAL)CLOSE_MARGIN,
                 (REAL)CLOSE_SIZE, (REAL)CLOSE_SIZE);
}

// ---------------------------------------------------------------------------
// Paint
// ---------------------------------------------------------------------------
static void PaintWelcome(HWND hWnd)
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);

    RECT rc; GetClientRect(hWnd, &rc);
    int w = rc.right, h = rc.bottom;

    HDC     memDC  = CreateCompatibleDC(hdc);
    HBITMAP memBmp = CreateCompatibleBitmap(hdc, w, h);
    HBITMAP oldBmp = (HBITMAP)SelectObject(memDC, memBmp);

    Graphics g(memDC);
    g.SetSmoothingMode(SmoothingModeAntiAlias);
    g.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);
    g.SetPixelOffsetMode(PixelOffsetModeHalf);
    g.SetInterpolationMode(InterpolationModeHighQualityBicubic);

    // ===== Background gradient =====
    RectF bgRc(0, 0, (REAL)w, (REAL)h);
    LinearGradientBrush bgBrush(bgRc,
        Color(255, 10, 13, 24),
        Color(255, 28, 34, 58),
        LinearGradientModeVertical);
    g.FillRectangle(&bgBrush, bgRc);

    // ===== Top golden glow =====
    RectF glowRc(0, 0, (REAL)w, 150.0f);
    LinearGradientBrush glowBrush(glowRc,
        Color(85, 201, 162, 39),
        Color(0, 201, 162, 39),
        LinearGradientModeVertical);
    g.FillRectangle(&glowBrush, glowRc);

    // ===== Outer golden border =====
    Pen outerPen(Color(255, 201, 162, 39), 1.6f);
    DrawRoundRect(g, RectF(0.8f, 0.8f, (REAL)w - 1.6f, (REAL)h - 1.6f),
                  14.0f, &outerPen, nullptr);

    // ===== Inner hairline =====
    Pen innerPen(Color(70, 232, 200, 120), 1.0f);
    DrawRoundRect(g, RectF(6.0f, 6.0f, (REAL)w - 12.0f, (REAL)h - 12.0f),
                  10.0f, &innerPen, nullptr);

    // ===== Corner diamond ornaments =====
    {
        SolidBrush goldBr(Color(255, 210, 175, 80));
        REAL cx[4] = { 22.0f, (REAL)w - 22.0f, 22.0f, (REAL)w - 22.0f };
        REAL cy[4] = { 22.0f, 22.0f, (REAL)h - 22.0f, (REAL)h - 22.0f };
        for (int i = 0; i < 4; i++) {
            PointF pts[4] = {
                PointF(cx[i], cy[i] - 5.0f),
                PointF(cx[i] + 5.0f, cy[i]),
                PointF(cx[i], cy[i] + 5.0f),
                PointF(cx[i] - 5.0f, cy[i])
            };
            g.FillPolygon(&goldBr, pts, 4);
        }
    }

    StringFormat sfCenter;
    sfCenter.SetAlignment(StringAlignmentCenter);
    sfCenter.SetLineAlignment(StringAlignmentCenter);

    // ===== Title =====
    FontFamily ff(L"Segoe UI");
    Font fontTitle(&ff, 28.0f, FontStyleBold, UnitPixel);
    RectF titleRc(0, 38.0f, (REAL)w, 36.0f);
    SolidBrush goldText(Color(255, 235, 205, 125));
    g.DrawString(kTitle, -1, &fontTitle, titleRc, &sfCenter, &goldText);

    // ===== Subtitle =====
    Font fontSub(&ff, 14.0f, FontStyleRegular, UnitPixel);
    RectF subRc(0, 78.0f, (REAL)w, 20.0f);
    SolidBrush subText(Color(200, 180, 150, 90));
    g.DrawString(kSubtitle, -1, &fontSub, subRc, &sfCenter, &subText);

    // ===== Decorative divider =====
    {
        REAL dy = 116.0f;
        REAL cx = w / 2.0f;
        REAL len = 150.0f;

        LinearGradientBrush lineL(
            RectF(cx - len - 60.0f, dy, 120.0f, 2.0f),
            Color(0, 201, 162, 39), Color(255, 201, 162, 39),
            LinearGradientModeHorizontal);
        g.FillRectangle(&lineL, RectF(cx - len - 60.0f, dy, 120.0f, 1.4f));

        LinearGradientBrush lineR(
            RectF(cx + len - 60.0f, dy, 120.0f, 2.0f),
            Color(255, 201, 162, 39), Color(0, 201, 162, 39),
            LinearGradientModeHorizontal);
        g.FillRectangle(&lineR, RectF(cx + len - 60.0f, dy, 120.0f, 1.4f));

        SolidBrush goldBr(Color(255, 232, 200, 120));
        PointF dpts[4] = {
            PointF(cx, dy - 5.0f),
            PointF(cx + 5.0f, dy + 0.7f),
            PointF(cx, dy + 6.4f),
            PointF(cx - 5.0f, dy + 0.7f)
        };
        g.FillPolygon(&goldBr, dpts, 4);
    }

    // ===== Body text =====
    Font fontBody(&ff, 16.0f, FontStyleRegular, UnitPixel);
    Font fontBodyBold(&ff, 19.0f, FontStyleBold, UnitPixel);

    SolidBrush bodyText(Color(255, 225, 225, 235));
    SolidBrush goldBody(Color(255, 235, 205, 125));

    RectF b1Rc(30.0f, 142.0f, (REAL)w - 60.0f, 26.0f);
    g.DrawString(kBodyL1, -1, &fontBody, b1Rc, &sfCenter, &bodyText);

    RectF b2Rc(30.0f, 178.0f, (REAL)w - 60.0f, 30.0f);
    g.DrawString(kBodyL2, -1, &fontBodyBold, b2Rc, &sfCenter, &goldBody);

    RectF b3Rc(30.0f, 216.0f, (REAL)w - 60.0f, 26.0f);
    g.DrawString(kBodyL3, -1, &fontBody, b3Rc, &sfCenter, &bodyText);

    // ===== Button =====
    RectF btn = GetButtonRect(hWnd);

    if (g_btnHover || g_btnPressed) {
        for (int i = 6; i >= 1; i--) {
            BYTE a = (BYTE)(14 - i * 2);
            Pen glowPen(Color(a, 232, 200, 120), (REAL)(i * 2));
            DrawRoundRect(g, RectF(btn.X - i, btn.Y - i,
                                   btn.Width + i * 2, btn.Height + i * 2),
                          12.0f + i, &glowPen, nullptr);
        }
    }

    Color btnTop = g_btnPressed ? Color(255, 150, 118, 30)
                                : (g_btnHover ? Color(255, 236, 195, 85)
                                              : Color(255, 210, 165, 55));
    Color btnBot = g_btnPressed ? Color(255, 110, 85, 20)
                                : (g_btnHover ? Color(255, 180, 135, 40)
                                              : Color(255, 155, 115, 30));

    LinearGradientBrush btnBrush(btn, btnTop, btnBot,
                                 LinearGradientModeVertical);
    Pen btnBorder(Color(255, 235, 205, 125), 1.6f);
    DrawRoundRect(g, btn, 10.0f, &btnBorder, &btnBrush);

    Pen btnInner(Color(80, 255, 240, 200), 1.0f);
    DrawRoundRect(g, RectF(btn.X + 3, btn.Y + 3,
                           btn.Width - 6, btn.Height - 6),
                  8.0f, &btnInner, nullptr);

    Font fontBtn(&ff, 19.0f, FontStyleBold, UnitPixel);
    SolidBrush btnTextColor(g_btnPressed ? Color(255, 40, 30, 10)
                                          : Color(255, 35, 25, 5));
    g.DrawString(kBtnText, -1, &fontBtn, btn, &sfCenter, &btnTextColor);

    // ===== Close (X) =====
    RectF closeRc = GetCloseRect(hWnd);
    if (g_closeHover) {
        SolidBrush hoverBg(Color(120, 220, 60, 60));
        g.FillEllipse(&hoverBg, closeRc);
    }
    Pen xPen(g_closeHover ? Color(255, 255, 200, 200)
                          : Color(255, 190, 170, 170), 1.8f);
    REAL pad = 9.0f;
    g.DrawLine(&xPen,
        closeRc.X + pad, closeRc.Y + pad,
        closeRc.GetRight() - pad, closeRc.GetBottom() - pad);
    g.DrawLine(&xPen,
        closeRc.GetRight() - pad, closeRc.Y + pad,
        closeRc.X + pad, closeRc.GetBottom() - pad);

    BitBlt(hdc, 0, 0, w, h, memDC, 0, 0, SRCCOPY);

    SelectObject(memDC, oldBmp);
    DeleteObject(memBmp);
    DeleteDC(memDC);
    EndPaint(hWnd, &ps);
}

// ---------------------------------------------------------------------------
// Window procedure
// ---------------------------------------------------------------------------
static LRESULT CALLBACK WelcomeWndProc(HWND hWnd, UINT msg,
                                       WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CREATE:
        SetTimer(hWnd, 1, 800, NULL); // لأجل نبضة توهج اختيارية
        return 0;

    case WM_TIMER:
        // (اختياري) نبضة زر - يمكن إلغاؤها إن لم تُرَد
        return 0;

    case WM_PAINT:
        PaintWelcome(hWnd);
        return 0;

    case WM_ERASEBKGND:
        return 1;

    case WM_MOUSEMOVE: {
        if (!g_tracking) {
            TRACKMOUSEEVENT tme = { sizeof(tme), TME_LEAVE, hWnd, 0 };
            TrackMouseEvent(&tme);
            g_tracking = true;
        }
        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        RectF btn     = GetButtonRect(hWnd);
        RectF closeRc = GetCloseRect(hWnd);

        bool bh = btn.Contains((REAL)pt.x, (REAL)pt.y);
        bool ch = closeRc.Contains((REAL)pt.x, (REAL)pt.y);

        if (bh != g_btnHover || ch != g_closeHover) {
            g_btnHover   = bh;
            g_closeHover = ch;
            InvalidateRect(hWnd, NULL, FALSE);
        }

        SetCursor(LoadCursor(NULL, (bh || ch) ? IDC_HAND : IDC_ARROW));

        if (g_dragging) {
            POINT cur; GetCursorPos(&cur);
            SetWindowPos(hWnd, NULL,
                cur.x - g_dragOrigin.x,
                cur.y - g_dragOrigin.y,
                0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
        }
        return 0;
    }

    case WM_MOUSELEAVE:
        g_tracking = false;
        if (g_btnHover || g_closeHover) {
            g_btnHover = g_closeHover = false;
            InvalidateRect(hWnd, NULL, FALSE);
        }
        return 0;

    case WM_LBUTTONDOWN: {
        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        RectF btn     = GetButtonRect(hWnd);
        RectF closeRc = GetCloseRect(hWnd);

        if (closeRc.Contains((REAL)pt.x, (REAL)pt.y)) {
            DestroyWindow(hWnd);
            return 0;
        }
        if (btn.Contains((REAL)pt.x, (REAL)pt.y)) {
            g_btnPressed = true;
            InvalidateRect(hWnd, NULL, FALSE);
        } else {
            g_dragging = true;
            RECT wr; GetWindowRect(hWnd, &wr);
            POINT cur; GetCursorPos(&cur);
            g_dragOrigin.x = cur.x - wr.left;
            g_dragOrigin.y = cur.y - wr.top;
            SetCapture(hWnd);
        }
        return 0;
    }

    case WM_LBUTTONUP: {
        if (g_dragging) {
            g_dragging = false;
            ReleaseCapture();
        }
        if (g_btnPressed) {
            g_btnPressed = false;
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            RectF btn = GetButtonRect(hWnd);
            if (btn.Contains((REAL)pt.x, (REAL)pt.y)) {
                DestroyWindow(hWnd);
                return 0;
            }
            InvalidateRect(hWnd, NULL, FALSE);
        }
        return 0;
    }

    case WM_KEYDOWN:
        if (wParam == VK_ESCAPE || wParam == VK_RETURN || wParam == VK_SPACE)
            DestroyWindow(hWnd);
        return 0;

    case WM_DESTROY:
        KillTimer(hWnd, 1);
        g_hWelcomeWnd = NULL;
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

// ---------------------------------------------------------------------------
// Show
// ---------------------------------------------------------------------------
static void ShowWelcomeDialog()
{
    ULONG_PTR gdiToken = 0;
    GdiplusStartupInput gsi;
    GdiplusStartup(&gdiToken, &gsi, NULL);

    WNDCLASSEXW wc = {0};
    wc.cbSize        = sizeof(wc);
    wc.lpfnWndProc   = WelcomeWndProc;
    wc.hInstance     = GetModuleHandleW(NULL);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = NULL;
    wc.lpszClassName = kWndClass;
    RegisterClassExW(&wc);

    int sw = GetSystemMetrics(SM_CXSCREEN);
    int sh = GetSystemMetrics(SM_CYSCREEN);
    int x  = (sw - WND_W) / 2;
    int y  = (sh - WND_H) / 2;

    g_hWelcomeWnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
        kWndClass, kTitle,
        WS_POPUP | WS_VISIBLE,
        x, y, WND_W, WND_H,
        NULL, NULL, wc.hInstance, NULL);

    if (g_hWelcomeWnd) {
        // زوايا دائرية
        HRGN hRgn = CreateRoundRectRgn(0, 0, WND_W + 1, WND_H + 1, 18, 18);
        SetWindowRgn(g_hWelcomeWnd, hRgn, TRUE);

        ShowWindow(g_hWelcomeWnd, SW_SHOW);
        UpdateWindow(g_hWelcomeWnd);
        SetForegroundWindow(g_hWelcomeWnd);
        SetFocus(g_hWelcomeWnd);

        // ---- Dedicated Message loop ----
        MSG msg;
        while (GetMessageW(&msg, NULL, 0, 0))
        {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    if (gdiToken != 0) {
        GdiplusShutdown(gdiToken);
    }
    UnregisterClassW(kWndClass, wc.hInstance);
}

// ---------------------------------------------------------------------------
// Wrapper متوافق مع الكود القديم
// ---------------------------------------------------------------------------
static DWORD WINAPI WelcomeDialogThreadProc(LPVOID)
{
    ShowWelcomeDialog();
    return 0;
}

static void EnsureWelcomeDialog()
{
    static volatile LONG s_bWelcomeShown = 0;
    if (InterlockedCompareExchange(&s_bWelcomeShown, 1, 0) != 0)
        return;

    HANDLE hThread = CreateThread(NULL, 0, WelcomeDialogThreadProc, NULL, 0, NULL);
    if (hThread) {
        CloseHandle(hThread);
    }
}