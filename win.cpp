#ifndef _TEST_

/* 包含头文件 */
extern "C" {
#include "win.h"
}

/* 全局变量 */
static HINSTANCE RGE_APP_INSTANCE = NULL;

/* 函数实现 */
void RGE_WIN_INIT(HINSTANCE hInst)
{
    RGE_APP_INSTANCE = hInst;
}

int RGE_MSG_LOOP(void)
{
    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage (&msg);
    }
    return msg.wParam;
}

HINSTANCE RGE_GET_APP_INSTANCE(void)
{
    return RGE_APP_INSTANCE;
}

/*
  WINSCREEN 实现
 */

/* 常量定义 */
#define RGE_WND_CLASS "RGEWndClass"
#define RGE_WND_NAME  "RGE"

/* 类型定义 */
typedef struct
{
    HWND    hwnd;
    DWORD   style;
    WNDPROC wndproc;
    HBITMAP hbmp;
    HDC     hdc;
} BMP_EXTRA_WIN;

/* 内部函数声明 */
static BOOL _windrv_createbmp (void *pb);
static void _windrv_destroybmp(void *pb);
static void _windrv_lock  (void *pb);
static void _windrv_unlock(void *pb);
static void _windrv_setpal(void *pb, int i, int n, BYTE *pal);
static void _windrv_getpal(void *pb, int i, int n, BYTE *pal);

/* 全局变量定义 */
static BMPDRV WINSCREEN_DRV =
{
    _windrv_createbmp,
    _windrv_destroybmp,
    _windrv_lock,
    _windrv_unlock,
    _windrv_setpal,
    _windrv_getpal,
};

static BMP_EXTRA_WIN WINSCREEN_EXTRA =
{
    NULL,
    WS_SYSMENU,
    DEF_WINSCREEN_WNDPROC,
};

BMP WINSCREEN =
{
    &WINSCREEN_DRV,
    0, 0, 0, 0, {0, 0, 0, 0}, NULL, NULL,
    &WINSCREEN_EXTRA,
};

