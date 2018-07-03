#ifndef _TEST_

/* 包含头文件 */
#include <stdlib.h>
#include "pal.h"
#include "utils.h"

/* 函数实现 */
void palutils_bitmap_palmap(BYTE *palmap, BYTE *dstpal, BYTE *srcpal)
{
    int i;
    for (i=0; i<256; i++) {
        palmap[i] = PALRGB(dstpal, srcpal[i * 4 + 2], srcpal[i * 4 + 1], srcpal[i * 4 + 0]);
    }
}

void palutils_alpha_palmap(BYTE *palmap, BYTE *pal, DWORD color)
{
    int fr, fg, fb;
    int br, bg, bb;
    int rr, rg, rb;
    int a, i;

    a  = (BYTE)(color >> 24);
    fr = (BYTE)(color >> 16);
    fg = (BYTE)(color >> 8 );
    fb = (BYTE)(color >> 0 );

    for (i=0; i<256; i++) {
        br = pal[i * 4 + 2];
        bg = pal[i * 4 + 1];
        bb = pal[i * 4 + 0];

        /* 进行 alpha 混合运算 */
        rr = br + a * (fr - br) / 256;
        rg = bg + a * (fg - bg) / 256;
        rb = bb + a * (fb - bb) / 256;

        palmap[i] = PALRGB(pal, rr, rg, rb);
    }
}

#else
/* 在这里书写测试程序 */
#include <stdlib.h>
#include <conio.h>
#include "screen.h"
#include "draw2d.h"
#include "bitblt.h"
#include "font.h"
#include "utils.h"

void main(void)
{
    void *ctxt;
    BMP   mybmp1 = {0};
    BYTE  palmap1[256];
    BYTE  palmap2[256];

    createbmp(&SCREEN);

    loadfont(&FONT12);
    loadbmp(&mybmp1, "res/me.bmp", NULL);

    palutils_bitmap_palmap(palmap1, SCREEN.ppal, mybmp1.ppal);
    palutils_alpha_palmap (palmap2, SCREEN.ppal, ARGB(128, 0, 0, 255));

    ctxt = draw2d_init(&SCREEN);
    settextfont (ctxt, &FONT12);
    settextcolor(ctxt, RGB888(0, 255, 0));
    paint_begin (ctxt);
    printtext   (ctxt, "调色板映射演示 + 调色板匹配演示");
    paint_done  (ctxt);
    draw2d_free (ctxt);

    putbmp(&SCREEN, 0 , 15, &mybmp1, FS_256_PALMAPSRC, 0, 0, palmap1);
    putbmp(&SCREEN, 60, 60, &mybmp1, FS_256_PALMAPDST, 0, 0, palmap2);
    getch();

    savebmp(&SCREEN, "screen.bmp", NULL);
    getch();

    destroybmp(&mybmp1);
    destroybmp(&SCREEN);
    freefont(&FONT12);
}
#endif




