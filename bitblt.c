#ifndef _TEST_

/* 包含头文件 */
#include "fnps/fnps.h"
#include "bitblt.h"

/* 内部函数实现 */
/* 位块传送的区域裁剪函数 */
static BOOL bltclip(BMP *dstpb, int *dstx, int *dsty,
                    BMP *srcpb, int *srcx, int *srcy,
                    int *w, int *h)
{
    /* 对源图进行裁剪 */
    if (*srcx >= srcpb->width || *srcy >= srcpb->height) return FALSE;
    if (*srcx < 0)
    {
        *w    += *srcx;
        *dstx -= *srcx;
        *srcx  = 0;
    }
    if (*srcy < 0)
    {
        *h    += *srcy;
        *dsty -= *srcy;
        *srcy  = 0;
    }

    if (*srcx + *w > srcpb->width)  *w = srcpb->width  - *srcx;
    if (*srcy + *h > srcpb->height) *h = srcpb->height - *srcy;

    /* 对目的图进行裁剪 */
    if (*dstx > dstpb->clipper.right || *dsty >= dstpb->clipper.bottom) return FALSE;
    if (*dstx < dstpb->clipper.left)
    {
        *w    -= (dstpb->clipper.left - *dstx);
        *srcx += (dstpb->clipper.left - *dstx);
        *dstx  =  dstpb->clipper.left;
    }
    if (*dsty < dstpb->clipper.top)
    {
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
void bitbltcopy(BMP *dstpb, int dstx, int dsty,
                BMP *srcpb, int srcx, int srcy, int srcw, int srch)
{
    PFNSCANLINE scanline;
    DRAWCONTEXT ctxt;
    int dstw, dsth;

    if (srcw == -1) srcw = srcpb->width;
    if (srch == -1) srch = srcpb->height;

    /* 进行裁剪处理 */
    if (!bltclip(dstpb, &dstx, &dsty, srcpb, &srcx, &srcy, &srcw, &srch)) return;

    if (dstpb->cdepth == srcpb->cdepth) {
        scanline = TABFN_SCANLINEBMP_FAST[dstpb->cdepth / 8];
    }
    else {
        scanline = TABFN_SCANLINEBMP_CONVERT[dstpb->cdepth / 8];
    }

    ctxt.fillbmp  = srcpb;
    ctxt.fillorgx = -dstx;
    ctxt.fillorgy = -dsty;

    /* 绘制扫描线 */
    dstw = srcw;
    dsth = srch;
    while (dsth--) scanline(dstpb, dsty++, dstx, dstx + dstw - 1, (DWORD)&ctxt);
}

void bitbltalpha(BMP *dstpb, int dstx, int dsty,
                     BMP *srcpb, int srcx, int srcy, int srcw, int srch, int alpha)
{
    PFNSCANLINE scanline;
    DRAWCONTEXT ctxt;
    int dstw, dsth;

    if (srcw == -1) srcw = srcpb->width;
    if (srch == -1) srch = srcpb->height;

    /* 进行裁剪处理 */
    if (!bltclip(dstpb, &dstx, &dsty, srcpb, &srcx, &srcy, &srcw, &srch)) return;

    scanline = TABFN_SCANLINEBMP_ALPHA[dstpb->cdepth / 8];
    ctxt.fillbmp  = srcpb;
    ctxt.fillorgx = -dstx;
    ctxt.fillorgy = -dsty;
    ctxt.alpha    = alpha;

    /* 绘制扫描线 */
    dstw = srcw;
    dsth = srch;
    while (dsth--) scanline(dstpb, dsty++, dstx, dstx + dstw - 1, (DWORD)&ctxt);
}

void bitbltmaskalpha(BMP *dstpb, int dstx, int dsty,
                     BMP *srcpb, int srcx, int srcy, int srcw, int srch, DWORD maskc, int alpha)
{
    PFNSCANLINE scanline;
    DRAWCONTEXT ctxt;
    int dstw, dsth;

    if (srcw == -1) srcw = srcpb->width;
    if (srch == -1) srch = srcpb->height;

    /* 进行裁剪处理 */
    if (!bltclip(dstpb, &dstx, &dsty, srcpb, &srcx, &srcy, &srcw, &srch)) return;

    scanline = TABFN_SCANLINEBMP_MASKALPHA[dstpb->cdepth / 8];
    ctxt.fillbmp  = srcpb;
    ctxt.fillorgx = -dstx;
    ctxt.fillorgy = -dsty;
    ctxt.maskc    = maskc;
    ctxt.alpha    = alpha;

    /* 绘制扫描线 */
    dstw = srcw;
    dsth = srch;
    while (dsth--) scanline(dstpb, dsty++, dstx, dstx + dstw - 1, (DWORD)&ctxt);
}

void bitbltpalmap(BMP *dstpb, int dstx, int dsty,
                  BMP *srcpb, int srcx, int srcy, int srcw, int srch, BYTE *palmap)
{
    PFNSCANLINE scanline;
    DRAWCONTEXT ctxt;
    int dstw, dsth;

    if (srcw == -1) srcw = srcpb->width;
    if (srch == -1) srch = srcpb->height;

    /* 进行裁剪处理 */
    if (!bltclip(dstpb, &dstx, &dsty, srcpb, &srcx, &srcy, &srcw, &srch)) return;

    scanline = PFN_SCANLINE_PALMAP_BMP;
    ctxt.fillbmp  = srcpb;
    ctxt.fillorgx = -dstx;
    ctxt.fillorgy = -dsty;
    ctxt.palmap   = palmap;

    /* 绘制扫描线 */
    dstw = srcw;
    dsth = srch;
    while (dsth--) scanline(dstpb, dsty++, dstx, dstx + dstw - 1, (DWORD)&ctxt);
}

void bitbltstretch(BMP *dstpb, int dstx, int dsty, int dstw, int dsth,
                   BMP *srcpb, int srcx, int srcy, int srcw, int srch)
{
    // todo..
}

void setblttype(void *ctxt, int type)
{
    DRAWCONTEXT *context = (DRAWCONTEXT*)context;
    context->blttype = type;
}

void bitblt(void *ctxt,  int dstx, int dsty, int dstw, int dsth,
            BMP  *srcpb, int srcx, int srcy, int srcw, int srch)
{
    DRAWCONTEXT *context = (DRAWCONTEXT*)context;

    if (dstw != srcw || dsth != srch) {
        bitbltstretch(context->dstbmp, dstx, dsty, dstw, dsth, srcpb, srcx, srcy, srcw, srch);
        return;
    }

    switch (context->blttype)
    {
    case BLT_TYPE_COPY:
        if (context->alpha == 0) {
            bitbltcopy(context->dstbmp, dstx, dsty, srcpb, srcx, srcy, srcw, srch);
        }
        else {
            bitbltalpha(context->dstbmp, dstx, dsty, srcpb, srcx, srcy, srcw, srch, context->alpha);
        }
        break;
    case BLT_TYPE_MASK:
        if (context->alpha == 0) {
            bitbltmask(context->dstbmp, dstx, dsty, srcpb, srcx, srcy, srcw, srch, context->maskc);
        }
        else {
            bitbltmaskalpha(context->dstbmp, dstx, dsty, srcpb, srcx, srcy, srcw, srch, context->maskc, context->alpha);
        }
        break;
    case BLT_TYPE_PALMAP:
        bitbltpalmap(context->dstbmp, dstx, dsty, srcpb, srcx, srcy, srcw, srch, context->palmap);
        break;
    }
}

#else
/* 包含头文件 */
#include "win.h"
#include "bmpfile.h"
#include "bitblt.h"
int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPreInst, LPSTR lpszCmdLine, int nCmdShow)
{
    BMP mybmp = {0};

    if (*lpszCmdLine == 0) {
        lpszCmdLine = "res/me.bmp";
    }

    RGE_WIN_INIT(hInst);
    SCREEN.cdepth = 16;
    createbmp(&SCREEN);
    loadbmp(&mybmp, lpszCmdLine, NULL);

    putbmp(&SCREEN,
           (SCREEN.width  - mybmp.width ) / 2,
           (SCREEN.height - mybmp.height) / 2,
           &mybmp);

    RGE_MSG_LOOP();
    destroybmp(&mybmp);
    destroybmp(&SCREEN);
    return 0;
}
#endif



