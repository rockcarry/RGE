#ifndef __RGE_PCX_H__
#define __RGE_PCX_H__

/* ����ͷ�ļ� */
#include "bmp.h"
#include "fio.h"

/* note: now only surpport 256 color pcx images */

/* ���Ͷ��� */
/* PCX ������ */
typedef struct {
    WORD  width;     /* pcx �Ŀ�� */
    WORD  height;    /* pcx �ĸ߶� */
    DWORD datasize;  /* pcx �����ݳ��� */
    BYTE  encoding;  /* pcx �ı��뷽ʽ */
    BYTE *pdata;     /* ָ������ */
    BYTE *ppal;      /* ָ���ɫ�� */
} PCX;

/* �������� */
BOOL createpcx (PCX *pcx);
void destroypcx(PCX *pcx);
BOOL loadpcx(PCX *pcx, char *file, FIO *fio);
BOOL savepcx(PCX *pcx, char *file, FIO *fio);
BOOL encodepcx(PCX *pcx, BMP *pb);
BOOL decodepcx(PCX *pcx, BMP *pb);

#endif





















