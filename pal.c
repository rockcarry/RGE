#ifndef _TEST_

/* 包含头文件 */
#include <stdlib.h>
#include "pal.h"

#if 0
/* 全局变量声明定义 */
/* 256 色标准调色板 */
BYTE STDPAL[] = {
      0,   0,   0,   0,   0,  85,   0,   0, 170,   0,   0, 255, 
      0,  36,   0,   0,  36,  85,   0,  36, 170,   0,  36, 255, 
      0,  72,   0,   0,  72,  85,   0,  72, 170,   0,  72, 255, 
      0, 108,   0,   0, 108,  85,   0, 108, 170,   0, 108, 255, 
      0, 144,   0,   0, 144,  85,   0, 144, 170,   0, 144, 255, 
      0, 180,   0,   0, 180,  85,   0, 180, 170,   0, 180, 255, 
      0, 216,   0,   0, 216,  85,   0, 216, 170,   0, 216, 255, 
      0, 252,   0,   0, 252,  85,   0, 252, 170,   0, 252, 255, 
     36,   0,   0,  36,   0,  85,  36,   0, 170,  36,   0, 255, 
     36,  36,   0,  36,  36,  85,  36,  36, 170,  36,  36, 255, 
     36,  72,   0,  36,  72,  85,  36,  72, 170,  36,  72, 255, 
     36, 108,   0,  36, 108,  85,  36, 108, 170,  36, 108, 255, 
     36, 144,   0,  36, 144,  85,  36, 144, 170,  36, 144, 255, 
     36, 180,   0,  36, 180,  85,  36, 180, 170,  36, 180, 255, 
     36, 216,   0,  36, 216,  85,  36, 216, 170,  36, 216, 255, 
     36, 252,   0,  36, 252,  85,  36, 252, 170,  36, 252, 255, 
     72,   0,   0,  72,   0,  85,  72,   0, 170,  72,   0, 255, 
     72,  36,   0,  72,  36,  85,  72,  36, 170,  72,  36, 255, 
     72,  72,   0,  72,  72,  85,  72,  72, 170,  72,  72, 255, 
     72, 108,   0,  72, 108,  85,  72, 108, 170,  72, 108, 255, 
     72, 144,   0,  72, 144,  85,  72, 144, 170,  72, 144, 255, 
     72, 180,   0,  72, 180,  85,  72, 180, 170,  72, 180, 255, 
     72, 216,   0,  72, 216,  85,  72, 216, 170,  72, 216, 255, 
     72, 252,   0,  72, 252,  85,  72, 252, 170,  72, 252, 255, 
    108,   0,   0, 108,   0,  85, 108,   0, 170, 108,   0, 255, 
    108,  36,   0, 108,  36,  85, 108,  36, 170, 108,  36, 255, 
    108,  72,   0, 108,  72,  85, 108,  72, 170, 108,  72, 255, 
    108, 108,   0, 108, 108,  85, 108, 108, 170, 108, 108, 255, 
    108, 144,   0, 108, 144,  85, 108, 144, 170, 108, 144, 255, 
    108, 180,   0, 108, 180,  85, 108, 180, 170, 108, 180, 255, 
    108, 216,   0, 108, 216,  85, 108, 216, 170, 108, 216, 255, 
    108, 252,   0, 108, 252,  85, 108, 252, 170, 108, 252, 255, 
    144,   0,   0, 144,   0,  85, 144,   0, 170, 144,   0, 255, 
    144,  36,   0, 144,  36,  85, 144,  36, 170, 144,  36, 255, 
    144,  72,   0, 144,  72,  85, 144,  72, 170, 144,  72, 255, 
    144, 108,   0, 144, 108,  85, 144, 108, 170, 144, 108, 255, 
    144, 144,   0, 144, 144,  85, 144, 144, 170, 144, 144, 255, 
    144, 180,   0, 144, 180,  85, 144, 180, 170, 144, 180, 255, 
    144, 216,   0, 144, 216,  85, 144, 216, 170, 144, 216, 255, 
    144, 252,   0, 144, 252,  85, 144, 252, 170, 144, 252, 255, 
    180,   0,   0, 180,   0,  85, 180,   0, 170, 180,   0, 255, 
    180,  36,   0, 180,  36,  85, 180,  36, 170, 180,  36, 255, 
    180,  72,   0, 180,  72,  85, 180,  72, 170, 180,  72, 255, 
    180, 108,   0, 180, 108,  85, 180, 108, 170, 180, 108, 255, 
    180, 144,   0, 180, 144,  85, 180, 144, 170, 180, 144, 255, 
    180, 180,   0, 180, 180,  85, 180, 180, 170, 180, 180, 255, 
    180, 216,   0, 180, 216,  85, 180, 216, 170, 180, 216, 255, 
    180, 252,   0, 180, 252,  85, 180, 252, 170, 180, 252, 255, 
    216,   0,   0, 216,   0,  85, 216,   0, 170, 216,   0, 255, 
    216,  36,   0, 216,  36,  85, 216,  36, 170, 216,  36, 255, 
    216,  72,   0, 216,  72,  85, 216,  72, 170, 216,  72, 255, 
    216, 108,   0, 216, 108,  85, 216, 108, 170, 216, 108, 255, 
    216, 144,   0, 216, 144,  85, 216, 144, 170, 216, 144, 255, 
    216, 180,   0, 216, 180,  85, 216, 180, 170, 216, 180, 255, 
    216, 216,   0, 216, 216,  85, 216, 216, 170, 216, 216, 255, 
    216, 252,   0, 216, 252,  85, 216, 252, 170, 216, 252, 255, 
    252,   0,   0, 252,   0,  85, 252,   0, 170, 252,   0, 255, 
    252,  36,   0, 252,  36,  85, 252,  36, 170, 252,  36, 255, 
    252,  72,   0, 252,  72,  85, 252,  72, 170, 252,  72, 255, 
    252, 108,   0, 252, 108,  85, 252, 108, 170, 252, 108, 255, 
    252, 144,   0, 252, 144,  85, 252, 144, 170, 252, 144, 255, 
    252, 180,   0, 252, 180,  85, 252, 180, 170, 252, 180, 255, 
    252, 216,   0, 252, 216,  85, 252, 216, 170, 252, 216, 255, 
    252, 252,   0, 252, 252,  85, 252, 252, 170, 252, 252, 255, 
};
#endif

