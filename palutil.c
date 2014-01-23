#ifndef _TEST_

/* 包含头文件 */
#include "palette.h"
#include "palutil.h"

/* 函数实现 */
void palcolormap(BYTE *map, BYTE *srcpal, BYTE *dstpal)
{
    int i;
    for (i=0; i<256; i++)
    {
        map [i] = PALRGB(srcpal,
            dstpal[i * 4 + 2],
            dstpal[i * 4 + 1],
            dstpal[i * 4 + 0]);
    }
}

void alphacolormap(BYTE *map, BYTE *pal, DWORD color)
{
    int fr, fg, fb;
    int br, bg, bb;
    int rr, rg, rb;
    int a, i;

    a  = (color >> 24) & 0xff;
    fr = (color >> 16) & 0xff;
    fg = (color >> 8 ) & 0xff;
    fb = (color >> 0 ) & 0xff;

    for (i=0; i<256; i++)
    {
        br = pal[i * 4 + 2];
        bg = pal[i * 4 + 1];
        bb = pal[i * 4 + 0];

        /* 进行 alpha 混合运算 */
        rr = br + a * (fr - br) / 256;
        rg = bg + a * (fg - bg) / 256;
        rb = bb + a * (fb - bb) / 256;

        map[i] = PALRGB(pal, rr, rg, rb);
    }
}

#else
/* 包含头文件 */
#include "win.h"
#include "bmpfile.h"
#include "draw2d.h"
#include "palutil.h"
int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPreInst, LPSTR lpszCmdLine, int nCmdShow)
{
    void *context;
    BMP   mybmp = {0};
    BYTE  map1[256];
    BYTE  map2[256];
    int   pg1[] = { 123, 128, 323, 50, 483, 88, 540, 235,
                    460, 435, 260, 465, 150, 265 };

    RGE_WIN_INIT(hInst);
    SCREEN.cdepth = 8;
    createbmp(&SCREEN);
    loadbmp(&mybmp, "res/me.bmp", NULL);
    setbmppal(&SCREEN, 0, 256, mybmp.ppal);

    palcolormap(map1, mybmp.ppal, mybmp.ppal);
    alphacolormap(map2, mybmp.ppal, ARGB(128, 255, 0, 0));

    context = paint_begin(&SCREEN);
    setfillstyle(context, FILL_STYLE_PALMAP_BMP);
    setfillbmp  (context, &mybmp);
    setpalmaptab(context, map1);
    polygon(context, pg1, 7);

    setfillstyle(context, FILL_STYLE_PALMAP_SOLID);
    setpalmaptab(context, map2);
    rectangle(context, 100, 100, 520, 420);
    paint_end(context);

    RGE_MSG_LOOP();
    destroybmp(&mybmp);
    destroybmp(&SCREEN);
    return 0;
}
#endif




