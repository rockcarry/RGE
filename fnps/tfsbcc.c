/* 包含头文件 */
#include "fnps.h"

/* ++ convert bmp scan line ++ */
static void scanline8bitbmpconvert(BMP *pb, int y, int x0, int x1, DWORD c)
{
    DRAWCONTEXT *ctxt   = (DRAWCONTEXT*)c;
    BMP         *pbdst  = pb;
    BMP         *pbsrc  = ctxt->fillbmp;
    int          sx     = (ctxt->fillorgx + x0) % pbsrc->width;
    int          sy     = (ctxt->fillorgy + y ) % pbsrc->height;
    BYTE        *pdst   = (BYTE*)(pbdst->pdata +  y * pbdst->stride) + x0;
    BYTE        *psrc   = (BYTE*)(pbsrc->pdata + sy * pbsrc->stride) + sx * (pbsrc->cdepth / 8);
    BYTE        *pstart = (BYTE*)(pbsrc->pdata + sy * pbsrc->stride);
    BYTE        *pend   = pstart + pbsrc->width * (pbsrc->cdepth / 8);
    DWORD        color;
    int          i;

    switch (pbsrc->cdepth)
    {
    case 16:
        for (i=x0; i<=x1; i++)
        {
            color  = *(WORD*)psrc;
            color  = ((color & 0x0018) >> 3) | ((color & 0x0700) >> 8) | ((color & 0xE000) >> 13);
           *pdst++ = color;
            psrc  += 2;
            if (psrc == pend) psrc = pstart;
        }
        break;
    case 24:
        for (i=x0; i<=x1; i++)
        {
            color  = RGB332(psrc[2], psrc[1], psrc[0]);
           *pdst++ = color;
            psrc  += 3;
            if (psrc == pend) psrc = pstart;
        }
        break;
    case 32:
        for (i=x0; i<=x1; i++)
        {
            color  = RGB332(psrc[2], psrc[1], psrc[0]);
           *pdst++ = color;
            psrc  += 4;
            if (psrc == pend) psrc = pstart;
        }
        break;
    }
}

static void scanline16bitbmpconvert(BMP *pb, int y, int x0, int x1, DWORD c)
{
    DRAWCONTEXT *ctxt   = (DRAWCONTEXT*)c;
    BMP         *pbdst  = pb;
    BMP         *pbsrc  = ctxt->fillbmp;
    BYTE        *pal    = pbsrc->ppal;
    int          sx     = (ctxt->fillorgx + x0) % pbsrc->width;
    int          sy     = (ctxt->fillorgy + y ) % pbsrc->height;
    WORD        *pdst   = (WORD*)(pbdst->pdata +  y * pbdst->stride) + x0;
    BYTE        *psrc   = (BYTE*)(pbsrc->pdata + sy * pbsrc->stride) + sx * (pbsrc->cdepth / 8);
    BYTE        *pstart = (BYTE*)(pbsrc->pdata + sy * pbsrc->stride);
    BYTE        *pend   = pstart + pbsrc->width * (pbsrc->cdepth / 8);
    DWORD        color;
    int          i;

    switch (pbsrc->cdepth)
    {
    case 8:
        for (i=x0; i<=x1; i++)
        {
            color  = *psrc;
            color  = RGB565(pal[color * 4 + 2], pal[color * 4 + 1], pal[color * 4 + 0]);
           *pdst++ = color;
            psrc  += 1;
            if (psrc == pend) psrc = pstart;
        }
        break;
    case 24:
        for (i=x0; i<=x1; i++)
        {
            color  = RGB565(psrc[2], psrc[1], psrc[0]);
           *pdst++ = color;
            psrc  += 3;
            if (psrc == pend) psrc = pstart;
        }
        break;
    case 32:
        for (i=x0; i<=x1; i++)
        {
            color  = RGB565(psrc[2], psrc[1], psrc[0]);
           *pdst++ = color;
            psrc  += 4;
            if (psrc == pend) psrc = pstart;
        }
        break;
    }
}

