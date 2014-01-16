#ifndef _TEST_

/* 包含头文件 */
#include "bmpfile.h"

/* 内部类型定义 */
/* BMP 文件结构类型定义 */
#pragma pack(1)
typedef struct
{
    WORD   filetype;
    DWORD  filesize;
    DWORD  reserved;
    DWORD  dataoffset;

    DWORD  infosize;
    LONG   imagewidth;
    LONG   imageheight;
    WORD   planes;
    WORD   bitsperpixel;
    DWORD  compression;
    DWORD  imagesize;
    LONG   xpixelpermeter;
    LONG   ypixelpermeter;
    DWORD  colorused;
    DWORD  colorimportant;

    BYTE  *ppal;
    BYTE  *pdata;
} BMPFILE;
#pragma pack()

/* 函数实现 */
/* 装载 BMP 文件到 BMP 对象 */
BOOL loadbmp(BMP *pb, char *file, FIODRV *fdrv)
{
    BMPFILE bmpfile;
    void   *fp;
    BYTE   *pdest;
    int     bstride;
    int     fstride;
    int     i;

    if (!pb  ) return FALSE;
    if (!fdrv) fdrv = &DEF_FIO_DRV;

    /* 打开文件 */
    fp = fdrv->open(file, "rb");
    if (!fp) return FALSE;

    /* 读取文件头 */
    fdrv->read(&bmpfile, sizeof(bmpfile), 1, fp);
    if (bmpfile.filetype != (('B' << 0) | ('M' << 8))) return FALSE;
    pb->cdepth = bmpfile.bitsperpixel;
    pb->width  = (int)bmpfile.imagewidth;
    pb->height = (int)bmpfile.imageheight;

    /* 目前只支持 256 色、16/24/32bit 色的 BMP 图像 */
    if (  pb->cdepth != 8
       && pb->cdepth != 16
       && pb->cdepth != 24
       && pb->cdepth != 32)
    {
        return FALSE;
    }

    /* 创建 BMP 对象 */
    if (!createbmp(pb)) return FALSE;

    /* 读取调色板数据 */
    if (pb->cdepth == 8)
    {
        fdrv->seek(fp, bmpfile.dataoffset - 256 * 4, SEEK_SET);
        fdrv->read(pb->ppal, 4, 256, fp);
        setbmppal(pb, 0, 256, pb->ppal);
    }

    bstride = pb->stride;
    fstride = (pb->width * pb->cdepth / 8 + 3) / 4 * 4;

    /* 初始化指针 */
    pdest = pb->pdata + bstride * (pb->height - 1);

    /* 读取图像数据 */
    fdrv->seek(fp, bmpfile.dataoffset, SEEK_SET);
    for (i=0; i<pb->height; i++)
    {
        fdrv->read(pdest, fstride, 1, fp);
        pdest -= bstride;
    }

    if (fp) fdrv->close(fp); /* 关闭文件 */
    return TRUE;
}

