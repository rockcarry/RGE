#ifndef __RGE_BMP_H__
#define __RGE_BMP_H__

/* ����ͷ�ļ� */
#include "stdefine.h"
#include "fio.h"

/* ���Ͷ��� */
/* BMP ��������Ͷ��� */
typedef struct {
    int    width;      /* ��� */
    int    height;     /* �߶� */
    int    cdepth;     /* ����λ��� */
    int    stride;     /* �п��ֽ��� */
    RECT   clipper;    /* �ü��� */
    BYTE  *pdata;      /* ָ��ͼ������ */
    BYTE  *ppal;       /* ָ��ɫ������ */
    void  *pextra;     /* ָ�򸽼����� */

    BOOL (*createbmp )(void *pb);
    void (*destroybmp)(void *pb);
    void (*lock  )(void *pb);
    void (*unlock)(void *pb);
    void (*setpal)(void *pb, int i, int n, BYTE *pal);
    void (*getpal)(void *pb, int i, int n, BYTE *pal);
} BMP;

#define RGB332(r, g, b)  ( ((r) >> 5 << 5 ) | ((g) >> 5 << 2) | ((b) >> 6 << 0) )
#define RGB565(r, g, b)  ( ((r) >> 3 << 11) | ((g) >> 2 << 5) | ((b) >> 3 << 0) )
#define RGB888(r, g, b)  ( ((r) << 16) | ((g) << 8) | ((b) << 0) )
#define ARGB(a, r, g, b) ( ((DWORD)(a) << 24) | ((r) << 16) | ((g) << 8) | ((b) << 0) )
DWORD COLOR_CONVERT(int cdepth, DWORD color);

/* �������� */
/* ���������� */
BOOL createbmp (BMP *pb);
void destroybmp(BMP *pb);

/* lock & unlock */
void lockbmp  (BMP *pb);
void unlockbmp(BMP *pb);

/* ��ɫ����� */
void setbmppal(BMP *pb, int i, int n, BYTE *pal);
void getbmppal(BMP *pb, int i, int n, BYTE *pal);

/* �ü��� */
void setclipper(BMP *pb, int left, int top, int right, int bottom);

BOOL loadbmp(BMP *pb, char *file, FIO *fio);
BOOL savebmp(BMP *pb, char *file, FIO *fio);

#endif


