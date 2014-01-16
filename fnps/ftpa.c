/* 包含头文件 */
#include "fnps.h"

/* ++ alpha pixel functions ++ */
static void pixel8bitalpha(BMP *pb, int x, int y, DWORD c)
{
    BYTE *pbyte = pb->pdata + y * pb->stride + x;
    DWORD fc    = ( c     | ( c     << 9)) & 0x38E3;
    DWORD bc    = (*pbyte | (*pbyte << 9)) & 0x38E3;
    DWORD alpha = (c >> 24 >> 5);
    DWORD rc    = (bc + alpha * (fc - bc) / 8) & 0x38E3;
    *pbyte = (BYTE)(rc | rc >> 9);
}

static void pixel16bitalpha(BMP *pb, int x, int y, DWORD c)
{
    WORD  *pword = (WORD*)(pb->pdata + y * pb->stride) + x;
    DWORD  fc    = ( c     | ( c     << 16)) & 0x07E0F81F;
    DWORD  bc    = (*pword | (*pword << 16)) & 0x07E0F81F;
    DWORD  alpha = (c >> 24 >> 3);
    DWORD  rc    = (bc + alpha * (fc - bc) / 32) & 0x07E0F81F;
    *pword = (WORD)(rc | rc >> 16);
}

static void pixel24bitalpha(BMP *pb, int x, int y, DWORD c)
{
    BYTE *pbyte = pb->pdata + y * pb->stride + x * 3;
    DWORD alpha = (c >> 24) & 0xff;
    DWORD r     = (c >> 16) & 0xff;
    DWORD g     = (c >> 8 ) & 0xff;
    DWORD b     = (c >> 0 ) & 0xff;
    pbyte[0] = pbyte[0] + alpha * (b - pbyte[0]) / 256;
    pbyte[1] = pbyte[1] + alpha * (g - pbyte[1]) / 256;
    pbyte[2] = pbyte[2] + alpha * (r - pbyte[2]) / 256;
}

static void pixel32bitalpha(BMP *pb, int x, int y, DWORD c)
{
    DWORD *pdword = (DWORD*)(pb->pdata + y * pb->stride) + x;
    DWORD  fc     =  c      & 0xff00ff;
    DWORD  fg     =  c      & 0x00ff00;
    DWORD  bc     = *pdword & 0xff00ff;
    DWORD  bg     = *pdword & 0x00ff00;
    DWORD  alpha  = c >> 24;
    DWORD  rc     = (bc + alpha * (fc - bc) / 256) & 0xff00ff;
    DWORD  rg     = (bg + alpha * (fg - bg) / 256) & 0x00ff00;
    *pdword = (rc | rg);
}
/* -- alpha pixel functions -- */

PFNPIXEL TABFN_PIXEL_ALPHA[] =
{
    [1] = pixel8bitalpha,
    [2] = pixel16bitalpha,
    [3] = pixel24bitalpha,
    [4] = pixel32bitalpha,
};
