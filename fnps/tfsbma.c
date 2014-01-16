/* 包含头文件 */
#include "fnps.h"

static inline void draw_alpha_8bit(BYTE *pdst, int alpha, DWORD color)
{
    DWORD fc, bc, rc;

    if (alpha < 7)
    {
        fc    = (color | (color << 9)) & 0x38E3;
        bc    = (*pdst | (*pdst << 9)) & 0x38E3;
        rc    = (bc + alpha * (fc - bc) / 8) & 0x38E3;
        *pdst = (BYTE)(rc | rc >> 9);
    }
    else *pdst = color;
}

static inline void draw_alpha_16bit(WORD *pdst, int alpha, DWORD color)
{
    DWORD fc, bc, rc;

    if (alpha < 31)
    {
        fc    = (color | (color << 16)) & 0x07E0F81F;
        bc    = (*pdst | (*pdst << 16)) & 0x07E0F81F;
        rc    = (bc + alpha * (fc - bc) / 32) & 0x07E0F81F;
        *pdst = (WORD)(rc | rc >> 16);
    }
    else *pdst = color;
}

static inline void draw_alpha_24bit(BYTE *pdst, int alpha, BYTE r, BYTE g, BYTE b)
{
    if (alpha < 255)
    {
        pdst[0] = pdst[0] + alpha * (b - pdst[0]) / 256;
        pdst[1] = pdst[1] + alpha * (g - pdst[1]) / 256;
        pdst[2] = pdst[2] + alpha * (r - pdst[2]) / 256;
    }
    else
    {
        pdst[0] = b;
        pdst[1] = g;
        pdst[2] = r;
    }
}

static inline void draw_alpha_32bit(DWORD *pdst, int alpha, DWORD color)
{
    DWORD fc = color & 0xff00ff;
    DWORD fg = color & 0x00ff00;
    DWORD bc, bg, rc, rg;

    if (alpha < 255)
    {
        bc = (*pdst & 0xff00ff);
        bg = (*pdst & 0x00ff00);
        rc = (bc + alpha * (fc - bc) / 256) & 0xff00ff;
        rg = (bg + alpha * (fg - bg) / 256) & 0x00ff00;
        *pdst = rc | rg;
    }
    else *pdst = color;
}


/* ++ alpha bmp scan line ++ */
static void scanline8bitbmpalpha(BMP *pb, int y, int x0, int x1, DWORD c)
{
    DRAWCONTEXT *ctxt   = (DRAWCONTEXT*)c;
    BMP         *pbdst  = pb;
    BMP         *pbsrc  = ctxt->fillbmp;
    BYTE        *pal    = pbsrc->ppal;
    int          sx     = (ctxt->fillorgx + x0) % pbsrc->width;
    int          sy     = (ctxt->fillorgy + y ) % pbsrc->height;
    int          alpha  = (ctxt->alpha >> 5);
    BYTE        *pdst   = (BYTE*)(pbdst->pdata +  y * pbdst->stride) + x0;
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
            color = RGB332(pal[*psrc * 4 + 2], pal[*psrc * 4 + 1], pal[*psrc * 4 + 0]);
            draw_alpha_8bit(pdst, alpha, color);
            pdst += 1;
            psrc += 1;
            if (psrc == pend) psrc = pstart;
        }
        break;
    case 16:
        for (i=x0; i<=x1; i++)
        {
            color = *(WORD*)psrc;
            color = ((color & 0x0018) >> 3) | ((color & 0x0700) >> 8) | ((color & 0xE000) >> 13);
            draw_alpha_8bit(pdst, alpha, color);
            pdst += 1;
            psrc += 2;
            if (psrc == pend) psrc = pstart;
        }
        break;
    case 24:
        for (i=x0; i<=x1; i++)
        {
            color = RGB332(psrc[2], psrc[1], psrc[0]);
            draw_alpha_8bit(pdst, alpha, color);
            pdst += 1;
            psrc += 3;
            if (psrc == pend) psrc = pstart;
        }
        break;
    case 32:
        for (i=x0; i<=x1; i++)
        {
            if (ctxt->alpha == (DWORD)-1) alpha = psrc[3] >> 5;
            color = RGB332(psrc[2], psrc[1], psrc[0]);
            draw_alpha_8bit(pdst, alpha, color);
            pdst += 1;
            psrc += 4;
            if (psrc == pend) psrc = pstart;
        }
        break;
    }
}

