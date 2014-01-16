/* 包含头文件 */
#include "fnps.h"

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
    [1] = pixel8bitsolid,
    [2] = pixel16bitsolid,
    [3] = pixel24bitsolid,
    [4] = pixel32bitsolid,
};