/* 内部函数实现 */
static BOOL _windrv_createbmp(void *pb)
{
    BMP           *pbmp   = (BMP*)pb;
    BMP_EXTRA_WIN *pextra = (BMP_EXTRA_WIN*)pbmp->pextra;
    WNDCLASS wc  = {0};
    RECT cltrect = {0};
    int  x, y, w, h;

    BITMAP      bitmap  = {0};
    BYTE        buffer[sizeof(BITMAPINFOHEADER) + sizeof(DWORD) * 256] = {0};
    BITMAPINFO *bmpinfo = (BITMAPINFO*)buffer;

    wc.lpfnWndProc   = pextra->wndproc;
    wc.hInstance     = RGE_APP_INSTANCE;
    wc.hIcon         = LoadIcon  (NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
    wc.lpszClassName = RGE_WND_CLASS;
    if (!RegisterClass(&wc)) return FALSE;

    bmpinfo->bmiHeader.biSize     =  sizeof(BITMAPINFOHEADER);
    bmpinfo->bmiHeader.biWidth    =  pbmp->width;
    bmpinfo->bmiHeader.biHeight   = -pbmp->height;
    bmpinfo->bmiHeader.biPlanes   =  1;
    bmpinfo->bmiHeader.biBitCount =  pbmp->cdepth;

    switch (pbmp->cdepth)
    {
    case 8:
        bmpinfo->bmiHeader.biCompression = BI_RGB;
        memcpy((BYTE*)bmpinfo->bmiColors, pbmp->ppal, 256 * 4);
        break;

    case 16:
        bmpinfo->bmiHeader.biCompression = BI_BITFIELDS;
        ((DWORD*)bmpinfo->bmiColors)[0]  = 0xF800;
        ((DWORD*)bmpinfo->bmiColors)[1]  = 0x07E0;
        ((DWORD*)bmpinfo->bmiColors)[2]  = 0x001F;
        break;

    default:
        bmpinfo->bmiHeader.biCompression = BI_RGB;
        break;
    }

    pextra->hdc  = CreateCompatibleDC(NULL);
    pextra->hbmp = CreateDIBSection(pextra->hdc, bmpinfo, DIB_RGB_COLORS, (void**)&(pbmp->pdata), NULL, 0);
    if (!pextra->hdc || !pextra->hbmp || !pbmp->pdata) return FALSE;

    GetObject(pextra->hbmp, sizeof(BITMAP), &bitmap);
    pbmp->stride = bitmap.bmWidthBytes;
    SelectObject(pextra->hdc, pextra->hbmp);

    pextra->hwnd = CreateWindow(RGE_WND_CLASS, RGE_WND_NAME, pextra->style,
        CW_USEDEFAULT, CW_USEDEFAULT, pbmp->width, pbmp->height,
        NULL, NULL, RGE_APP_INSTANCE, NULL);
    if (!pextra->hwnd) return FALSE;

    GetClientRect(pextra->hwnd, &cltrect);
    w = pbmp->width  + (pbmp->width  - cltrect.right);
    h = pbmp->height + (pbmp->height - cltrect.bottom);
    x = (GetSystemMetrics(SM_CXSCREEN) - w) / 2;
    y = (GetSystemMetrics(SM_CYSCREEN) - h) / 2;
    x = x > 0 ? x : 0;
    y = y > 0 ? y : 0;

    MoveWindow(pextra->hwnd, x, y, w, h, FALSE);
    ShowWindow(pextra->hwnd, SW_SHOW);
    UpdateWindow(pextra->hwnd);

    return TRUE;
}

static void _windrv_destroybmp(void *pb)
{
    BMP           *pbmp   = (BMP*)pb;
    BMP_EXTRA_WIN *pextra = (BMP_EXTRA_WIN*)pbmp->pextra;
    DestroyWindow(pextra->hwnd);

    if (pextra->hdc ) DeleteDC(pextra->hdc);
    if (pextra->hbmp) DeleteObject(pextra->hbmp);

    pextra->hwnd = NULL;
    pextra->hdc  = NULL;
    pextra->hbmp = NULL;
}

static void _windrv_lock(void *pb)
{
}

static void _windrv_unlock(void *pb)
{
    BMP           *pbmp   = (BMP*)pb;
    BMP_EXTRA_WIN *pextra = (BMP_EXTRA_WIN*)pbmp->pextra;
    InvalidateRect(pextra->hwnd, NULL, FALSE);
}

static void _windrv_setpal(void *pb, int i, int n, BYTE *pal)
{
    BMP           *pbmp   = (BMP*)pb;
    BMP_EXTRA_WIN *pextra = (BMP_EXTRA_WIN*)pbmp->pextra;
    SetDIBColorTable(pextra->hdc, i, n, (RGBQUAD*)pal);
}

static void _windrv_getpal(void *pb, int i, int n, BYTE *pal)
{
    BMP           *pbmp   = (BMP*)pb;
    BMP_EXTRA_WIN *pextra = (BMP_EXTRA_WIN*)pbmp->pextra;
    GetDIBColorTable(pextra->hdc, i, n, (RGBQUAD*)pal);
}

/* 函数实现 */
HWND GET_WINSCREEN_HWND(void)
{
    BMP_EXTRA_WIN *pextra = (BMP_EXTRA_WIN*)WINSCREEN.pextra;
    return pextra->hwnd;
}

LRESULT CALLBACK DEF_WINSCREEN_WNDPROC(
    HWND hwnd,      /* handle to window */
    UINT uMsg,      /* message identifier */
    WPARAM wParam,  /* first message parameter */
    LPARAM lParam   /* second message parameter */
)
{
    PAINTSTRUCT ps = {0};
    HDC        hdc = NULL;

    switch (uMsg)
    {
    case WM_PAINT:
        hdc = BeginPaint(hwnd, &ps);
        BitBlt(hdc,
            ps.rcPaint.left,
            ps.rcPaint.top,
            ps.rcPaint.right  - ps.rcPaint.left,
            ps.rcPaint.bottom - ps.rcPaint.top,
            ((BMP_EXTRA_WIN*)WINSCREEN.pextra)->hdc,
            ps.rcPaint.left,
            ps.rcPaint.top,
            SRCCOPY);
        EndPaint(hwnd, &ps);
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

#ifdef ENABLE_WIN32_DDRAW
/*
  DDRAWSCREEN 实现
 */
#include "dx/ddraw.h"

/* 类型定义 */
typedef struct
{
    HWND    hwnd;
    DWORD   style;
    WNDPROC wndproc;
    IDirectDraw        *lpDirectDraw;
    IDirectDrawSurface *lpDDSPrimary;
    LPDIRECTDRAWPALETTE lpDDPalette;
} BMP_EXTRA_DDRAW;

/* 内部函数声明 */
static BOOL _ddrawdrv_createbmp (void *pb);
static void _ddrawdrv_destroybmp(void *pb);
static void _ddrawdrv_lock  (void *pb);
static void _ddrawdrv_unlock(void *pb);
static void _ddrawdrv_setpal(void *pb, int i, int n, BYTE *pal);
static void _ddrawdrv_getpal(void *pb, int i, int n, BYTE *pal);

/* 全局变量定义 */
static BMPDRV DDRAWSCREEN_DRV =
{
    _ddrawdrv_createbmp,
    _ddrawdrv_destroybmp,
    _ddrawdrv_lock,
    _ddrawdrv_unlock,
    _ddrawdrv_setpal,
    _ddrawdrv_getpal,
};

static BMP_EXTRA_DDRAW DDRAWSCREEN_EXTRA =
{
    NULL,
    WS_POPUP,
    DEF_DDRAWSCREEN_WNDPROC,
};

BMP DDRAWSCREEN =
{
    &DDRAWSCREEN_DRV,
    0, 0, 0, 0, {0, 0, 0, 0}, NULL, NULL,
    &DDRAWSCREEN_EXTRA,
};

/* 内部函数实现 */
static BOOL _ddrawdrv_createbmp(void *pb)
{
    BMP             *pbmp   = (BMP*)pb;
    BMP_EXTRA_DDRAW *pextra = (BMP_EXTRA_DDRAW*)pbmp->pextra;
    WNDCLASS wc = {0};

    wc.lpfnWndProc   = pextra->wndproc;
    wc.hInstance     = RGE_APP_INSTANCE;
    wc.hIcon         = LoadIcon  (NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
    wc.lpszClassName = RGE_WND_CLASS;
    if (!RegisterClass(&wc)) return FALSE;

    pextra->hwnd = CreateWindow(RGE_WND_CLASS, RGE_WND_NAME, pextra->style,
        CW_USEDEFAULT, CW_USEDEFAULT, pbmp->width, pbmp->height,
        NULL, NULL, RGE_APP_INSTANCE, NULL);
    if (!pextra->hwnd) return FALSE;

    ShowWindow(pextra->hwnd, SW_SHOW);
    UpdateWindow(pextra->hwnd);

    /* ++ create direct draw & primary surface ++ */
    DirectDrawCreate(NULL, &pextra->lpDirectDraw, NULL);
    if (pextra->lpDirectDraw == NULL) return FALSE;
    pextra->lpDirectDraw->SetCooperativeLevel(pextra->hwnd, DDSCL_FULLSCREEN|DDSCL_EXCLUSIVE);
    pextra->lpDirectDraw->SetDisplayMode(pbmp->width, pbmp->height, pbmp->cdepth);

    DDSURFACEDESC ddsd = {0};
    ddsd.dwSize  = sizeof(ddsd);
    ddsd.dwFlags = DDSD_CAPS;
    ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
    pextra->lpDirectDraw->CreateSurface(&ddsd, &pextra->lpDDSPrimary, NULL);
    if (pextra->lpDDSPrimary == NULL) return false;
    /* -- create direct draw & primary surface -- */

    /* ++ for 256-colors ++ */
    if (pbmp->cdepth == 8) {
        BYTE swp[256*4];
        int  i;
        for (i=0; i<256; i++)
        {
            swp[i*4 + 0] = pbmp->ppal[i*4 + 2];
            swp[i*4 + 1] = pbmp->ppal[i*4 + 1];
            swp[i*4 + 2] = pbmp->ppal[i*4 + 0];
        }

        pextra->lpDirectDraw->CreatePalette(DDPCAPS_8BIT|DDPCAPS_ALLOW256|DDPCAPS_INITIALIZE,
            (LPPALETTEENTRY)swp, &pextra->lpDDPalette, NULL);
        pextra->lpDDSPrimary->SetPalette(pextra->lpDDPalette);
    }
    /* -- for 256-colors -- */

    /* ++ lock & unlock to obtain address & stride ++ */
    _ddrawdrv_lock(pb);
    _ddrawdrv_unlock(pb);
    /* -- lock & unlock to obtain address & stride -- */
    return TRUE;
}

static void _ddrawdrv_destroybmp(void *pb)
{
    BMP             *pbmp   = (BMP*)pb;
    BMP_EXTRA_DDRAW *pextra = (BMP_EXTRA_DDRAW*)pbmp->pextra;

    /* release direct draw */
    if (pextra->lpDDPalette ) pextra->lpDDPalette ->Release();
    if (pextra->lpDDSPrimary) pextra->lpDDSPrimary->Release();
    if (pextra->lpDirectDraw) pextra->lpDirectDraw->Release();

    /* destroy window */
    DestroyWindow(pextra->hwnd);

    pextra->hwnd         = NULL;
    pextra->lpDirectDraw = NULL;
    pextra->lpDDSPrimary = NULL;
    pextra->lpDDPalette  = NULL;
}

static void _ddrawdrv_lock(void *pb)
{
    BMP             *pbmp   = (BMP*)pb;
    BMP_EXTRA_DDRAW *pextra = (BMP_EXTRA_DDRAW*)pbmp->pextra;
    DDSURFACEDESC    ddsd   = {sizeof(ddsd)};

    pextra->lpDDSPrimary->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL);
    pbmp->pdata  = (BYTE*)ddsd.lpSurface;
    pbmp->stride = ddsd.lPitch;
}

static void _ddrawdrv_unlock(void *pb)
{
    BMP             *pbmp   = (BMP*)pb;
    BMP_EXTRA_DDRAW *pextra = (BMP_EXTRA_DDRAW*)pbmp->pextra;
    pextra->lpDDSPrimary->Unlock(NULL);
}

static void _ddrawdrv_setpal(void *pb, int i, int n, BYTE *pal)
{
    BMP             *pbmp   = (BMP*)pb;
    BMP_EXTRA_DDRAW *pextra = (BMP_EXTRA_DDRAW*)pbmp->pextra;
    BYTE             swp[256*4];
    int              j;

    for (j=0; j<n; j++)
    {
        swp[j*4 + 0] = pal[j*4 + 2];
        swp[j*4 + 1] = pal[j*4 + 1];
        swp[j*4 + 2] = pal[j*4 + 0];
    }
    if (pextra->lpDDPalette) pextra->lpDDPalette->SetEntries(0, i, n, (LPPALETTEENTRY)swp);
}

static void _ddrawdrv_getpal(void *pb, int i, int n, BYTE *pal)
{
    BMP             *pbmp   = (BMP*)pb;
    BMP_EXTRA_DDRAW *pextra = (BMP_EXTRA_DDRAW*)pbmp->pextra;
    BYTE             swp[256*4];
    int              j;

    if (pextra->lpDDPalette) pextra->lpDDPalette->GetEntries(0, i, n, (LPPALETTEENTRY)swp);
    for (j=0; j<n; j++)
    {
        pal[j*4 + 0] = swp[j*4 + 2];
        pal[j*4 + 1] = swp[j*4 + 1];
        pal[j*4 + 2] = swp[j*4 + 0];
    }
}

LRESULT CALLBACK DEF_DDRAWSCREEN_WNDPROC(
    HWND hwnd,      /* handle to window */
    UINT uMsg,      /* message identifier */
    WPARAM wParam,  /* first message parameter */
    LPARAM lParam   /* second message parameter */
)
{
    switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_KEYDOWN:
        if ((int)wParam == VK_ESCAPE) {
            PostQuitMessage(0);
        }
        return 0;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

HWND GET_DDRAWSCREEN_HWND(void)
{
    BMP_EXTRA_WIN *pextra = (BMP_EXTRA_WIN*)DDRAWSCREEN.pextra;
    return pextra->hwnd;
}
#endif

#else
/* 包含头文件 */
extern "C" {
#include "win.h"
}
int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPreInst, LPSTR lpszCmdLine, int nCmdShow)
{
    int  i, j;

    RGE_WIN_INIT(hInst);
    SCREEN.cdepth = 16;
    createbmp(&SCREEN);

    #define RGB555(r, g, b)  ( (((r) & 0xF8) <<  7) | (((g) & 0xF8) << 2) | (((b) & 0xF8) >> 3) )
    #define RGB565(r, g, b)  ( (((r) & 0xF8) <<  8) | (((g) & 0xFC) << 3) | (((b) & 0xF8) >> 3) )
    lockbmp(&SCREEN);
    for (i=0; i<SCREEN.height; i++)
        for (j=0; j<SCREEN.width; j++)
            *(WORD*)(SCREEN.pdata + i * SCREEN.stride + j * 2) = RGB565(i, j, i);
    unlockbmp(&SCREEN);

    RGE_MSG_LOOP();
    destroybmp(&SCREEN);
    return 0;
}
#endif