static void scanline16bitbmpalpha(BMP *pb, int y, int x0, int x1, DWORD c)
{
    DRAWCONTEXT *ctxt   = (DRAWCONTEXT*)c;
    BMP         *pbdst  = pb;
    BMP         *pbsrc  = ctxt->fillbmp;
    BYTE        *pal    = pbsrc->ppal;
    int          sx     = (ctxt->fillorgx + x0) % pbsrc->width;
    int          sy     = (ctxt->fillorgy + y ) % pbsrc->height;
    int          alpha  = (ctxt->alpha >> 3);
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
            color = RGB565(pal[*psrc * 4 + 2], pal[*psrc * 4 + 1], pal[*psrc * 4 + 0]);
            draw_alpha_16bit(pdst, alpha, color);
            pdst += 1;
            psrc += 1;
            if (psrc == pend) psrc = pstart;
        }
        break;
    case 16:
        for (i=x0; i<=x1; i++)
        {
            color = *(WORD*)psrc;
            draw_alpha_16bit(pdst, alpha, color);
            pdst += 1;
            psrc += 2;
            if (psrc == pend) psrc = pstart;
        }
        break;
    case 24:
        for (i=x0; i<=x1; i++)
        {
            color = RGB565(psrc[2], psrc[1], psrc[0]);
            draw_alpha_16bit(pdst, alpha, color);
            pdst += 1;
            psrc += 3;
            if (psrc == pend) psrc = pstart;
        }
        break;
    case 32:
        for (i=x0; i<=x1; i++)
        {
            if (ctxt->alpha == (DWORD)-1) alpha = psrc[3] >> 3;
            color = RGB565(psrc[2], psrc[1], psrc[0]);
            draw_alpha_16bit(pdst, alpha, color);
            pdst += 1;
            psrc += 4;
            if (psrc == pend) psrc = pstart;
        }
        break;
    }
}

static void scanline24bitbmpalpha(BMP *pb, int y, int x0, int x1, DWORD c)
{
    DRAWCONTEXT *ctxt   = (DRAWCONTEXT*)c;
    BMP         *pbdst  = pb;
    BMP         *pbsrc  = ctxt->fillbmp;
    BYTE        *pal    = pbsrc->ppal;
    int          sx     = (ctxt->fillorgx + x0) % pbsrc->width;
    int          sy     = (ctxt->fillorgy + y ) % pbsrc->height;
    int          alpha  = (ctxt->alpha >> 0);
    BYTE        *pdst   = (BYTE*)(pbdst->pdata +  y * pbdst->stride) + x0 * 3;
    BYTE        *psrc   = (BYTE*)(pbsrc->pdata + sy * pbsrc->stride) + sx * (pbsrc->cdepth / 8);
    BYTE        *pstart = (BYTE*)(pbsrc->pdata + sy * pbsrc->stride);
    BYTE        *pend   = pstart + pbsrc->width * (pbsrc->cdepth / 8);
    int          i;

    switch (pbsrc->cdepth)
    {
    case 8:
        for (i=x0; i<=x1; i++)
        {
            draw_alpha_24bit(pdst, alpha,
                             pal[*psrc * 4 + 2],
                             pal[*psrc * 4 + 1],
                             pal[*psrc * 4 + 0]);
            pdst += 3;
            psrc += 1;
            if (psrc == pend) psrc = pstart;
        }
        break;
    case 16:
        for (i=x0; i<=x1; i++)
        {
            draw_alpha_24bit(pdst, alpha,
                             (*(WORD*)psrc & 0x001f) << 3,
                             (*(WORD*)psrc & 0x07e0) >> 3,
                             (*(WORD*)psrc & 0xf800) >> 8);
            pdst += 3;
            psrc += 2;
            if (psrc == pend) psrc = pstart;
        }
        break;
    case 24:
        for (i=x0; i<=x1; i++)
        {
            draw_alpha_24bit(pdst, alpha,
                             psrc[2], psrc[1], psrc[0]);
            pdst += 3;
            psrc += 3;
            if (psrc == pend) psrc = pstart;
        }
        break;
    case 32:
        for (i=x0; i<=x1; i++)
        {
            if (ctxt->alpha == (DWORD)-1) alpha = psrc[3];
            draw_alpha_24bit(pdst, alpha,
                             psrc[2], psrc[1], psrc[0]);
            pdst += 3;
            psrc += 4;
            if (psrc == pend) psrc = pstart;
        }
        break;
    }
}

