#ifndef _PCX_H_
#define _PCX_H_

/* ����ͷ�ļ� */
#include "bmp.h"
#include "fiodrv.h"

/* note: now only surpport 256 color pcx images */

/* ���Ͷ��� */
/* PCX ������ */
typedef struct
{
    WORD  width;     /* pcx �Ŀ�� */
    WORD  height;    /* pcx �ĸ߶� */
    DWORD datasize;  /* pcx �����ݳ��� */
    BYTE  encoding;  /* pcx �ı��뷽ʽ */
    BYTE *pdata;     /* ָ������ */
    BYTE *ppal;      /* ָ���ɫ�� */
} PCX;

/* �������� */
BOOL createpcx(PCX *pcx);
void destroypcx(PCX *pcx);
BOOL loadpcx(PCX *pcx, char *file, FIODRV *fdrv);
BOOL savepcx(PCX *pcx, char *file, FIODRV *fdrv);
BOOL encodepcx(PCX *pcx, BMP *pb);
BOOL decodepcx(PCX *pcx, BMP *pb);

#endif





















