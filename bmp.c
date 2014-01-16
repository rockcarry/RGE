/* ����ͷ�ļ� */
#include <stdlib.h>
#include "palette.h"
#include "bmp.h"

/*++ default bmp driver ++*/
static BOOL _defdrv_createbmp(void *pb)
{
    BMP *pbmp = (BMP*)pb;
    pbmp->pdata = (BYTE*)malloc((size_t)pbmp->stride * pbmp->height);
    if (!pbmp->pdata) return FALSE;
    else return TRUE;
}

static void _defdrv_destroybmp(void *pb)
{
    BMP *pbmp = (BMP*)pb;
    if (!pbmp->pdata)
    {
        free(pbmp->pdata);
        pbmp->pdata = NULL;
    }
}

static void _defdrv_lock  (void *pb) {}
static void _defdrv_unlock(void *pb) {}
static void _defdrv_setpal(void *pb, int i, int n, BYTE *pal) {}
static void _defdrv_getpal(void *pb, int i, int n, BYTE *pal) {}

/* ȫ�ֱ������� */
static BMPDRV DEF_BMP_DRV =
{
    _defdrv_createbmp,
    _defdrv_destroybmp,
    _defdrv_lock,
    _defdrv_unlock,
    _defdrv_setpal,
    _defdrv_getpal,
};
/*-- default bmp driver --*/

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
    if (pb->pbmpdrv == NULL) pb->pbmpdrv =&DEF_BMP_DRV;
    if (pb->width   == 0   ) pb->width   = DEF_BMP_WIDTH;
    if (pb->height  == 0   ) pb->height  = DEF_BMP_HEIGHT;
    if (pb->cdepth  == 0   ) pb->cdepth  = DEF_BMP_CDEPTH;

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
    if (pb->cdepth == 8)
    {
        pb->ppal = (BYTE*)malloc(256 * 4);
        if (!pb->ppal) return FALSE;
        else createstdpal(pb->ppal);
    }

    /* ʹ�� BMPDRV ����λͼ */
    return pb->pbmpdrv->createbmp(pb);
}

/* ����һ�� BMP ���� */
void destroybmp(BMP *pb)
{
    /* ʹ�� BMPDRV ����λͼ */
    if (pb->pbmpdrv) pb->pbmpdrv->destroybmp(pb);

    /* �ͷŵ�ɫ�建���� */
    if (pb->ppal)
    {
        free(pb->ppal);
        pb->ppal = NULL;
    }
}

/* lock & unlock */
void lockbmp(BMP *pb)
{
    pb->pbmpdrv->lock(pb);
}

void unlockbmp(BMP *pb)
{
    pb->pbmpdrv->unlock(pb);
}

/* ���� BMP �ĵ�ɫ�� */
void setbmppal(BMP *pb, int i, int n, BYTE *pal)
{
    BYTE *psrc =      pal + i * 4;
    BYTE *pdst = pb->ppal + i * 4;

    memcpy(pdst, psrc, n * 4);

    /* ʹ�� BMPDRV ���õ�ɫ�� */
    pb->pbmpdrv->setpal(pb, i, n, pb->ppal);
}

/* ��ȡ BMP �ĵ�ɫ�� */
void getbmppal(BMP *pb, int i, int n, BYTE *pal)
{
    BYTE *psrc = pb->ppal + i * 4;
    BYTE *pdst =      pal + i * 4;

    /* ʹ�� BMPDRV ��ȡ��ɫ�� */
    pb->pbmpdrv->getpal(pb, i, n, pb->ppal);

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