static void scanline32bitbmpalpha(BMP *pb, int y, int x0, int x1, DWORD c)
{
    DRAWCONTEXT *ctxt   = (DRAWCONTEXT*)c;
    BMP         *pbdst  = pb;
    BMP         *pbsrc  = ctxt->fillbmp;
    BYTE        *pal    = pbsrc->ppal;
    int          sx     = (ctxt->fillorgx + x0) % pbsrc->width;
    int          sy     = (ctxt->fillorgy + y ) % pbsrc->height;
    int          alpha  = (ctxt->alpha >> 0);
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
            color = RGB888(pal[*psrc * 4 + 2], pal[*psrc * 4 + 1], pal[*psrc * 4 + 0]);
            draw_alpha_32bit(pdst, alpha, color);
            pdst += 1;
            psrc += 1;
            if (psrc == pend) psrc = pstart;
        }
        break;
    case 16:
        for (i=x0; i<=x1; i++)
        {
            color = *(WORD*)psrc;
            color = ((color & 0x001f) << 3) | ((color & 0x07e0) << 5) | ((color & 0xf800) << 8);
            draw_alpha_32bit(pdst, alpha, color);
            pdst += 1;
            psrc += 2;
            if (psrc == pend) psrc = pstart;
        }
        break;
    case 24:
        for (i=x0; i<=x1; i++)
        {
            color = RGB888(psrc[2], psrc[1], psrc[0]);
            draw_alpha_32bit(pdst, alpha, color);
            pdst += 1;
            psrc += 3;
            if (psrc == pend) psrc = pstart;
        }
        break;
    case 32:
        for (i=x0; i<=x1; i++)
        {
            if (ctxt->alpha == (DWORD)-1) alpha = psrc[3];
            color = RGB888(psrc[2], psrc[1], psrc[0]);
            draw_alpha_32bit(pdst, alpha, color);
            pdst += 1;
            psrc += 4;
            if (psrc == pend) psrc = pstart;
        }
        break;
    }
}
/* -- alpha bmp scan line -- */


/* ++ mask alpha bmp scan line ++ */
static void scanline8bitbmpmaskalpha(BMP *pb, int y, int x0, int x1, DWORD c)
{
    DRAWCONTEXT *ctxt   = (DRAWCONTEXT*)c;
    BMP         *pbdst  = pb;
    BMP         *pbsrc  = ctxt->fillbmp;
    BYTE        *pal    = pbsrc->ppal;
    int          sx     = (ctxt->fillorgx + x0) % pbsrc->width;
    int          sy     = (ctxt->fillorgy + y ) % pbsrc->height;
    int          alpha  = (ctxt->alpha >> 5);
    BYTE        *pdst   = (BYTE*)(pbdst->pdata +  y * pbdst->stride) + x0;
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
            if (ctxt->maskc != *(BYTE*)psrc)
            {
                color = RGB332(pal[*psrc * 4 + 2], pal[*psrc * 4 + 1], pal[*psrc * 4 + 0]);
                draw_alpha_8bit(pdst, alpha, color);
            }
            pdst += 1;
            psrc += 1;
            if (psrc == pend) psrc = pstart;
        }
        break;
    case 16:
        for (i=x0; i<=x1; i++)
        {
            if (ctxt->maskc != *(WORD*)psrc)
            {
                color = *(WORD*)psrc;
                color = ((color & 0x0018) >> 3) | ((color & 0x0700) >> 8) | ((color & 0xE000) >> 13);
                draw_alpha_8bit(pdst, alpha, color);
            }
            pdst += 1;
            psrc += 2;
            if (psrc == pend) psrc = pstart;
        }
        break;
    case 24:
        for (i=x0; i<=x1; i++)
        {
            if (ctxt->maskc != (*(DWORD*)psrc & 0xffffff))
            {
                color = RGB332(psrc[2], psrc[1], psrc[0]);
                draw_alpha_8bit(pdst, alpha, color);
            }
            pdst += 1;
            psrc += 3;
            if (psrc == pend) psrc = pstart;
        }
        break;
    case 32:
        for (i=x0; i<=x1; i++)
        {
            if (ctxt->maskc != *(DWORD*)psrc)
            {
                if (ctxt->alpha == (DWORD)-1) alpha = psrc[3] >> 5;
                color = RGB332(psrc[2], psrc[1], psrc[0]);
                draw_alpha_8bit(pdst, alpha, color);
            }
            pdst += 1;
            psrc += 4;
            if (psrc == pend) psrc = pstart;
        }
        break;
    }
}