static void scanline24bitbmpconvert(BMP *pb, int y, int x0, int x1, DWORD c)
{
    DRAWCONTEXT *ctxt   = (DRAWCONTEXT*)c;
    BMP         *pbdst  = pb;
    BMP         *pbsrc  = ctxt->fillbmp;
    BYTE        *pal    = pbsrc->ppal;
    int          sx     = (ctxt->fillorgx + x0) % pbsrc->width;
    int          sy     = (ctxt->fillorgy + y ) % pbsrc->height;
    BYTE        *pdst   = (BYTE*)(pbdst->pdata +  y * pbdst->stride) + x0 * 3;
    BYTE        *psrc   = (BYTE*)(pbsrc->pdata + sy * pbsrc->stride) + sx * (pbsrc->cdepth / 8);
    BYTE        *pstart = (BYTE*)(pbsrc->pdata + sy * pbsrc->stride);
    BYTE        *pend   = pstart + pbsrc->width * (pbsrc->cdepth / 8);
    DWORD        color;
    int          i;

    switch (pbsrc->cdepth)
    {
    case 8:
        for (i=x0; i<=x1; i++)
        {
            color  = *psrc;
           *pdst++ = pal[color * 4 + 0];
           *pdst++ = pal[color * 4 + 1];
           *pdst++ = pal[color * 4 + 2];
            psrc  += 1;
            if (psrc == pend) psrc = pstart;
        }
        break;
    case 16:
        for (i=x0; i<=x1; i++)
        {
            color  = *(WORD*)psrc;
           *pdst++ = (color & 0x001f) << 3;
           *pdst++ = (color & 0x07e0) >> 3;
           *pdst++ = (color & 0xf800) >> 8;
            psrc  += 2;
            if (psrc == pend) psrc = pstart;
        }
        break;
    case 32:
        for (i=x0; i<=x1; i++)
        {
           *pdst++ = psrc[0];
           *pdst++ = psrc[1];
           *pdst++ = psrc[2];
            psrc  += 4;
            if (psrc == pend) psrc = pstart;
        }
        break;
    }
}

static void scanline32bitbmpconvert(BMP *pb, int y, int x0, int x1, DWORD c)
{
    DRAWCONTEXT *ctxt   = (DRAWCONTEXT*)c;
    BMP         *pbdst  = pb;
    BMP         *pbsrc  = ctxt->fillbmp;
    BYTE        *pal    = pbsrc->ppal;
    int          sx     = (ctxt->fillorgx + x0) % pbsrc->width;
    int          sy     = (ctxt->fillorgy + y ) % pbsrc->height;
    DWORD       *pdst   = (DWORD*)(pbdst->pdata +  y * pbdst->stride) + x0;
    BYTE        *psrc   = (BYTE *)(pbsrc->pdata + sy * pbsrc->stride) + sx * (pbsrc->cdepth / 8);
    BYTE        *pstart = (BYTE *)(pbsrc->pdata + sy * pbsrc->stride);
    BYTE        *pend   = pstart + pbsrc->width * (pbsrc->cdepth / 8);
    DWORD        color;
    int          i;

    switch (pbsrc->cdepth)
    {
    case 8:
        for (i=x0; i<=x1; i++)
        {
            color  = *psrc;
            color  = RGB888(pal[color * 4 + 2], pal[color * 4 + 1], pal[color * 4 + 0]);
           *pdst++ = color;
            psrc  += 1;
            if (psrc == pend) psrc = pstart;
        }
        break;
    case 16:
        for (i=x0; i<=x1; i++)
        {
            color  = *(WORD*)psrc;
           *pdst++ = ((color & 0x001f) << 3) | ((color & 0x07e0) << 5) | ((color & 0xf800) << 8);
            psrc  += 2;
            if (psrc == pend) psrc = pstart;
        }
        break;
    case 24:
        for (i=x0; i<=x1; i++)
        {
            color  = RGB888(psrc[2], psrc[1], psrc[0]);
           *pdst++ = color;
            psrc  += 3;
            if (psrc == pend) psrc = pstart;
        }
        break;
    }
}
/* -- convert bmp scan line -- */

PFNSCANLINE TABFN_SCANLINEBMP_CONVERT[] =
{
    [1] = scanline8bitbmpconvert,
    [2] = scanline16bitbmpconvert,
    [3] = scanline24bitbmpconvert,
    [4] = scanline32bitbmpconvert,
};
