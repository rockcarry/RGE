#ifndef _TEST_

/* 包含头文件 */
#include "pcx.h"

/* 内部类型定义 */
/* PCX 文件结构定义 */
typedef struct
{
    BYTE  manufacturer;
    BYTE  version;
    BYTE  encoding;
    BYTE  bitsperpixel;
    WORD  xmin;
    WORD  ymin;
    WORD  xmax;
    WORD  ymax;
    WORD  hdpi;
    WORD  vdpi;
    BYTE  colormap[48];
    BYTE  reserved;
    BYTE  nplanes;
    WORD  bytesperline;
    WORD  paletteinfo;
    WORD  hscreensize;
    WORD  vscreensize;
    BYTE  filler[54];
    BYTE *pdata;
    BYTE *ppal;
} PCXFILE, *PPCXFILE;

/* 函数实现 */
BOOL createpcx(PCX *pcx)
{
    WORD bytesperline = (pcx->width + 3) / 4 * 4;

    if (pcx->datasize == 0) {
        pcx->datasize = bytesperline * pcx->height;
    }

    pcx->pdata = (BYTE*)malloc(pcx->datasize);
    if (!pcx->pdata) return FALSE;

    pcx->ppal = (BYTE*)malloc(256 * 3);
    if (!pcx->ppal) return FALSE;

    return TRUE;
}

void destroypcx(PCX *pcx)
{
    if (pcx->pdata)
    {
        free(pcx->pdata);
        pcx->pdata = NULL;
    }
    if (pcx->ppal)
    {
        free(pcx->ppal);
        pcx->ppal = NULL;
    }
}

BOOL loadpcx(PCX *pcx, char *file, FIODRV *fdrv)
{
    PCXFILE pf;
    void   *fp;

    if (!pcx ) return FALSE;
    if (!fdrv) fdrv = &DEF_FIO_DRV;

    /* 打开文件 */
    fp = fdrv->open(file, "rb");
    if (!fp) return FALSE;

    /* 读入文件头 */
    fdrv->read(&pf, 128, 1, fp);

    /* only support 256 colors pcx */
    if (pf.bitsperpixel != 8) return FALSE;

    /* 创建 pcx 对象 */
    pcx->width    = pf.xmax - pf.xmin + 1;
    pcx->height   = pf.ymax - pf.ymin + 1;
    pcx->encoding = pf.encoding;

    fdrv->seek(fp, -256 * 3, SEEK_END);
    pcx->datasize = fdrv->tell(fp) - 128;
    if (!createpcx(pcx)) return FALSE;

    /* 读取调色板数据 */
    fdrv->read(pcx->ppal, 256 * 3, 1, fp);

    /* 读取图像数据 */
    fdrv->seek(fp, 128, SEEK_SET);
    fdrv->read(pcx->pdata, pcx->datasize, 1, fp);

    if (fp) fdrv->close(fp);
    return TRUE;
}

BOOL savepcx(PCX *pcx, char *file, FIODRV *fdrv)
{
    PCXFILE pf = {0};
    void   *fp;

    if (!pcx ) return FALSE;
    if (!fdrv) fdrv = &DEF_FIO_DRV;

    /* 打开文件 */
    fp = fdrv->open(file, "wb");
    if (!fp) return FALSE;

    /* 填充 pcx 文件头 */
    pf.manufacturer = 10;
    pf.version      = 5;
    pf.encoding     = pcx->encoding;
    pf.bitsperpixel = 8;
    pf.xmin         = 0;
    pf.ymin         = 0;
    pf.xmax         = pcx->width  - 1;
    pf.ymax         = pcx->height - 1;
    pf.nplanes      = 1;
    pf.bytesperline = (pcx->width + 3) / 4 * 4;
    pf.paletteinfo  = 1;

    /* 写入文件头 */
    fdrv->write(&pf, 128, 1, fp);

    /* 写入图像数据 */
    if (pcx->pdata) fdrv->write(pcx->pdata, pcx->datasize, 1, fp);

    /* 写入调色板数据 */
    if (pcx->ppal ) fdrv->write(pcx->ppal, 256 * 3, 1, fp);

    /* 关闭文件 */
    if (fp) fdrv->close(fp);
    return TRUE;
}

