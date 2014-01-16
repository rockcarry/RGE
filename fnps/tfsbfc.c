/* 包含头文件 */
#include "fnps.h"

/* ++ fast bmp scan line ++ */
static void scanline8bitbmpfast(BMP *pb, int y, int x0, int x1, DWORD c)
{
    DRAWCONTEXT *ctxt   = (DRAWCONTEXT*)c;
    BMP         *pbdst  = pb;
    BMP         *pbsrc  = ctxt->fillbmp;
    int          sx     = (ctxt->fillorgx + x0) % pbsrc->width;
    int          sy     = (ctxt->fillorgy + y ) % pbsrc->height;
    BYTE        *pdst   = (BYTE*)(pbdst->pdata +  y * pbdst->stride) + x0;
    BYTE        *psrc   = (BYTE*)(pbsrc->pdata + sy * pbsrc->stride) + sx;
    BYTE        *pstart = (BYTE*)(pbsrc->pdata + sy * pbsrc->stride);
    BYTE        *pend   = pstart + pbsrc->width;
    int          i;
    for (i=x0; i<=x1; i++)
    {
       *pdst++ = RGB332(pbsrc->ppal[*psrc * 4 + 2],
                        pbsrc->ppal[*psrc * 4 + 1],
                        pbsrc->ppal[*psrc * 4 + 0]);
        psrc++;
        if (psrc == pend) psrc = pstart;
    }
}

static void scanline16bitbmpfast(BMP *pb, int y, int x0, int x1, DWORD c)
{
    DRAWCONTEXT *ctxt   = (DRAWCONTEXT*)c;
    BMP         *pbdst  = pb;
    BMP         *pbsrc  = ctxt->fillbmp;
    int          sx     = (ctxt->fillorgx + x0) % pbsrc->width;
    int          sy     = (ctxt->fillorgy + y ) % pbsrc->height;
    WORD        *pdst   = (WORD*)(pbdst->pdata +  y * pbdst->stride) + x0;
    WORD        *psrc   = (WORD*)(pbsrc->pdata + sy * pbsrc->stride) + sx;
    WORD        *pstart = (WORD*)(pbsrc->pdata + sy * pbsrc->stride);
    WORD        *pend   = pstart + pbsrc->width;
    int          i;
    for (i=x0; i<=x1; i++)
    {
        *pdst++ = *psrc++;
        if (psrc == pend) psrc = pstart;
    }
}

static void scanline24bitbmpfast(BMP *pb, int y, int x0, int x1, DWORD c)
{
    DRAWCONTEXT *ctxt   = (DRAWCONTEXT*)c;
    BMP         *pbdst  = pb;
    BMP         *pbsrc  = ctxt->fillbmp;
    int          sx     = (ctxt->fillorgx + x0) % pbsrc->width;
    int          sy     = (ctxt->fillorgy + y ) % pbsrc->height;
    BYTE        *pdst   = pbdst->pdata +  y * pbdst->stride + x0 * 3;
    BYTE        *psrc   = pbsrc->pdata + sy * pbsrc->stride + sx * 3;
    BYTE        *pstart = pbsrc->pdata + sy * pbsrc->stride;
    BYTE        *pend   = pstart + pbsrc->width * 3;
    int          i;
    for (i=x0; i<=x1; i++)
    {
        *pdst++ = *psrc++;
        *pdst++ = *psrc++;
        *pdst++ = *psrc++;
        if (psrc == pend) psrc = pstart;
    }
}

static void scanline32bitbmpfast(BMP *pb, int y, int x0, int x1, DWORD c)
{
    DRAWCONTEXT *ctxt   = (DRAWCONTEXT*)c;
    BMP         *pbdst  = pb;
    BMP         *pbsrc  = ctxt->fillbmp;
    int          sx     = (ctxt->fillorgx + x0) % pbsrc->width;
    int          sy     = (ctxt->fillorgy + y ) % pbsrc->height;
    DWORD       *pdst   = (DWORD*)(pbdst->pdata +  y * pbdst->stride) + x0;
    DWORD       *psrc   = (DWORD*)(pbsrc->pdata + sy * pbsrc->stride) + sx;
    DWORD       *pstart = (DWORD*)(pbsrc->pdata + sy * pbsrc->stride);
    DWORD       *pend   = pstart + pbsrc->width;
    int          i;
    for (i=x0; i<=x1; i++)
    {
        *pdst++ = *psrc++;
        if (psrc == pend) psrc = pstart;
    }
}
/* -- fast bmp scan line -- */

PFNSCANLINE TABFN_SCANLINEBMP_FAST[] =
{
    [1] = scanline8bitbmpfast,
    [2] = scanline16bitbmpfast,
    [3] = scanline24bitbmpfast,
    [4] = scanline32bitbmpfast,
};