/*
  note: 所有调色板操作都保证 0 号调色板不变
 */
/* 创建 256 色标准调色板 */
void createstdpal(BYTE *pal)
{
    int r, g, b;
    if (!pal) return;
    for (r=0; r<256; r+=36) {
        for (g=0; g<256; g+=36) {
            for (b=0; b<256; b+=85) {
                *pal++ = b;
                *pal++ = g;
                *pal++ = r;
                *pal++ = 0;
            }
        }
    }
}

/* 调色板逼近 */
int approachpal(BYTE *pal1, BYTE *pal2) /* pal1 -> pal2 */
{
    int ret, i;

    if (!pal1 || !pal2) return -1;

    pal1 += 4;  /* 跳过 0 号调色板 */
    pal2 += 4;  /* 跳过 0 号调色板 */
    ret   = 0;

    for (i=4; i<256*4; i++) {
        if      (*pal1 > *pal2) (*pal1)--;
        else if (*pal1 < *pal2) (*pal1)++;
        else    ret++;
        pal1++; pal2++;
    }
    return ret;
}

/* 向右旋转调色板 */
void rightrotpal(BYTE *pal)
{
    BYTE r, g, b;
    int  i;

    if (!pal) return;

    b = *(pal + (256 - 1) * 4 + 0);
    g = *(pal + (256 - 1) * 4 + 1);
    r = *(pal + (256 - 1) * 4 + 2);

    pal += (256 - 1) * 4;
    for (i=(256-1); i>=2; i--) {
        *(pal + 0) = *(pal + 0 - 4);
        *(pal + 1) = *(pal + 1 - 4);
        *(pal + 2) = *(pal + 2 - 4);
        pal -= 4;
    }

    *(pal + 0) = b;
    *(pal + 1) = g;
    *(pal + 2) = r;
}

/* 向左旋转调色板 */
void leftrotpal(BYTE *pal)
{
    BYTE r, g, b;
    int  i;

    if (!pal) return;

    b = *(pal + 1 * 4 + 0);
    g = *(pal + 1 * 4 + 1);
    r = *(pal + 1 * 4 + 2);

    pal += 4;  /* 跳过 0 号调色板 */
    for (i=1; i<=(256-2); i++) {
        *(pal + 0) = *(pal + 0 + 4);
        *(pal + 1) = *(pal + 1 + 4);
        *(pal + 2) = *(pal + 2 + 4);
        pal += 4;
    }

    *(pal + 0) = b;
    *(pal + 1) = g;
    *(pal + 2) = r;
}

/* 随机化调色板 */
void randpal(BYTE *pal)
{
    BYTE m = 1 + rand() % (256 - 1);
    BYTE n = 1 + rand() % (256 - 1);
    BYTE temp;
    int  i;

    if (!pal) return;

    for (i=0; i<4; i++) {
        temp = *(pal + m * 4 + i);
        *(pal + m * 4 + i) = *(pal + n * 4 + i);
        *(pal + n * 4 + i) = temp;
    }
}

