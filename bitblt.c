#ifndef _TEST_

/* 包含头文件 */
#include "drawctxt.h"
#include "bitblt.h"

/* 内部函数实现 */
/* 位块传送的区域裁剪函数 */
static BOOL bltclip(BMP *dstpb, int *dstx, int *dsty,
                    BMP *srcpb, int *srcx, int *srcy,
                    int *w, int *h)
{
    /* 对源图进行裁剪 */
    if (*srcx >= srcpb->width || *srcy >= srcpb->height) return FALSE;
    if (*srcx < 0) {
        *w    += *srcx;
        *dstx -= *srcx;
        *srcx  = 0;
    }
    if (*srcy < 0) {
        *h    += *srcy;
        *dsty -= *srcy;
        *srcy  = 0;
    }

    if (*srcx + *w > srcpb->width)  *w = srcpb->width  - *srcx;
    if (*srcy + *h > srcpb->height) *h = srcpb->height - *srcy;

    /* 对目的图进行裁剪 */
    if (*dstx > dstpb->clipper.right || *dsty >= dstpb->clipper.bottom) return FALSE;
    if (*dstx < dstpb->clipper.left) {
        *w    -= (dstpb->clipper.left - *dstx);
        *srcx += (dstpb->clipper.left - *dstx);
        *dstx  =  dstpb->clipper.left;
    }
    if (*dsty < dstpb->clipper.top) {
        *h    -= (dstpb->clipper.top - *dsty);
        *srcy += (dstpb->clipper.top - *dsty);
        *dsty  =  dstpb->clipper.top;
    }

    if (*w < 0 || *h < 0) return FALSE;
    if (*dstx + *w > dstpb->clipper.right  + 1) *w = dstpb->clipper.right  - *dstx + 1;
    if (*dsty + *h > dstpb->clipper.bottom + 1) *h = dstpb->clipper.bottom - *dsty + 1;

    return TRUE;
}

/* 函数实现 */
void bitblt(BMP *dstpb, int dstx, int dsty,
            BMP *srcpb, int srcx, int srcy, int srcw, int srch,
            int style, int color, int alpha, void *data)
{
    SCANLINEPARAMS params = {0};
    PFNSCANLINE    scanline;
    BYTE *dstp, *srcp;

    if (srcw == -1) srcw = srcpb->width ;
    if (srch == -1) srch = srcpb->height;
    if (!bltclip(dstpb, &dstx, &dsty, srcpb, &srcx, &srcy, &srcw, &srch)) return;

    params.srcbmp   = srcpb;
    params.style    = style;
    params.maskc    = color;
    params.alpha    = alpha;
    params.palmap   = data;
    params.pattern  = data;
    params.filldstx = dstx;
    params.filldsty = dsty;
    select_scanline_func(dstpb, srcpb, style, &scanline);

    if ((style & FS_BMP_MASK) != 0 && color == -1 && srcpb) {
        switch (srcpb->cdepth) {
        case 8 : color = *(BYTE *)srcpb->pdata; break;
        case 16: color = *(WORD *)srcpb->pdata; break;
        case 24: color = *(DWORD*)srcpb->pdata; break;
        case 32: color = *(DWORD*)srcpb->pdata; break;
        }
        if (srcpb->cdepth == 24) color &= 0xffffff;
    } else {
        color = COLOR_CONVERT(dstpb->cdepth, color, TRUE);
    }
    params.fillc = color;

    if (style & FS_AUTO_LOCK) lockbmp(dstpb);
    dstp = (BYTE*)dstpb->pdata + dsty * dstpb->stride + dstx * (dstpb->cdepth / 8);
    srcp = (BYTE*)srcpb->pdata + srcy * srcpb->stride + srcx * (srcpb->cdepth / 8);
    while (srch--) {
        scanline(dstp, srcp, srcw, &params);
        dstp += dstpb->stride;
        srcp += srcpb->stride;
        params.filldstx = dstx;
        params.filldsty+= 1;
    }
    if (dstpb->cdepth == 8 && (style & FS_256_COPYPAL)) {
        setbmppal(dstpb, 0, 256, srcpb->ppal);
    }
    if (style & FS_AUTO_LOCK) unlockbmp(dstpb);
}

#else
/* 包含头文件 */
#include "win.h"
#include "bitblt.h"
int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPreInst, LPSTR lpszCmdLine, int nCmdShow)
{
    BMP mybmp  = {0};
    DWORD pattern[32] = { 0xaaaaaaaa, 0x55555555, 0xaaaaaaaa, 0x55555555, 0xaaaaaaaa, 0x55555555, 0xaaaaaaaa, 0x55555555,
                          0xaaaaaaaa, 0x55555555, 0xaaaaaaaa, 0x55555555, 0xaaaaaaaa, 0x55555555, 0xaaaaaaaa, 0x55555555,
                          0xaaaaaaaa, 0x55555555, 0xaaaaaaaa, 0x55555555, 0xaaaaaaaa, 0x55555555, 0xaaaaaaaa, 0x55555555,
                          0xaaaaaaaa, 0x55555555, 0xaaaaaaaa, 0x55555555, 0xaaaaaaaa, 0x55555555, 0xaaaaaaaa, 0x55555555 };

    if (*lpszCmdLine == 0) {
        lpszCmdLine = "res/boy332.bmp";
    }

    RGE_WIN_INIT(hInst);
    SCREEN.cdepth = 32;
    createbmp(&SCREEN);
    loadbmp(&mybmp, lpszCmdLine, NULL);

    putbmp(&SCREEN,
           (SCREEN.width  - mybmp.width ) / 2,
           (SCREEN.height - mybmp.height) / 2,
           &mybmp, FS_AUTO_LOCK|FS_BMP_MASK|FS_BMP_ALPHA, -1, 128, NULL);

    putbmp(&SCREEN, 0, 0, &mybmp, FS_AUTO_LOCK|FS_PATTERN, RGB888(0,255,0), 0, pattern);

    RGE_MSG_LOOP();
    destroybmp(&mybmp);
    destroybmp(&SCREEN);
    return 0;
}
#endif