BOOL encodepcx(PCX *pcx, BMP *pb)
{
    int   numbyte;
    BYTE  lastbyte;
    BYTE  curbyte;
    BYTE *pbdata;
    BYTE *ppdata;
    BYTE *ppend ;
    BYTE  pal[256*4];
    int   bstride;
    int   pstride;
    int   i, j;

    if (!pcx || !pb || pb->cdepth != 8) return FALSE;

    memset(pcx, 0, sizeof(PCX));
    pcx->width    = pb->width;
    pcx->height   = pb->height;
    pcx->encoding = 1;
    if (!createpcx(pcx)) return FALSE;

    pbdata  = pb ->pdata;
    ppdata  = pcx->pdata;
    ppend   = ppdata + pcx->datasize;
    bstride = pb->stride;
    pstride = (pcx->width + 3) / 4 * 4;

    for (i=0; i<pb->height; i++)
    {
        numbyte  = 1;
        lastbyte = *pbdata++;
        for (j=1; j<pstride; j++)
        {
            curbyte = *pbdata++;
            if (curbyte == lastbyte)
            {
                if (numbyte == 0x3f)
                {
                    if (ppdata >= ppend) return FALSE;
                    *ppdata++ = numbyte + 0xc0;
                    if (ppdata >= ppend) return FALSE;
                    *ppdata++ = lastbyte;
                    numbyte = 1;
                }
                else numbyte++;
            }
            else
            {
                if (numbyte > 1 || lastbyte >= 0xc0)
                {
                    if (ppdata >= ppend) return FALSE;
                    *ppdata++ = numbyte + 0xc0;
                }
                if (ppdata >= ppend) return FALSE;
                *ppdata++ = lastbyte;
                lastbyte  = curbyte;
                numbyte   = 1;
            }
        }

        pbdata -= pstride;
        pbdata += bstride;

        if (numbyte > 1 || lastbyte >= 0xc0)
        {
            if (ppdata >= ppend) return FALSE;
            *ppdata++ = numbyte + 0xc0;
        }
        if (ppdata >= ppend) return FALSE;
        *ppdata++ = lastbyte;
    }

    /* 12 */
    if (ppdata >= ppend) return FALSE;
    *ppdata++ = 12;

    /* datasize */
    pcx->datasize = ppdata - pcx->pdata;

    getbmppal(pb, 0, 256, pal);
    for (i=0; i<256; i++)
    {
        pcx->ppal[i * 3 + 0] = pal[i * 4 + 2];
        pcx->ppal[i * 3 + 1] = pal[i * 4 + 1];
        pcx->ppal[i * 3 + 2] = pal[i * 4 + 0];
    }
    return TRUE;
}

BOOL decodepcx(PCX *pcx, BMP *pb)
{
    BYTE  byte1;
    BYTE  byte2;
    BYTE *ppdata;
    BYTE *pbdata;
    BYTE  pal[256*4];
    int   i, j, k;

    if (!pcx || !pb) return FALSE;

    memset(pb, 0, sizeof(BMP));
    pb->width  = pcx->width;
    pb->height = pcx->height;
    pb->cdepth = 8;
    if (!createbmp(pb)) return FALSE;

    i = j = 0;
    ppdata = pcx->pdata;
    pbdata = pb ->pdata;
    while (i < pb->height)
    {
        byte1 = *ppdata++;
        if (byte1 > 0xc0 && pcx->encoding == 1)
        {
            byte2 = *ppdata++;
            for (k=0; k<byte1-0xc0; k++)
            {
                *pbdata++ = byte2; j++;
                if (j >= pb->width) {
                    pbdata -= pb->width;
                    pbdata += pb->stride;
                    j = 0;
                    i++;
                }
            }
        }
        else
        {
            *pbdata++ = byte1; j++;
            if (j >= pb->width) {
                pbdata -= pb->width;
                pbdata += pb->stride;
                j = 0;
                i++;
            }
        }
    }

    for (i=0; i<256; i++)
    {
        pal[i * 4 + 2] = pcx->ppal[i * 3 + 0];
        pal[i * 4 + 1] = pcx->ppal[i * 3 + 1];
        pal[i * 4 + 0] = pcx->ppal[i * 3 + 2];
    }
    setbmppal(pb, 0, 256, pal);
    return TRUE;
}

#else
#include "win.h"
#include "pcx.h"
#include "bitblt.h"

int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPreInst, LPSTR lpszCmdLine, int nCmdShow)
{
    PCX mypcx1 = {0};
    PCX mypcx2 = {0};
    BMP mybmp  = {0};

    if (*lpszCmdLine == 0) {
        lpszCmdLine = "res/me.pcx";
    }

    RGE_WIN_INIT(hInst);

    loadpcx(&mypcx1, lpszCmdLine, NULL);
    decodepcx(&mypcx1, &mybmp);

    SCREEN.width  = 640;
    SCREEN.height = 480;
    SCREEN.cdepth = 8;
    createbmp(&SCREEN);
    setbmppal(&SCREEN, 0, 256, mybmp.ppal);

    putbmppalmap(&SCREEN,
                 (SCREEN.width  - mybmp.width ) / 2,
                 (SCREEN.height - mybmp.height) / 2,
                 &mybmp, NULL);
    encodepcx(&mypcx2, &SCREEN);

    RGE_MSG_LOOP();
    savepcx(&mypcx2, "screen.pcx", NULL);
    destroypcx(&mypcx1);
    destroypcx(&mypcx2);
    destroybmp(&mybmp);
    destroybmp(&SCREEN);
    return 0;
}
#endif