/* 保存 BMP 对象到 BMP 文件 */
BOOL savebmp(BMP *pb, char *file, FIODRV *fdrv)
{
    BMPFILE bmpfile = {0};
    DWORD   mask[3] = { 0xF800, 0x07E0, 0x001F };
    void   *fp;
    BYTE   *psrc;
    int     bstride;
    int     fstride;
    int     i;

    if (!pb  ) return FALSE;
    if (!fdrv) fdrv = &DEF_FIO_DRV;

    bstride = pb->stride;
    fstride = (pb->width * pb->cdepth / 8 + 3) / 4 * 4;

    /* 填充 BMPFILE 结构 */
    bmpfile.filetype     = ('B' << 0) | ('M' << 8);
    bmpfile.dataoffset   = sizeof(bmpfile) - sizeof(BYTE*) * 2;
    bmpfile.imagesize    = fstride * pb->height;
    bmpfile.filesize     = bmpfile.dataoffset + bmpfile.imagesize;
    bmpfile.infosize     = 40;
    bmpfile.imagewidth   = pb->width;
    bmpfile.imageheight  = pb->height;
    bmpfile.planes       = 1;
    bmpfile.bitsperpixel = pb->cdepth;

    if (bmpfile.bitsperpixel == 8)
    {
        bmpfile.dataoffset += 256 * 4;
        bmpfile.filesize   += 256 * 4;
    }

    if (bmpfile.bitsperpixel == 16)
    {
        bmpfile.dataoffset += sizeof(DWORD) * 3;
        bmpfile.filesize   += sizeof(DWORD) * 3;
        bmpfile.compression = 3; // BI_BITFIELDS
    }

    /* 打开文件 */
    fp = fdrv->open(file, "wb");
    if (!fp) return FALSE;

    /* 写入文件头 */
    fdrv->write(&bmpfile, sizeof(bmpfile) - sizeof(BYTE*) * 2, 1, fp);

    /* 写入调色板数据 */
    if (bmpfile.bitsperpixel == 8)
    {
        getbmppal(pb, 0, 256, pb->ppal);
        fdrv->write(pb->ppal, 4, 256, fp);
    }

    /* for 16bit RGB565 bmp file*/
    if (bmpfile.bitsperpixel == 16) fdrv->write(mask, sizeof(mask), 1, fp);

    /* 初始化指针 */
    psrc = pb->pdata + bstride * (pb->height - 1);

    /* 写入图像数据 */
    for (i=0; i<pb->height; i++)
    {
        fdrv->write(psrc, fstride, 1, fp);
        psrc -= bstride;
    }

    /* 关闭文件 */
    if (fp) fdrv->close(fp);
    return TRUE;
}

#else
/* 包含头文件 */
#include "bmp.h"
#include "bmpfile.h"
int main(void)
{
    int i, j;
    BMP mybmp332  = { .cdepth = 8  };
    BMP mybmp565  = { .cdepth = 16 };
    BMP mybmp888  = { .cdepth = 24 };
    BMP mybmp8888 = { .cdepth = 32 };

    createbmp(&mybmp332 );
    createbmp(&mybmp565 );
    createbmp(&mybmp888 );
    createbmp(&mybmp8888);

    lockbmp(&mybmp332 );
    lockbmp(&mybmp565 );
    lockbmp(&mybmp888 );
    lockbmp(&mybmp8888);

    #define RGB332(r, g, b)  ( (((r) & 0xE0) <<  0) | (((g) & 0xE0) >> 3) | (((b) & 0xC0) >> 6) )
    #define RGB555(r, g, b)  ( (((r) & 0xF8) <<  7) | (((g) & 0xF8) << 2) | (((b) & 0xF8) >> 3) )
    #define RGB565(r, g, b)  ( (((r) & 0xF8) <<  8) | (((g) & 0xFC) << 3) | (((b) & 0xF8) >> 3) )
    #define RGB888(r, g, b)  ( ((DWORD)(0) << 24) | ((r) << 16) | ((g) << 8) | ((b) << 0) )

    for (i=0; i<mybmp332.height; i++)
    {
        for (j=0; j<mybmp332.width; j++)
        {
            *((BYTE *)mybmp332.pdata  + i * mybmp332.width  + j) = RGB332(i, j, i);
            *((WORD *)mybmp565.pdata  + i * mybmp565.width  + j) = RGB565(i, j, i);
            *((BYTE *)mybmp888.pdata  + (i * mybmp888.width  + j) * 3 + 0) = (BYTE)i;
            *((BYTE *)mybmp888.pdata  + (i * mybmp888.width  + j) * 3 + 1) = (BYTE)j;
            *((BYTE *)mybmp888.pdata  + (i * mybmp888.width  + j) * 3 + 2) = (BYTE)i;
            *((DWORD*)mybmp8888.pdata + i * mybmp8888.width + j) = RGB888(i, j, i);
        }
    }

    unlockbmp(&mybmp8888);
    unlockbmp(&mybmp888 );
    unlockbmp(&mybmp565 );
    unlockbmp(&mybmp332 );

    savebmp(&mybmp332 , "332.bmp",  NULL);
    savebmp(&mybmp565 , "565.bmp",  NULL);
    savebmp(&mybmp888 , "888.bmp",  NULL);
    savebmp(&mybmp8888, "8888.bmp", NULL);

    destroybmp(&mybmp332 );
    destroybmp(&mybmp565 );
    destroybmp(&mybmp888 );
    destroybmp(&mybmp8888);
    return 0;
}
#endif




