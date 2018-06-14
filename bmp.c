#ifndef _TEST_

/* ����ͷ�ļ� */
#include <stdlib.h>
#include "pal.h"
#include "bmp.h"

/* �������� */
/* Ĭ��λͼ����λ��� */
#define DEF_BMP_WIDTH   640
#define DEF_BMP_HEIGHT  480
#define DEF_BMP_CDEPTH  16

/* ����ʵ�� */
/* ����һ�� BMP ����ĺ��� */
BOOL createbmp(BMP *pb)
{
    /* ʹ��Ĭ��ֵ */
    if (pb->width  == 0) pb->width  = DEF_BMP_WIDTH;
    if (pb->height == 0) pb->height = DEF_BMP_HEIGHT;
    if (pb->cdepth == 0) pb->cdepth = DEF_BMP_CDEPTH;

    /* ����Ĭ�ϵĲü��� */
    if (  pb->clipper.left   == 0
       && pb->clipper.right  == 0
       && pb->clipper.bottom == 0
       && pb->clipper.top    == 0)
    {
        setclipper(pb, 0, 0, -1, -1);
    }

    /* ���� BMP ����� stride */
    pb->stride  = pb->width;
    pb->stride *= pb->cdepth;
    pb->stride /= 8;
    pb->stride += 3;
    pb->stride /= 4;
    pb->stride *= 4;

    /* ������ɫ�建���� */
    if (pb->cdepth == 8) {
        pb->ppal = (BYTE*)malloc(256 * 4);
        if (!pb->ppal) return FALSE;
        else createstdpal(pb->ppal);
    }

    /* ʹ�� BMPDRV ����λͼ */
    if (pb->createbmp) {
        return pb->createbmp(pb);
    } else {
        pb->pdata = (BYTE*)malloc((size_t)pb->stride * pb->height);
        return pb->pdata ? TRUE : FALSE;
    }
}

/* ����һ�� BMP ���� */
void destroybmp(BMP *pb)
{
    /* ʹ�� BMPDRV ����λͼ */
    if (pb->destroybmp) {
        pb->destroybmp(pb);
    }

    /* �ͷŵ�ɫ�建���� */
    if (pb->ppal) {
        free(pb->ppal);
        pb->ppal = NULL;
    }

    /* �ͷ�λͼ���ݻ����� */
    if (pb->pdata) {
        free(pb->pdata);
        pb->pdata = NULL;
    }
}

/* lock & unlock */
void lockbmp(BMP *pb)
{
    if (pb->lock) {
        pb->lock(pb);
    }
}

void unlockbmp(BMP *pb)
{
    if (pb->unlock) {
        pb->unlock(pb);
    }
}

/* ���� BMP �ĵ�ɫ�� */
void setbmppal(BMP *pb, int i, int n, BYTE *pal)
{
    BYTE *psrc =      pal + i * 4;
    BYTE *pdst = pb->ppal + i * 4;

    memcpy(pdst, psrc, n * 4);

    /* ʹ�� BMPDRV ���õ�ɫ�� */
    if (pb->setpal) {
        pb->setpal(pb, i, n, pb->ppal);
    }
}

/* ��ȡ BMP �ĵ�ɫ�� */
void getbmppal(BMP *pb, int i, int n, BYTE *pal)
{
    BYTE *psrc = pb->ppal + i * 4;
    BYTE *pdst =      pal + i * 4;

    /* ʹ�� BMPDRV ��ȡ��ɫ�� */
    if (pb->getpal) {
        pb->getpal(pb, i, n, pb->ppal);
    }

    memcpy(pdst, psrc, n * 4);
}

/* ���� BMP ����Ĳü��� */
void setclipper(BMP *pb, int left, int top, int right, int bottom)
{
    if (right  == -1) right  += pb->width;
    if (bottom == -1) bottom += pb->height;
    pb->clipper.left   = left;
    pb->clipper.right  = right;
    pb->clipper.top    = top;
    pb->clipper.bottom = bottom;
}

/* �ڲ����Ͷ��� */
/* BMP �ļ��ṹ���Ͷ��� */
#pragma pack(1)
typedef struct {
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
} BMPFILE;
#pragma pack()