static void scanline16bitbmpmaskalpha(BMP *pb, int y, int x0, int x1, DWORD c)
{
    DRAWCONTEXT *ctxt   = (DRAWCONTEXT*)c;
    BMP         *pbdst  = pb;
    BMP         *pbsrc  = ctxt->fillbmp;
    BYTE        *pal    = pbsrc->ppal;
    int          sx     = (ctxt->fillorgx + x0) % pbsrc->width;
    int          sy     = (ctxt->fillorgy + y ) % pbsrc->height;
    int          alpha  = (ctxt->alpha >> 3);
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
            if (ctxt->maskc != *(BYTE*)psrc)
            {
                color = RGB565(pal[*psrc * 4 + 2], pal[*psrc * 4 + 1], pal[*psrc * 4 + 0]);
                draw_alpha_16bit(pdst, alpha, color);
            }
            pdst += 1;
            psrc += 1;
            if (psrc == pend) psrc = pstart;
        }
        break;
    case 16:
        for (i=x0; i<=x1; i++)
        {
            if (ctxt->maskc != *(WORD*)psrc)
            {
                color = *(WORD*)psrc;
                draw_alpha_16bit(pdst, alpha, color);
            }
            pdst += 1;
            psrc += 2;
            if (psrc == pend) psrc = pstart;
        }
        break;
    case 24:
        for (i=x0; i<=x1; i++)
        {
            if (ctxt->maskc != (*(DWORD*)psrc & 0xffffff))
            {
                color = RGB565(psrc[2], psrc[1], psrc[0]);
                draw_alpha_16bit(pdst, alpha, color);
            }
            pdst += 1;
            psrc += 3;
            if (psrc == pend) psrc = pstart;
        }
        break;
    case 32:
        for (i=x0; i<=x1; i++)
        {
            if (ctxt->maskc != *(DWORD*)psrc)
            {
                if (ctxt->alpha == (DWORD)-1) alpha = psrc[3] >> 3;
                color = RGB565(psrc[2], psrc[1], psrc[0]);
                draw_alpha_16bit(pdst, alpha, color);
            }
            pdst += 1;
            psrc += 4;
            if (psrc == pend) psrc = pstart;
        }
        break;
    }
}

static void scanline24bitbmpmaskalpha(BMP *pb, int y, int x0, int x1, DWORD c)
{
    DRAWCONTEXT *ctxt   = (DRAWCONTEXT*)c;
    BMP         *pbdst  = pb;
    BMP         *pbsrc  = ctxt->fillbmp;
    BYTE        *pal    = pbsrc->ppal;
    int          sx     = (ctxt->fillorgx + x0) % pbsrc->width;
    int          sy     = (ctxt->fillorgy + y ) % pbsrc->height;
    int          alpha  = (ctxt->alpha >> 0);
    BYTE        *pdst   = (BYTE*)(pbdst->pdata +  y * pbdst->stride) + x0 * 3;
    BYTE        *psrc   = (BYTE*)(pbsrc->pdata + sy * pbsrc->stride) + sx * (pbsrc->cdepth / 8);
    BYTE        *pstart = (BYTE*)(pbsrc->pdata + sy * pbsrc->stride);
    BYTE        *pend   = pstart + pbsrc->width * (pbsrc->cdepth / 8);
    int          i;

    switch (pbsrc->cdepth)
    {
    case 8:
        for (i=x0; i<=x1; i++)
        {
            if (ctxt->maskc != *(BYTE*)psrc)
            {
                draw_alpha_24bit(pdst, alpha,
                                 pal[*psrc * 4 + 2],
                                 pal[*psrc * 4 + 1],
                                 pal[*psrc * 4 + 0]);
            }
            pdst += 3;
            psrc += 1;
            if (psrc == pend) psrc = pstart;
        }
        break;
    case 16:
        for (i=x0; i<=x1; i++)
        {
            if (ctxt->maskc != *(WORD*)psrc)
            {
                draw_alpha_24bit(pdst, alpha,
                                 (*(WORD*)psrc & 0x001f) << 3,
                                 (*(WORD*)psrc & 0x07e0) >> 3,
                                 (*(WORD*)psrc & 0xf800) >> 8);
            }
            pdst += 3;
            psrc += 2;
            if (psrc == pend) psrc = pstart;
        }
        break;
    case 24:
        for (i=x0; i<=x1; i++)
        {
            if (ctxt->maskc != (*(DWORD*)psrc & 0xffffff))
            {
                draw_alpha_24bit(pdst, alpha,
                                 psrc[2], psrc[1], psrc[0]);
            }
            pdst += 3;
            psrc += 3;
            if (psrc == pend) psrc = pstart;
        }
        break;
    case 32:
        for (i=x0; i<=x1; i++)
        {
            if (ctxt->maskc != *(DWORD*)psrc)
            {
                if (ctxt->alpha == (DWORD)-1) alpha = psrc[3];
                draw_alpha_24bit(pdst, alpha,
                                 psrc[2], psrc[1], psrc[0]);
            }
            pdst += 3;
            psrc += 4;
            if (psrc == pend) psrc = pstart;
        }
        break;
    }
}

