#ifndef _TEST_

/* 包含头文件 */
#include <stdlib.h>
#include "palette.h"
#include "palutil.h"
#include "matchpal.h"

/* 内部类型定义 */
typedef struct
{
    int  color;
    long freq;
} COLORITEM;

static int color_item_cmp(const void *a, const void *b)
{
    COLORITEM *item1 = (COLORITEM*)a;
    COLORITEM *item2 = (COLORITEM*)b;
    if      (item2->freq - item1->freq > 0) return  1;
    else if (item2->freq - item1->freq < 0) return -1;
    else    return 0;
}

/* 函数实现 */
void matchpal(BMP *pb1, BMP *pb2)
{
    COLORITEM colors[512];
    BYTE      newpal[256*4];
    BYTE      cmap1 [256] = {0};
    BYTE      cmap2 [256] = {0};
    BYTE      *ppal   = NULL;
    BYTE      *pixel  = NULL;
    int        color;
    int        i, j;
    int        black1;
    int        black2;

    /* 该函数仅用于 256 色调色板位图 */
    if (!pb1->ppal || !pb2->ppal) return;

    /* 初始化颜色统计表 */
    for (i=0; i<512; i++)
    {
        colors[i].color = i;
        colors[i].freq  = 0;
    }

    /* 统计颜色分布 */
    for (i=0; i<pb1->stride*pb1->height; i++)
    {
        colors[*(pb1->pdata + i) + 0  ].freq++;
    }
    for (i=0; i<pb2->stride*pb2->height; i++)
    {
        colors[*(pb2->pdata + i) + 256].freq++;
    }

    /* 保证 0 号调色板为黑色 */
    black1 = PALRGB(pb1->ppal, 0, 0, 0);
    black2 = PALRGB(pb2->ppal, 0, 0, 0);
    if (colors[black1].freq > colors[black2 + 256].freq) {
        colors[black1 + 0  ].freq = 0x3fffffff; // 不能使用 0x7fffffff
        colors[black1 + 256].freq = 0;          // 否则在下一步合并重复颜色时会溢出
    }
    else
    {
        colors[black1 + 0  ].freq = 0;
        colors[black2 + 256].freq = 0x3fffffff;
    }

    /* 合并重复颜色 */
    for (i=0; i<256; i++)
    {
        for (j=0; j<256; j++)
        {
            if (  pb1->ppal[i*4+0] == pb2->ppal[j*4+0]
               && pb1->ppal[i*4+1] == pb2->ppal[j*4+1]
               && pb1->ppal[i*4+2] == pb2->ppal[j*4+2] )
            {
                colors[i].freq += colors[j+256].freq;
                colors[j+256].freq = 0;
            }
        }
    }

    /* 对颜色表进行排序 */
    qsort(colors, 512, sizeof(COLORITEM), color_item_cmp);

    /* 生成新调色板并建立匹配的映射 */
    for (i=0; i<256; i++)
    {
        color = colors[i].color;
        if (color < 256)
        {
            ppal = pb1->ppal;
            cmap1[color] = i;
        }
        else
        {
            ppal   = pb2->ppal;
            color -= 256;
            cmap2[color] = i;
        }

        newpal[i * 4 + 0] = ppal[color * 4 + 0];
        newpal[i * 4 + 1] = ppal[color * 4 + 1];
        newpal[i * 4 + 2] = ppal[color * 4 + 2];
    }

    /* 建立未匹配的映射 */
    for (i=0; i<256; i++)
    {
        if (cmap1[i] == 0)
        {
            cmap1[i] = PALRGB(newpal,
                pb1->ppal[i * 4 + 2],
                pb1->ppal[i * 4 + 1],
                pb1->ppal[i * 4 + 0]);
        }

        if (cmap2[i] == 0)
        {
            cmap2[i] = PALRGB(newpal,
                pb2->ppal[i * 4 + 2],
                pb2->ppal[i * 4 + 1],
                pb2->ppal[i * 4 + 0]);
        }
    }

    /* 修改图像数据 */
    pixel = pb1->pdata;
    for (i=0; i<pb1->stride * pb1->height; i++)
    {
        *pixel = cmap1[*pixel];
         pixel++;
    }

    pixel = pb2->pdata;
    for (i=0; i<pb2->stride * pb2->height; i++)
    {
        *pixel = cmap2[*pixel];
         pixel++;
    }

    /* 修改调色板 */
    setbmppal(pb1, 0, 256, newpal);
    setbmppal(pb2, 0, 256, newpal);
}

#else
/* 在这里书写测试程序 */
#include "win.h"
#include "bmpfile.h"
#include "draw2d.h"
#include "bitblt.h"
#include "font.h"
#include "matchpal.h"

static BMP mybmp1 = {0};
static BMP mybmp2 = {0};

LRESULT CALLBACK MyWndProc(
    HWND hwnd,      /* handle to window */
    UINT uMsg,      /* message identifier */
    WPARAM wParam,  /* first message parameter */
    LPARAM lParam   /* second message parameter */
)
{
    static int counter = 0;

    switch (uMsg)
    {
    case WM_KEYDOWN:
        switch (counter)
        {
        case 0:
            setbmppal(&SCREEN, 0, 256, mybmp1.ppal);
            putbmppalmap(&SCREEN, 0, 0, &mybmp1, NULL);
            break;

        case 1:
            setbmppal(&SCREEN, 0, 256, mybmp2.ppal);
            putbmppalmap(&SCREEN, 150, 0, &mybmp2, NULL);
            break;

        case 2:
            matchpal(&mybmp1, &mybmp2);
            setbmppal(&SCREEN, 0, 256, mybmp1.ppal);

            putbmppalmap(&SCREEN, 0  , 0, &mybmp1, NULL);
            putbmppalmap(&SCREEN, 150, 0, &mybmp2, NULL);
            break;

        case 3:
            savebmp(&SCREEN, "screen.bmp", NULL);
            PostMessage(hwnd, WM_CLOSE, 0, 0);
            break;
        }
        counter++;
        return 0;

    default:
        return DEF_SCREEN_WNDPROC(hwnd, uMsg, wParam, lParam);
    }
}

int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPreInst, LPSTR lpszCmdLine, int nCmdShow)
{
    HWND  hwnd;
    void *context;

    RGE_WIN_INIT(hInst);
    SCREEN.cdepth = 8;
    createbmp(&SCREEN);

    hwnd = GET_SCREEN_HWND();
    SetWindowLong(hwnd, GWL_WNDPROC, (long)MyWndProc);

    loadfont(&FONT16);
    loadbmp(&mybmp1, "res/me.bmp",  NULL);
    loadbmp(&mybmp2, "res/boy.bmp", NULL);

    context = paint_begin(&SCREEN);
    settextfont (context, &FONT16);
    settextcolor(context, RGB(255, 255, 255));
    printtext(context, "调色板匹配演示");
    paint_end(context);

    RGE_MSG_LOOP();
    destroybmp(&mybmp1);
    destroybmp(&mybmp2);
    destroybmp(&SCREEN);
    freefont(&FONT16);
    return 0;
}
#endif






