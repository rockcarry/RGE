/* 包含头文件 */
#include "fnps.h"

/* ++ solid scan line ++ */
static void scanline8bitsolid(BMP *pb, int y, int x0, int x1, DWORD c)
{
    BYTE *pbyte = pb->pdata + y * pb->stride + x0;
    memset(pbyte, c, x1 - x0 + 1);
}

static void scanline16bitsolid(BMP *pb, int y, int x0, int x1, DWORD c)
{
    WORD *pword = (WORD*)(pb->pdata + y * pb->stride) + x0;
    int  i;
    for (i=x0; i<=x1; i++) *pword++ = c;
}

static void scanline24bitsolid(BMP *pb, int y, int x0, int x1, DWORD c)
{
    BYTE *pbyte = pb->pdata + y * pb->stride + x0 * 3;
    int  i;
    for (i=x0; i<=x1; i++)
    {
        *pbyte++ = (c >> 0 ) & 0xff;
        *pbyte++ = (c >> 8 ) & 0xff;
        *pbyte++ = (c >> 16) & 0xff;
    }
}

static void scanline32bitsolid(BMP *pb, int y, int x0, int x1, DWORD c)
{
    DWORD *pdword = (DWORD*)(pb->pdata + y * pb->stride) + x0;
    int   i;
    for (i=x0; i<=x1; i++) *pdword++ = c;
}
/* -- solid scan line -- */

PFNSCANLINE TABFN_SCANLINE_SOLID[] =
{
    [1] = scanline8bitsolid,
    [2] = scanline16bitsolid,
    [3] = scanline24bitsolid,
    [4] = scanline32bitsolid,
};