static void scanline32bitbmpmaskalpha(BMP *pb, int y, int x0, int x1, DWORD c)
{
    DRAWCONTEXT *ctxt   = (DRAWCONTEXT*)c;
    BMP         *pbdst  = pb;
    BMP         *pbsrc  = ctxt->fillbmp;
    BYTE        *pal    = pbsrc->ppal;
    int          sx     = (ctxt->fillorgx + x0) % pbsrc->width;
    int          sy     = (ctxt->fillorgy + y ) % pbsrc->height;
    int          alpha  = (ctxt->alpha >> 0);
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
            if (ctxt->maskc != *(BYTE*)psrc)
            {
                color = RGB888(pal[*psrc * 4 + 2], pal[*psrc * 4 + 1], pal[*psrc * 4 + 0]);
                draw_alpha_32bit(pdst, alpha, color);
            }
            pdst += 1;
            psrc += 1;
            if (psrc == pend) psrc = pstart;
        }
        break;
    case 16:
        for (i=x0; i<=x1; i++)
        {
            if (ctxt->maskc != *(WORD*)psrc)
            {
                color = *(WORD*)psrc;
                color = ((color & 0x001f) << 3) | ((color & 0x07e0) << 5) | ((color & 0xf800) << 8);
                draw_alpha_32bit(pdst, alpha, color);
            }
            pdst += 1;
            psrc += 2;
            if (psrc == pend) psrc = pstart;
        }
        break;
    case 24:
        for (i=x0; i<=x1; i++)
        {
            if (ctxt->maskc != (*(DWORD*)psrc & 0xffffff))
            {
                color = RGB888(psrc[2], psrc[1], psrc[0]);
                draw_alpha_32bit(pdst, alpha, color);
            }
            pdst += 1;
            psrc += 3;
            if (psrc == pend) psrc = pstart;
        }
        break;
    case 32:
        for (i=x0; i<=x1; i++)
        {
            if (ctxt->maskc != *(DWORD*)psrc)
            {
                if (ctxt->alpha == (DWORD)-1) alpha = psrc[3];
                color = RGB888(psrc[2], psrc[1], psrc[0]);
                draw_alpha_32bit(pdst, alpha, color);
            }
            pdst += 1;
            psrc += 4;
            if (psrc == pend) psrc = pstart;
        }
        break;
    }
}
/* -- mask alpha bmp scan line -- */

PFNSCANLINE TABFN_SCANLINEBMP_ALPHA[] =
{
    [1] = scanline8bitbmpalpha,
    [2] = scanline16bitbmpalpha,
    [3] = scanline24bitbmpalpha,
    [4] = scanline32bitbmpalpha,
};

PFNSCANLINE TABFN_SCANLINEBMP_MASKALPHA[] =
{
    [1] = scanline8bitbmpmaskalpha,
    [2] = scanline16bitbmpmaskalpha,
    [3] = scanline24bitbmpmaskalpha,
    [4] = scanline32bitbmpmaskalpha,
};
