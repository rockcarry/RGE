/* 包含头文件 */
#include "fnps.h"

/* ++ alpha scan line ++ */
static void scanline8bitalpha(BMP *pb, int y, int x0, int x1, DWORD c)
{
    BYTE *pbyte = pb->pdata + y * pb->stride + x0;
    DWORD fc    = (c | (c << 9)) & 0x38E3;
    DWORD alpha = (c >> 24 >> 5);
    DWORD bc, rc;
    int   i;

    for (i=x0; i<=x1; i++)
    {
        bc = (*pbyte | (*pbyte << 9)) & 0x38E3;
        rc = (bc + alpha * (fc - bc) / 8) & 0x38E3;
        *pbyte++ = (BYTE)(rc | rc >> 9);
    }
}

static void scanline16bitalpha(BMP *pb, int y, int x0, int x1, DWORD c)
{
    WORD *pword = (WORD*)(pb->pdata + y * pb->stride) + x0;
    DWORD fc    = (c | (c << 16)) & 0x07E0F81F;
    DWORD alpha = (c >> 24 >> 3);
    DWORD bc, rc;
    int   i;

    for (i=x0; i<=x1; i++)
    {
        bc = (*pword | (*pword << 16)) & 0x07E0F81F;
        rc = (bc + alpha * (fc - bc) / 32) & 0x07E0F81F;
        *pword++ = (WORD)(rc | rc >> 16);
    }
}

static void scanline24bitalpha(BMP *pb, int y, int x0, int x1, DWORD c)
{
    BYTE *pbyte = pb->pdata + y * pb->stride + x0 * 3;
    DWORD fc    = c;
    DWORD alpha = (fc >> 24) & 0xff;
    DWORD r     = (fc >> 16) & 0xff;
    DWORD g     = (fc >> 8 ) & 0xff;
    DWORD b     = (fc >> 0 ) & 0xff;
    int   i;

    for (i=x0; i<=x1; i++)
    {
        pbyte[0] = pbyte[0] + alpha * (b - pbyte[0]) / 256;
        pbyte[1] = pbyte[1] + alpha * (g - pbyte[1]) / 256;
        pbyte[2] = pbyte[2] + alpha * (r - pbyte[2]) / 256;
        pbyte   += 3;
    }
}

static void scanline32bitalpha(BMP *pb, int y, int x0, int x1, DWORD c)
{
    DWORD *pdword = (DWORD*)(pb->pdata + y * pb->stride) + x0;
    DWORD  fc     = c & 0xff00ff;
    DWORD  fg     = c & 0x00ff00;
    DWORD  alpha  = c >> 24;
    DWORD  bc, bg, rc, rg;
    int    i;

    for (i=x0; i<x1; i++)
    {
        bc = (*pdword & 0xff00ff);
        bg = (*pdword & 0x00ff00);
        rc = (bc + alpha * (fc - bc) / 256) & 0xff00ff;
        rg = (bg + alpha * (fg - bg) / 256) & 0x00ff00;
        *pdword++ = rc | rg;
    }
}
/* -- alpha scan line -- */

PFNSCANLINE TABFN_SCANLINE_ALPHA[] =
{
    [1] = scanline8bitalpha,
    [2] = scanline16bitalpha,
    [3] = scanline24bitalpha,
    [4] = scanline32bitalpha,
};