/* ����ʵ�� */
/* װ�� BMP �ļ��� BMP ���� */
BOOL loadbmp(BMP *pb, char *file, FIO *fio)
{
    BMPFILE bmpfile;
    void   *fp;
    BYTE   *pdest;
    int     bstride;
    int     fstride;
    int     i;

    if (!pb ) return FALSE;
    if (!fio) fio = &DEF_FIO;

    /* ���ļ� */
    fp = fio->open(file, "rb");
    if (!fp) return FALSE;

    /* ��ȡ�ļ�ͷ */
    fio->read(&bmpfile, sizeof(bmpfile), 1, fp);
    if (bmpfile.filetype != (('B' << 0) | ('M' << 8))) return FALSE;
    pb->cdepth = bmpfile.bitsperpixel;
    pb->width  = (int)bmpfile.imagewidth;
    pb->height = (int)bmpfile.imageheight;

    /* Ŀǰֻ֧�� 256 ɫ��16/24/32bit ɫ�� BMP ͼ�� */
    if (  pb->cdepth != 8
       && pb->cdepth != 16
       && pb->cdepth != 24
       && pb->cdepth != 32)
    {
        return FALSE;
    }

    /* ���� BMP ���� */
    if (!createbmp(pb)) return FALSE;

    /* ��ȡ��ɫ������ */
    if (pb->cdepth == 8) {
        fio->seek(fp, bmpfile.dataoffset - 256 * 4, SEEK_SET);
        fio->read(pb->ppal, 4, 256, fp);
        setbmppal(pb, 0, 256, pb->ppal);
    }

    bstride = pb->stride;
    fstride = (pb->width * (pb->cdepth / 8) + 3) / 4 * 4;

    /* ��ʼ��ָ�� */
    pdest = pb->pdata + bstride * (pb->height - 1);

    /* ��ȡͼ������ */
    fio->seek(fp, bmpfile.dataoffset, SEEK_SET);
    for (i=0; i<pb->height; i++) {
        fio->read(pdest, fstride, 1, fp);
        pdest -= bstride;
    }

    if (fp) fio->close(fp); /* �ر��ļ� */
    return TRUE;
}

/* ���� BMP ���� BMP �ļ� */
BOOL savebmp(BMP *pb, char *file, FIO *fio)
{
    BMPFILE bmpfile = {0};
    DWORD   mask[3] = { 0xF800, 0x07E0, 0x001F };
    void   *fp;
    BYTE   *psrc;
    int     bstride;
    int     fstride;
    int     i;

    if (!pb ) return FALSE;
    if (!fio) fio = &DEF_FIO;

    bstride = pb->stride;
    fstride = (pb->width * (pb->cdepth / 8) + 3) / 4 * 4;

    /* ��� BMPFILE �ṹ */
    bmpfile.filetype     = ('B' << 0) | ('M' << 8);
    bmpfile.dataoffset   = sizeof(bmpfile);
    bmpfile.imagesize    = fstride * pb->height;
    bmpfile.filesize     = bmpfile.dataoffset + bmpfile.imagesize;
    bmpfile.infosize     = 40;
    bmpfile.imagewidth   = pb->width;
    bmpfile.imageheight  = pb->height;
    bmpfile.planes       = 1;
    bmpfile.bitsperpixel = pb->cdepth;

    if (bmpfile.bitsperpixel == 8) {
        bmpfile.dataoffset += 256 * 4;
        bmpfile.filesize   += 256 * 4;
    }

    if (bmpfile.bitsperpixel == 16) {
        bmpfile.dataoffset += sizeof(DWORD) * 3;
        bmpfile.filesize   += sizeof(DWORD) * 3;
        bmpfile.compression = 3; // BI_BITFIELDS
    }

    /* ���ļ� */
    fp = fio->open(file, "wb");
    if (!fp) return FALSE;

    /* д���ļ�ͷ */
    fio->write(&bmpfile, sizeof(bmpfile), 1, fp);

    /* д���ɫ������ */
    if (bmpfile.bitsperpixel == 8) {
        getbmppal(pb, 0, 256, pb->ppal);
        fio->write(pb->ppal, 4, 256, fp);
    }

    /* for 16bit RGB565 bmp file*/
    if (bmpfile.bitsperpixel == 16) fio->write(mask, sizeof(mask), 1, fp);

    /* ��ʼ��ָ�� */
    psrc = pb->pdata + bstride * (pb->height - 1);

    /* д��ͼ������ */
    for (i=0; i<pb->height; i++) {
        fio->write(psrc, fstride, 1, fp);
        psrc -= bstride;
    }

    /* �ر��ļ� */
    if (fp) fio->close(fp);
    return TRUE;
}

DWORD COLOR_CONVERT(int cdepth, DWORD color)
{
    int r, g, b;

    r = (color >> 16) & 0xff;
    g = (color >> 8 ) & 0xff;
    b = (color >> 0 ) & 0xff;

    switch (cdepth) {
    case 8 : return RGB332(r, g, b);
    case 16: return RGB565(r, g, b);
    case 24: return RGB888(r, g, b);
    case 32: return RGB888(r, g, b);
    default: return (DWORD)-1;
    }
}

#else
/* ����ͷ�ļ� */
#include "bmp.h"
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

    for (i=0; i<mybmp332.height; i++) {
        for (j=0; j<mybmp332.width; j++) {
            *((BYTE *)mybmp332.pdata  + i * mybmp332.width  + j) = RGB332((BYTE)i, (BYTE)j, (BYTE)i);
            *((WORD *)mybmp565.pdata  + i * mybmp565.width  + j) = RGB565((BYTE)i, (BYTE)j, (BYTE)i);
            *((BYTE *)mybmp888.pdata  + (i * mybmp888.width  + j) * 3 + 0) = (BYTE)i;
            *((BYTE *)mybmp888.pdata  + (i * mybmp888.width  + j) * 3 + 1) = (BYTE)j;
            *((BYTE *)mybmp888.pdata  + (i * mybmp888.width  + j) * 3 + 2) = (BYTE)i;
            *((DWORD*)mybmp8888.pdata + i * mybmp8888.width + j) = RGB888((BYTE)i, (BYTE)j, (BYTE)i);
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