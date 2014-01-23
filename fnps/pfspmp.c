/* 包含头文件 */
#include "fnps.h"

/* ++ 256-colors palette map functions ++ */
static void scanlinepalmapalpha(BMP *pb, int y, int x0, int x1, DWORD c)
{
    DRAWCONTEXT *ctxt  = (DRAWCONTEXT*)c;
    BYTE        *map   = ctxt->palmap;
    BYTE        *pbyte = pb->pdata + y * pb->stride + x0;
    int          i;

    if (!map)
    {
        for (i=x0; i<=x1; i++)
        {
           *pbyte = *pbyte;
            pbyte++;
        }
    }
    else
    {
        for (i=x0; i<=x1; i++)
        {
           *pbyte = map[*pbyte];
            pbyte++;
        }
    }
}

static void scanlinepalmapbmp(BMP *pb, int y, int x0, int x1, DWORD c)
{
    DRAWCONTEXT *ctxt   = (DRAWCONTEXT*)c;
    BYTE        *map    = ctxt->palmap;
    BMP         *pbdst  = pb;
    BMP         *pbsrc  = ctxt->fillbmp;
    int          sx     = (ctxt->fillorgx + x0) % pbsrc->width;
    int          sy     = (ctxt->fillorgy + y ) % pbsrc->height;
    BYTE        *pdst   = (BYTE*)(pbdst->pdata +  y * pbdst->stride) + x0;
    BYTE        *psrc   = (BYTE*)(pbsrc->pdata + sy * pbsrc->stride) + sx;
    BYTE        *pstart = (BYTE*)(pbsrc->pdata + sy * pbsrc->stride);
    BYTE        *pend   = pstart + pbsrc->width;
    int          i;

    if (!map)
    {
        for (i=x0; i<=x1; i++)
        {
           *pdst++ = *psrc++;
            if (psrc == pend) psrc = pstart;
        }
    }
    else
    {
        for (i=x0; i<=x1; i++)
        {
           *pdst++ = map[*psrc++];
            if (psrc == pend) psrc = pstart;
        }
    }
}
/* -- 256-colors palette map functions -- */

PFNSCANLINE PFN_SCANLINE_PALMAP_COLOR = scanlinepalmapalpha;
PFNSCANLINE PFN_SCANLINE_PALMAP_BMP   = scanlinepalmapbmp;