/* 灰度化调色板 */
void graypal(BYTE *pal)
{
    BYTE gray;
    int  i;

    if (!pal) return;

    pal += 4;  /* 跳过 0 号调色板 */
    for (i=1; i<256; i++) {
        gray = (BYTE)(*(pal + 2) * 0.299 + *(pal + 1) * 0.587 + *(pal + 0) * 0.114 + 0.5);
        *(pal + 0) = *(pal + 1) = *(pal + 2) = gray;
        pal += 4;
    }
}

/* swap palette */
void swappal(BYTE *pal, int order)
{
    BYTE r, g, b;
    int  i;

    if (!pal) return;

    pal += 4;  /* 跳过 0 号调色板 */
    switch (order)
    {
    case SWAP_PAL_RBG:
        for (i=1; i<256; i++) {
            r = *(pal + 2);
            g = *(pal + 1);
            b = *(pal + 0);
            *(pal + 2) = r;
            *(pal + 1) = b;
            *(pal + 0) = g;
            pal += 4;
        }
        break;
    case SWAP_PAL_GRB:
        for (i=1; i<256; i++) {
            r = *(pal + 2);
            g = *(pal + 1);
            b = *(pal + 0);
            *(pal + 2) = g;
            *(pal + 1) = r;
            *(pal + 0) = b;
            pal += 4;
        }
        break;
    case SWAP_PAL_GBR:
        for (i=1; i<256; i++) {
            r = *(pal + 2);
            g = *(pal + 1);
            b = *(pal + 0);
            *(pal + 2) = g;
            *(pal + 1) = b;
            *(pal + 0) = r;
            pal += 4;
        }
        break;
    case SWAP_PAL_BRG:
        for (i=1; i<256; i++) {
            r = *(pal + 2);
            g = *(pal + 1);
            b = *(pal + 0);
            *(pal + 2) = b;
            *(pal + 1) = r;
            *(pal + 0) = g;
            pal += 4;
        }
        break;
    case SWAP_PAL_BGR:
        for (i=1; i<256; i++) {
            r = *(pal + 2);
            g = *(pal + 1);
            b = *(pal + 0);
            *(pal + 2) = b;
            *(pal + 1) = g;
            *(pal + 0) = r;
            pal += 4;
        }
        break;
    }
}

/* 匹配调色板中最接近的颜色 */
BYTE PALRGB(BYTE *pal, BYTE r, BYTE g, BYTE b)
{
    long mind;
    long newd;
    BYTE color;
    int  i;

    if (!pal) return 0;

    color = 0;
    mind  = (pal[0] - b) * (pal[0] - b) + (pal[1] - g) * (pal[1] - g) + (pal[2] - r) * (pal[2] - r);
    pal  += 4;

    for (i=1; i<256; i++) {
        newd = (pal[0] - b) * (pal[0] - b) + (pal[1] - g) * (pal[1] - g) + (pal[2] - r) * (pal[2] - r);
        if (newd < mind) {
            mind  = newd;
            color = i;
        }
        pal += 4;
    }

    return color;
}

#else
#include "pal.h"
#include "screen.h"

LRESULT CALLBACK MyWndProc(
    HWND hwnd,      /* handle to window */
    UINT uMsg,      /* message identifier */
    WPARAM wParam,  /* first message parameter */
    LPARAM lParam   /* second message parameter */
)
{
    BYTE pal[256*4];
    switch (uMsg)
    {
    case WM_TIMER:
        lockbmp(&SCREEN);
        getbmppal(&SCREEN, 0, 256, pal);
        rightrotpal(pal);
        setbmppal(&SCREEN, 0, 256, pal);
        unlockbmp(&SCREEN);
        return 0;

    default:
        return DEF_SCREEN_WNDPROC(hwnd, uMsg, wParam, lParam);
    }
}

int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPreInst, LPSTR lpszCmdLine, int nCmdShow)
{
    HWND hwnd;
    int  i, j;

    RGE_WIN_INIT(hInst);
    SCREEN.cdepth = 8;
    createbmp(&SCREEN);

    hwnd = GET_SCREEN_HWND();
    SetWindowLong(hwnd, GWL_WNDPROC, (long)MyWndProc);
    SetTimer(hwnd, 1, 50, NULL);

    lockbmp(&SCREEN);
    for (i=0; i<SCREEN.height; i++)
        for (j=0; j<SCREEN.width; j++)
            *((BYTE*)SCREEN.pdata + i * SCREEN.stride + j) = RGB332(i, j, i);
    unlockbmp(&SCREEN);

    RGE_MSG_LOOP();
    KillTimer(hwnd, 1);
    destroybmp(&SCREEN);
    return 0;
}
#endif


