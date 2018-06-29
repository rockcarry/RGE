/* 包含头文件 */
#include "pixel.h"

/* 内部函数实现 */
/* ++ solid pixel functions ++ */
static void pixel8bitsolid(BMP *pb, int x, int y, DWORD c)
{
    *((BYTE*)(pb->pdata + pb->stride * y) + x) = (BYTE)c;
}

static void pixel16bitsolid(BMP *pb, int x, int y, DWORD c)
{
    *((WORD*)(pb->pdata + pb->stride * y) + x) = (WORD)c;
}

static void pixel24bitsolid(BMP *pb, int x, int y, DWORD c)
{
    BYTE *pdata = pb->pdata + pb->stride * y + x * 3;
    *pdata++ = (BYTE)(c >> 0 );
    *pdata++ = (BYTE)(c >> 8 );
    *pdata++ = (BYTE)(c >> 16);
}

static void pixel32bitsolid(BMP *pb, int x, int y, DWORD c)
{
    *((DWORD*)(pb->pdata + pb->stride * y) + x) = (DWORD)c;
}
/* -- solid pixel functions -- */

PFNPIXEL TABFN_PIXEL_SOLID[] =
{
    pixel8bitsolid,
    pixel16bitsolid,
    pixel24bitsolid,
    pixel32bitsolid,
};

/* ++ alpha pixel functions ++ */
static void pixel8bitalpha(BMP *pb, int x, int y, DWORD c)
{
    BYTE *pbyte = pb->pdata + y * pb->stride + x;
    DWORD fc    = ( c     | ( c     << 9)) & 0x38E3;
    DWORD bc    = (*pbyte | (*pbyte << 9)) & 0x38E3;
    DWORD alpha = (c >> 24 >> 5);
    DWORD rc    = (bc + alpha * (fc - bc) / 8) & 0x38E3;
    *pbyte = (BYTE)(rc | (rc >> 9));
}

static void pixel16bitalpha(BMP *pb, int x, int y, DWORD c)
{
    WORD  *pword = (WORD*)(pb->pdata + y * pb->stride) + x;
    DWORD  fc    = ( c     | ( c     << 16)) & 0x07E0F81FL;
    DWORD  bc    = (*pword | (*pword << 16)) & 0x07E0F81FL;
    DWORD  alpha = (c >> 24 >> 3);
    DWORD  rc    = (bc + alpha * (fc - bc) / 32) & 0x07E0F81FL;
    *pword = (WORD)(rc | (rc >> 16));
}

static void pixel24bitalpha(BMP *pb, int x, int y, DWORD c)
{
    BYTE *pbyte = pb->pdata + y * pb->stride + x * 3;
    DWORD alpha = (c >> 24) & 0xff;
    DWORD r     = (c >> 16) & 0xff;
    DWORD g     = (c >> 8 ) & 0xff;
    DWORD b     = (c >> 0 ) & 0xff;
    pbyte[0] = (BYTE)(pbyte[0] + alpha * (b - pbyte[0]) / 256);
    pbyte[1] = (BYTE)(pbyte[1] + alpha * (g - pbyte[1]) / 256);
    pbyte[2] = (BYTE)(pbyte[2] + alpha * (r - pbyte[2]) / 256);
}

static void pixel32bitalpha(BMP *pb, int x, int y, DWORD c)
{
    DWORD *pdword = (DWORD*)(pb->pdata + y * pb->stride) + x;
    DWORD  fc     =  c      & 0xff00ffL;
    DWORD  fg     =  c      & 0x00ff00L;
    DWORD  bc     = *pdword & 0xff00ffL;
    DWORD  bg     = *pdword & 0x00ff00L;
    DWORD  alpha  = c >> 24;
    DWORD  rc     = (bc + alpha * (fc - bc) / 256) & 0xff00ffL;
    DWORD  rg     = (bg + alpha * (fg - bg) / 256) & 0x00ff00L;
    *pdword = (rc | rg);
}
/* -- alpha pixel functions -- */

PFNPIXEL TABFN_PIXEL_ALPHA[] =
{
    pixel8bitalpha,
    pixel16bitalpha,
    pixel24bitalpha,
    pixel32bitalpha,
};

